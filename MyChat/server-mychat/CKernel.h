#pragma once
#include"MySQL/CMySql.h"
#include"./mediator/INetMediator.h"
#include<iostream>
#include"net/PackDef.h"
#include<map>
using namespace std;
class CKernel;
//函数指针
typedef void (CKernel::* PFUN)(long lSendip,char* buf, int nLen);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//开启服务
	bool startServer();
	//关闭服务
	bool closeServer();
	//处理数据
	void dealData(long lSendip, char* buf, int nLen);
	
private:
	CMySql m_sql;
	InetMediator* m_pServer;
	//协议映射表(函数指针数组)
	PFUN m_netProtocalMap[_DEF_PROTOCAL_COUNT];
	//保存id和socket的映射关系
	map<int, int>m_mapIdtoSocket;
private:
	//初始化协议映射表
	void setProtocalMap();
	//获取好友列表，包括自己
	void getUserList(int id);
	//根据id 查用户信息
	void getUserInfobyId(int id, STRU_TCP_FRIEND_INFO* info);
	//处理注册请求
	void dealRegisterRQ(long lSendip,char* buf, int nLen);
	//处理登录请求
	void dealLoginRQ(long lSendip,char* buf, int nLen);
	//处理聊天请求
	void dealChatRQ(long lSendip, char* buf, int nLen);
	//处理添加好友请求
	void dealAddFriendRQ(long lSendip, char* buf, int nLen);
	//处理添加好友回复
	void dealAddFriendRS(long lSendip, char* buf, int nLen);
	//处理下线请求
	void dealOfflineRQ(long lSendip, char* buf, int nLen);
public:
	static CKernel* pKernel;
};

