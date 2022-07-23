#pragma once
#include"INet.h"
#include<list>
#include<map>
class TcpServerNet :public INet
{
public:
	TcpServerNet(InetMediator* Mediator);
	~TcpServerNet();
public:
	//初始化网络
	virtual bool InitNet();
	//发送数据
	virtual bool SendData(long lSendip, char* buf, int nLen);
	//关闭网络
	virtual void UnInitNet();
	static unsigned int __stdcall RecvThread(void* lpVoid);
	static unsigned int __stdcall AcceptThread(void* lpVoid);
protected:
	//接收数据
	virtual bool RecvData();
	SOCKET m_socket;//用于接受连接的socket
	list<HANDLE> m_hThreadHandleList;//保存线程句柄
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;  //保存客户端对应socket的映射关系
	bool m_isStop;
};
