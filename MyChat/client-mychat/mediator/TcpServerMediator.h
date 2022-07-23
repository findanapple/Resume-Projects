#pragma once
#include"INetMediator.h"

class TcpServerMediator :public InetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
public:
	virtual bool OpenNet();
	virtual bool SendData(long lSendip, char* buf, int nLen);
	virtual void CloseNet();
	virtual void DealData(long lSendip, char* buf, int nLen);
};
