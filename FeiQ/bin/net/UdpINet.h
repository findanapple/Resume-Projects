#pragma once
#include"myINet.h"

class UdpINet:public INet
{
public:
    UdpINet(InetMediator* Mediator);
    ~UdpINet();
public:
	virtual bool InitNet();
	virtual bool SendData(long lSendip,char* buf, int nLen);
	virtual void UnInitNet();
	static unsigned int __stdcall RecvThread(void* lpVoid);
protected:
	virtual bool RecvData();
	SOCKET m_socket;
	HANDLE m_handle;
	bool m_isStop;
};
