#pragma once
#include"INet.h"

class INet_UDP:public INet
{
public:
	INet_UDP(InetMediator* Mediator);
	~INet_UDP();
public:
	//��ʼ������
	virtual bool InitNet();
	//��������
	virtual bool SendData(long lSendip,char* buf, int nLen);
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
