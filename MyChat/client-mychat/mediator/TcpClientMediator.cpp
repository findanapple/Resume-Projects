#include"TcpClientMediator.h"
#include"TcpClientNet.h"
TcpClientMediator::TcpClientMediator()
{
	m_pNet = new TcpClientNet(this);
}
TcpClientMediator::~TcpClientMediator()
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool TcpClientMediator::OpenNet()
{
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
bool TcpClientMediator::SendData(long lSendip, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendip, buf, nLen)) {
		return false;
	}
	return true;
}
void TcpClientMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
void TcpClientMediator::DealData(long lSendip, char* buf, int nLen)
{
	//cout << buf << endl;
	//todo 把数据发给kernel类
    Q_EMIT SIG_ReadyData(lSendip,buf,nLen);
}
