#pragma once
#include"INetMediator.h"

class TcpClientMediator :public InetMediator
{
    Q_OBJECT
signals:
    void SIG_ReadyData(long lSendip, char* buf, int nLen);
public:
	TcpClientMediator();
	~TcpClientMediator();
public:
	virtual bool OpenNet();
	virtual bool SendData(long lSendip, char* buf, int nLen);
	virtual void CloseNet();
	virtual void DealData(long lSendip, char* buf, int nLen);
};
