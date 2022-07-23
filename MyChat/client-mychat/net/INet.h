#pragma once
#include<winsock2.h>


//#pragma comment(lib,"ws2_32.lib")
class InetMediator;
class INet
{
public:
	INet():m_pMediator(0) {}
	virtual ~INet(){}
public:
	//初始化网络
	virtual bool InitNet()=0;
	//发送数据
	virtual bool SendData(long lSendip, char* buf, int nLen)=0;
	//关闭网络
	virtual void UnInitNet()=0;
protected:
	//接收数据
	virtual bool RecvData()=0;
	InetMediator* m_pMediator;
};

