#include"TcpServerMediator.h"
#include"TcpServerNet.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServerNet(this);
} 
TcpServerMediator::~TcpServerMediator()
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool TcpServerMediator::OpenNet()
{
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
bool TcpServerMediator::SendData(long lSendip, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendip, buf, nLen)) {
		return false;
	}
	return true;
}
void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
void TcpServerMediator::DealData(long lSendip, char* buf, int nLen)
{

	SendData(lSendip, buf, nLen);
	//todo 把数据发给kernel类
}
