#pragma once
#include"INet.h"
#include<list>
#include<map>
class TcpServerNet :public INet
{
public:
	TcpServerNet(InetMediator* Mediator);
	~TcpServerNet();
public:
	//��ʼ������
	virtual bool InitNet();
	//��������
	virtual bool SendData(long lSendip, char* buf, int nLen);
	//�ر�����
	virtual void UnInitNet();
	static unsigned int __stdcall RecvThread(void* lpVoid);
	static unsigned int __stdcall AcceptThread(void* lpVoid);
protected:
	//��������
	virtual bool RecvData();
	SOCKET m_socket;//���ڽ������ӵ�socket
	list<HANDLE> m_hThreadHandleList;//�����߳̾��
	map<unsigned int, SOCKET>m_mapThreadIdToSocket;  //����ͻ��˶�Ӧsocket��ӳ���ϵ
	bool m_isStop;
};
