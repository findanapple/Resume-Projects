#pragma once
#include<iostream>
using namespace std;
#include<winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
class InetMediator;
class INet
{
public:
	INet():m_pMediator(0) {}
	virtual ~INet(){}
public:
	//��ʼ������
	virtual bool InitNet()=0;
	//��������
	virtual bool SendData(long lSendip, char* buf, int nLen)=0;
	//�ر�����
	virtual void UnInitNet()=0;
protected:
	//��������
	virtual bool RecvData()=0;
	InetMediator* m_pMediator;
};

