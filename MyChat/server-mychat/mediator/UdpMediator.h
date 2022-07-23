#pragma once
#include"INetMediator.h"

class INetMediator_UDP :public InetMediator
{
public:
	INetMediator_UDP();
	~INetMediator_UDP();
public:
	virtual bool OpenNet();
	virtual bool SendData(long lSendip, char* buf, int nLen);
	virtual void CloseNet();
	virtual void DealData(long lSendip, char* buf, int nLen);
};
