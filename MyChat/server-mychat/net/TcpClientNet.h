#pragma once
#include"INet.h"

class TcpClientNet :public INet
{
public:
	TcpClientNet(InetMediator* Mediator);
	~TcpClientNet();
public:
	//��ʼ������
	virtual bool InitNet();
	//��������
	virtual bool SendData(long lSendip, char* buf, int nLen);
	//�ر�����
	virtual void UnInitNet();
	static unsigned int __stdcall RecvThread(void* lpVoid);
protected:
	//��������
	virtual bool RecvData();
	SOCKET m_socket;
	HANDLE m_handle;
	bool m_isStop;
};
