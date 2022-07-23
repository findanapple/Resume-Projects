#pragma once
#include<QObject>
class INet;
class InetMediator:public QObject
{
    Q_OBJECT
protected:
	INet* m_pNet;
public:
    InetMediator();
    virtual ~InetMediator();
public:
	virtual bool OpenNet() = 0;
	virtual bool SendData(long lSendip, char* buf, int nLen) = 0;
	virtual void CloseNet() = 0;
    virtual void DealData(long lSendip, char* buf, int nLen) = 0;
signals:
    void SIG_ReadyData(long lSendip, char* buf, int nLen);
};

