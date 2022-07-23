#include<process.h>
#include"TcpClientNet.h"
#include"PackDef.h"
#include"TcpClientMediator.h"

TcpClientNet::TcpClientNet(InetMediator* Mediator) :m_socket(INVALID_SOCKET), m_handle(0), m_isStop(false)
{
    m_pMediator = Mediator;
}
TcpClientNet::~TcpClientNet()
{
    UnInitNet();
}


bool TcpClientNet::InitNet()
{
    //0.加载库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        cout << "WSAStartup failed with error: %d\n" << err << endl;
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        cout << "Could not find a usable version of Winsock.dll\n" << endl;
        UnInitNet();
        return false;
    }
    else {
        cout << "The Winsock 2.2 dll was found okay\n" << endl;
    }

    //1.创建套接字
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        cout << "socket error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "socket successed" << endl;
    }

    //2.绑定
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_DEF_TCP_PORT);
    server_addr.sin_addr.S_un.S_addr = inet_addr(_DEF_SERVER_IP);
    
    err = connect(m_socket, (const sockaddr*)&server_addr, sizeof(server_addr));
    if (err == SOCKET_ERROR) {
        cout << "connect error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "connect successed" << endl;
    }
    //4.接受数据--阻塞函数，创建线程
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    return true;
}

unsigned int __stdcall TcpClientNet::RecvThread(void* lpVoid) {
    TcpClientNet* pThis = (TcpClientNet*)lpVoid;
    pThis->RecvData();
    return 0;
}

bool TcpClientNet::SendData(long lSendip, char* buf, int nLen)
{
    //1.判断传入参数是否合法
    if (!buf || nLen <= 0) {
        return false;
    }
    //2.防止粘包，先发包大小，再发包内容
    if (send(m_socket,(char *)&nLen,sizeof(int),0) <= 0) {
        return false;
    }
    if (send(m_socket, buf, nLen, 0) <= 0) {
        return false;
    }
    return true;
}

bool TcpClientNet::RecvData()
{

    int packsize = 0;
    int nRecvNum = 0;
    int offset = 0;
    while (!m_isStop) {
        nRecvNum = recv(m_socket, (char*)&packsize, sizeof(int), 0);
        if (nRecvNum <= 0) break;
        char* recvBuf = new char[packsize];
        while (packsize) {
            nRecvNum = recv(m_socket, recvBuf + offset, packsize, 0);
            if (nRecvNum > 0) {
                offset += nRecvNum;
                packsize -= packsize;
            }               
        }
        
        this->m_pMediator->DealData(m_socket, recvBuf, offset);
        offset = 0;
        //todo  回收recvbuf

    }
    return true;
}

void TcpClientNet::UnInitNet()
{
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
    WSACleanup();

}