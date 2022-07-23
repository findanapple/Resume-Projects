 #include<process.h>
#include"TcpServerNet.h"
#include"PackDef.h"
#include"TcpServerMediator.h"

TcpServerNet::TcpServerNet(InetMediator* Mediator) :m_socket(INVALID_SOCKET), m_hThreadHandleList(0), m_isStop(false)
{
    m_pMediator = Mediator;
}
TcpServerNet::~TcpServerNet()
{
    UnInitNet();
}


bool TcpServerNet::InitNet()
{
    //0.���ؿ�
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

    //1.�����׽���
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        cout << "socket error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "socket successed" << endl;
    }
 
    //2.��
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(_DEF_SERVER_IP);
    server_addr.sin_port = htons(_DEF_TCP_PORT);
    if (bind(m_socket, (const sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cout << "bind error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "bind successed" << endl;
    }
    //����
    listen(m_socket, 128);
    //�������ӡ������������������������������߳�
    HANDLE handle= (HANDLE)_beginthreadex(NULL, 0,
        &AcceptThread, this, 0, NULL);
    if (handle) {
        m_hThreadHandleList.push_back(handle);
    }
    return true;
}
unsigned int __stdcall TcpServerNet::AcceptThread(void* lpVoid) {
    TcpServerNet* pThis = (TcpServerNet*)lpVoid;
    sockaddr_in client_addr;
    int nSize = sizeof(client_addr);
    while (!pThis->m_isStop) {
        SOCKET sock = accept(pThis->m_socket, (sockaddr*)&client_addr, &nSize);
        cout << "ip=" << inet_ntoa(client_addr.sin_addr) << endl;

        //��������--���������������߳� 
        unsigned int nThreadId = 0;
        HANDLE handle = (HANDLE)_beginthreadex(NULL, 0,
            &RecvThread, pThis, 0, &nThreadId);
        if (handle) {
            pThis->m_hThreadHandleList.push_back(handle);
            pThis->m_mapThreadIdToSocket[nThreadId] = sock;
        }
    }
    
    pThis->RecvData();
    return 0;
}
unsigned int __stdcall TcpServerNet::RecvThread(void* lpVoid) {
    TcpServerNet* pThis = (TcpServerNet*)lpVoid;
    pThis->RecvData();
    return 0;
}

bool TcpServerNet::SendData(long lSendip, char* buf, int nLen)
{
    //lSendipװ����socket
    //�жϲ����Ƿ�Ϸ�
    if (!buf || nLen <= 0) {
        return false;
    }
    //�ȷ�����С�ٷ�������
    if (send(lSendip, (char*)&nLen, sizeof(int), 0) <= 0) {
        cout << "send packsize error" << WSAGetLastError() << endl;
        return false;
    }
    if (send(lSendip, buf, nLen, 0) <= 0) {
        cout << "send buf error" << WSAGetLastError() << endl;
        return false;
    }
    return true;
}

bool TcpServerNet::RecvData()
{
    //��ȡ��ͻ���ͨѶ��socket
    Sleep(100);//��Ϊ�̴߳���������ִ�У������߳�id��socket����map��Ҫʱ��ִ�У��������߽���ʱ��Ƭ�ֻ�
    SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
    //�ж�socket�Ϸ���
    if (!sock || sock == INVALID_SOCKET) {
        return false;
    }
    //��������
    int nPackSize = 0;
    int nRecvNum = 0;
    int offset = 0;
    while (!m_isStop) {
        nRecvNum = recv(sock, (char*)&nPackSize,sizeof(int),0);
        if (nRecvNum <= 0) {
            break;
        }
        char* RecvBuf = new char[nPackSize];
        while(nPackSize) {           
            nRecvNum = recv(sock, RecvBuf + offset, nPackSize, 0);
            if (nRecvNum>0) {
                offset += nRecvNum;
                nPackSize -= nRecvNum;
            }   
        }
        this->m_pMediator->DealData(sock,RecvBuf, offset);
        //todo:recvbuf�ǵû���
        offset = 0;

    }
    return true;
}

void TcpServerNet::UnInitNet()
{
    //�˳��߳�
    m_isStop = true;
    for (auto ite = m_hThreadHandleList.begin(); ite != m_hThreadHandleList.end();) {
        if (*ite) {
            if (WAIT_TIMEOUT == WaitForSingleObject(*ite, 100)) {
                TerminateThread(*ite, -1);
            }
            CloseHandle(*ite);
            *ite = NULL;
        }
        m_hThreadHandleList.erase(ite);
    }
    //�ر��׽���
    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
        if (ite->second&& INVALID_SOCKET!= ite->second) {
            closesocket(ite->second);
        }
        m_mapThreadIdToSocket.erase(ite);
    }
    //ж�ؿ�
    WSACleanup();
}