#include<process.h>
#include"UdpNet.h"
#include"PackDef.h"
#include"UdpMediator.h"
#include<iostream>
using namespace std;
INet_UDP::INet_UDP(InetMediator* Mediator):m_socket(INVALID_SOCKET), m_handle(0), m_isStop(false)
{
	m_pMediator = Mediator;    
}
INet_UDP::~INet_UDP()
{
    UnInitNet();
}


bool INet_UDP::InitNet()
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
        cout<<"WSAStartup failed with error: %d\n"<<err<<endl;
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        cout<<"Could not find a usable version of Winsock.dll\n"<<endl;
        UnInitNet();
        return false;
    }
    else {
        cout<<"The Winsock 2.2 dll was found okay\n"<<endl;
    }

    //1.创建套接字
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) {
        cout << "socket error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "socket successed" << endl;
    }
    BOOL bval = TRUE;
    setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&bval, sizeof(bval));
    //2.绑定
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = htons(_DEF_UDP_PORT);
    if (bind(m_socket, (const sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cout << "bind error=" << WSAGetLastError();
        UnInitNet();
        return false;
    }
    else {
        cout << "bind successed" << endl;
    }
    //4.接受数据--阻塞函数，创建线程
    m_handle =(HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    return true;
}

unsigned int __stdcall INet_UDP::RecvThread(void* lpVoid) {
    INet_UDP* pThis = (INet_UDP*)lpVoid;
    pThis->RecvData();
    return 0;
}

bool INet_UDP::SendData(long lSendip, char* buf, int nLen)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = lSendip;
    addr.sin_port = htons(_DEF_UDP_PORT);
    if (sendto(m_socket, buf, nLen, 0, (sockaddr*)&addr, sizeof(addr)) <= 0) {
        cout << "sendto error" << WSAGetLastError() << endl;
        return false;
    }
	return true;
}

bool INet_UDP::RecvData()
{
    char buf[_DEF_BUF_SIZE] = "";
    sockaddr_in addrclient;
    int nSize = sizeof(addrclient);
    m_isStop = false;
    int nRecvNum=0;
    while (!m_isStop) {
        nRecvNum = recvfrom(m_socket, buf, _DEF_BUF_SIZE, 0, (sockaddr*)&addrclient, &nSize);
        if (nRecvNum > 0) {
            char *pack = new char[nRecvNum];
            if (pack) {
                memcpy(pack,buf, nRecvNum);
                m_pMediator->DealData(m_socket,pack, nRecvNum);
            }
        }
        else {
            cout << "recvfrom error" << WSAGetLastError() << endl;
        }
    }
	return true;   
}

void INet_UDP::UnInitNet()
{
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle,100)) {
            TerminateThread(m_handle,-1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
    WSACleanup();

}
