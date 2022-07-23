#include"INetMediator_UDP.h"
#include"UdpINet.h"
#include <QDebug>
INetMediator_UDP::INetMediator_UDP()
{
    m_pNet = new UdpINet(this);
}
INetMediator_UDP::~INetMediator_UDP()
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool INetMediator_UDP::OpenNet()
{
    qDebug()<<__func__;
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
bool INetMediator_UDP::SendData(long lSendip, char* buf, int nLen)
{
    qDebug()<<__func__;
	if (!m_pNet->SendData(lSendip, buf,nLen)) {
		return false;
	}
	return true;
}
void INetMediator_UDP::CloseNet()
{
    qDebug()<<__func__;
	m_pNet->UnInitNet();
}
void INetMediator_UDP::DealData(long lSendip, char* buf, int nLen)
{
    qDebug()<<__func__;

	//todo 把数据发给kernel类
    Q_EMIT SIG_ReadyData(lSendip,buf,nLen);
}
