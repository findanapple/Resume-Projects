#pragma once
#include"MySQL/CMySql.h"
#include"./mediator/INetMediator.h"
#include<iostream>
#include"net/PackDef.h"
#include<map>
using namespace std;
class CKernel;
//����ָ��
typedef void (CKernel::* PFUN)(long lSendip,char* buf, int nLen);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//��������
	bool startServer();
	//�رշ���
	bool closeServer();
	//��������
	void dealData(long lSendip, char* buf, int nLen);
	
private:
	CMySql m_sql;
	InetMediator* m_pServer;
	//Э��ӳ���(����ָ������)
	PFUN m_netProtocalMap[_DEF_PROTOCAL_COUNT];
	//����id��socket��ӳ���ϵ
	map<int, int>m_mapIdtoSocket;
private:
	//��ʼ��Э��ӳ���
	void setProtocalMap();
	//��ȡ�����б������Լ�
	void getUserList(int id);
	//����id ���û���Ϣ
	void getUserInfobyId(int id, STRU_TCP_FRIEND_INFO* info);
	//����ע������
	void dealRegisterRQ(long lSendip,char* buf, int nLen);
	//�����¼����
	void dealLoginRQ(long lSendip,char* buf, int nLen);
	//������������
	void dealChatRQ(long lSendip, char* buf, int nLen);
	//������Ӻ�������
	void dealAddFriendRQ(long lSendip, char* buf, int nLen);
	//������Ӻ��ѻظ�
	void dealAddFriendRS(long lSendip, char* buf, int nLen);
	//������������
	void dealOfflineRQ(long lSendip, char* buf, int nLen);
public:
	static CKernel* pKernel;
};

