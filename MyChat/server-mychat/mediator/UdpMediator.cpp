#include"UdpMediator.h"
#include"UdpNet.h"
INetMediator_UDP::INetMediator_UDP()
{
	m_pNet = new INet_UDP(this);
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
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
bool INetMediator_UDP::SendData(long lSendip, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendip, buf,nLen)) {
		return false;
	}
	return true;
}
void INetMediator_UDP::CloseNet()
{
	m_pNet->UnInitNet();
}
void INetMediator_UDP::DealData(long lSendip, char* buf, int nLen)
{
	//cout << buf << endl;
	//todo 把数据发给kernel类
}