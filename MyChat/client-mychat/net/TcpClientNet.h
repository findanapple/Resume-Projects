#pragma once
#include"INet.h"

class TcpClientNet :public INet
{
public:
	TcpClientNet(InetMediator* Mediator);
	~TcpClientNet();
public:
	//初始化网络
	virtual bool InitNet();
	//发送数据
	virtual bool SendData(long lSendip, char* buf, int nLen);
	//关闭网络
	virtual void UnInitNet();
	static unsigned int __stdcall RecvThread(void* lpVoid);
protected:
	//接收数据
	virtual bool RecvData();
	SOCKET m_socket;
	HANDLE m_handle;
	bool m_isStop;
};