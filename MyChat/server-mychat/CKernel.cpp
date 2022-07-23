#include "CKernel.h"
#include"./mediator/TcpServerMediator.h"
#include <time.h>
#define NetProtocalMap(a) m_netProtocalMap[a-_DEF_PROTOCAL_BASE-10]
//静态成员变量要在类外初始化
CKernel* CKernel::pKernel = NULL;

CKernel::CKernel() :m_pServer(NULL)
{
	pKernel = this;
	setProtocalMap();
}

CKernel::~CKernel()
{
	closeServer();
}
//开启服务
bool CKernel::startServer()
{
	//1.连接数据库
	if (!m_sql.ConnectMySql("127.0.0.1", "root", "hd981230", "im")) {
			cout << "数据库打开失败" << endl;
			return false;
	}
	//2.开启网络
	m_pServer = new TcpServerMediator;
	if (!m_pServer->OpenNet()) {
		cout << "打开网络失败" << endl;
		return false;
	}
	return true;
}
//关闭服务
bool CKernel::closeServer()
{
	//资源回收
	//1.断开与数据库连接
	m_sql.DisConnect();
	//2.关闭网络
	if (m_pServer) {
		m_pServer->CloseNet();
		delete m_pServer;
		m_pServer = NULL;
	}
	return true;
}
//处理数据
void CKernel::dealData(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	//1.获取协议头
	int nType = *(int*)buf;
	//2.判断协议头是否在映射范围内
	if (nType >= _DEF_PROTOCAL_BASE + 10 &&
		nType <= _DEF_PROTOCAL_BASE + 10 + _DEF_PROTOCAL_COUNT) {
		//3.计算数组下标，获取对应函数
		PFUN pf = NetProtocalMap(nType);
		//4.如果指针存在，调用对应的处理函数
		if (pf) {
			(this->*pf)(lSendip,buf, nLen);
		}
	}
	//5.回收buf
	delete[]buf;
}
//处理注册请求
void CKernel::dealRegisterRQ(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	srand(time(NULL));
	//1.拆包
	STRU_TCP_REGISTER_RQ* rq=(STRU_TCP_REGISTER_RQ*)buf;
	//2.取出tel、password、name，判断合法性，按照tel查表
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select tel from t_user where tel=%s;", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	
	//3.判断tel是否已经被注册
	STRU_TCP_REGISTER_RS rs;
	if (resultList.size() > 0) {
		//被注册
		rs.result = user_is_exist;
	}
	else {
		///4.如果没有注册，则向数据库写入注册内容
		rs.result = register_success;
		
		sprintf(sqlBuf, "insert into t_user (tel,password,name,icon,feeling) values ('%s','%s','%s','%d','比较懒，什么也没写');",
			rq->tel,rq->password,rq->name,rand()%30);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "UpdateMySql error:" << sqlBuf << endl;
			return;
		}

	}
	//5.给客户端返回注册结果
	m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
}
//处理登录请求
void CKernel::dealLoginRQ(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	//1.拆包取出password，tel
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	//2.根据tel查看是否有这个用户
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel=%s;", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 2, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//3.没有这个用户，直接返回失败
	STRU_TCP_LOGIN_RS rs;
	int nid = 0;
	string strpass;
	if (0==resultList.size()) {
		rs.result = user_not_exist;
		//8.给客户端返回登录结果
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	else {
		//4.有，获取id，密码
		nid = atoi(resultList.front().c_str());
		resultList.pop_front();
		strpass = resultList.front();
		resultList.pop_front();
		//5.判断密码是否一致
		if (0!=strcmp(strpass.c_str(),rq->password)) {
			//6.不一致返回登录失败
			rs.result = password_error;
			//8.给客户端返回登录结果
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
		}
		else {
			//7. 密码一致返回登录成功和用户id
			rs.result = login_success;
			rs.userid = nid;
			//8.给客户端返回登录结果
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
			//9.保存id和socket的映射关系
			m_mapIdtoSocket[nid] = lSendip;
			//10.获取好友列表																										
			getUserList(nid);
		}
	}
}
//处理聊天请求
void CKernel::dealChatRQ(long lSendip, char* buf, int nLen)
{
	//1.拆包
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;
	//2.看接收方是否在线
	if (m_mapIdtoSocket.find(rq->userid) != m_mapIdtoSocket.end()) {
		//2.1如果对方在线，获取对方套接字，转发数据
		SOCKET sock = m_mapIdtoSocket[rq->friendid];
		m_pServer->SendData(sock, buf, nLen);
	}
	else {
		//2.2如果对方不在线，回复客户端好友不在线
		STRU_TCP_CHAT_RS rs;
		rs.result = obj_offline;
		rs.userid = rq->userid;
		rs.friendid = rq->friendid;
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	
}
//处理添加好友请求
void CKernel::dealAddFriendRQ(long lSendip, char* buf, int nLen)
{
	//1.拆包
	STRU_TCP_ADDFRIEND_RQ* rq = (STRU_TCP_ADDFRIEND_RQ*)buf;
	//2.查看待添加好友是否存在
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id from t_user where name= '%s';", rq->friendname);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//3.好友存在，取出id
	if (resultList.size() > 0) {
		int friendid = atoi(resultList.front().c_str());
		resultList.pop_front();
		//4.看是否在线
		if (m_mapIdtoSocket.count(friendid) > 0) {
			//4.1如果在线，取出sock，发送添加好友请求
			SOCKET sock = m_mapIdtoSocket[friendid];
			m_pServer->SendData(sock, buf, nLen);
		}
		else {
			//4.2如果不在线，回复客户端不在线
			STRU_TCP_ADDFRIEND_RS rs;
			rs.result = obj_offline;
			rs.friendid = friendid;
			rs.userid = rq->userid;
			strcpy_s(rs.friendname, rq->friendname);
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
		}	
	}
	else {
		//5.好友不存在，回复客户端好友不存在
		STRU_TCP_ADDFRIEND_RS rs;
		rs.result = no_this_user;
		rs.friendid = 0;
		rs.userid = rq->userid;
		strcpy_s(rs.friendname, rq->friendname);
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	
}
//处理添加好友回复
void CKernel::dealAddFriendRS(long lSendip, char* buf, int nLen)
{
	//拆包
	STRU_TCP_ADDFRIEND_RS* rs = (STRU_TCP_ADDFRIEND_RS*)buf;
	//判断对方是否同意
	if (add_success==rs->result) {
		//如果同意，向数据库中添加好友关系
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "insert into t_friend values(%d,%d);", rs->friendid, rs->userid);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "UpdateMySql errpr:" << sqlBuf << endl;
			return;
		}
		sprintf(sqlBuf, "insert into t_friend values(%d,%d);", rs->userid, rs->friendid);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "UpdateMySql errpr:" << sqlBuf << endl;
			return;
		}
		/*char sqlBuf[1024] = "";
		sprintf(sqlBuf, "begin;insert into t_friend values(%d,%d);insert into t_friend values(%d,%d);commit;close;",
			rs->friendid, rs->userid, rs->userid, rs->friendid);*/
		//更新好友列表
		getUserList(rs->friendid);
	}
	//无论是否同意，都要转发回复
	if (m_mapIdtoSocket.count(rs->userid) > 0) {
		SOCKET sock = m_mapIdtoSocket[rs->userid];
		m_pServer->SendData(sock, buf, nLen);
	}
}
//处理下线请求
void CKernel::dealOfflineRQ(long lSendip, char* buf, int nLen)
{
	//拆包
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;
	//获取下线用户id
	int userid = rq->userid;
	//根据下线用户id查询该用户好友的id
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA= %d;", userid);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//遍历查询结果
	int friendid = 0;
	while (resultList.size() > 0) {
		//给在线好友发送下线通知
		//取出好友id
		friendid = atoi(resultList.front().c_str());
		resultList.pop_front(); 
		//判断好友是否在线
		if (m_mapIdtoSocket.count(friendid)>0) {
			//发送下线通知
			SOCKET sock = m_mapIdtoSocket[friendid];
			m_pServer->SendData(sock,buf,nLen);
		}
	}
	//把下线用户的socket从map中移除
	if (m_mapIdtoSocket.count(userid) > 0) {
		m_mapIdtoSocket.erase(userid);
	}
}
//初始化协议映射表
void CKernel::setProtocalMap() {
	memset(m_netProtocalMap, 0, _DEF_PROTOCAL_COUNT);
	NetProtocalMap(_DEF_PACK_REGISTER_RQ) = &CKernel::dealRegisterRQ;
	NetProtocalMap(_DEF_PACK_LOGIN_RQ) = &CKernel::dealLoginRQ;
	NetProtocalMap(_DEF_PACK_CHAT_RQ) = &CKernel::dealChatRQ;
	NetProtocalMap(_DEF_PACK_ADDFRINEND_RQ) = &CKernel::dealAddFriendRQ;
	NetProtocalMap(_DEF_PACK_ADDFRINEND_RS) = &CKernel::dealAddFriendRS;
	NetProtocalMap(_DEF_PACK_OFFLINE_RQ) = &CKernel::dealOfflineRQ;
}
//获取好友列表，包括自己
void CKernel::getUserList(int id)
{
	//1.查询自己的信息
	STRU_TCP_FRIEND_INFO logininfo;
	getUserInfobyId(id, &logininfo);
	//2.获取用户的套接字，根据id从map中取出
	if (m_mapIdtoSocket.find(id) == m_mapIdtoSocket.end()) {
		cout << "user:" << id << "is not login" << endl;																										
		return;
	}
	SOCKET loginsock = m_mapIdtoSocket[id];
	//3.把自己的信息发送给客户端
	m_pServer->SendData(loginsock, (char*)&logininfo, sizeof(logininfo));
	//4.根据用户id查找所有好友id
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", id);  
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//5.遍历查找结果
	int friendid = 0;
	STRU_TCP_FRIEND_INFO friendinfo;
	while (resultList.size() > 0) {
		//6.取出好友id，根据好友id查找好友信息
		friendid = atoi(resultList.front().c_str());
		resultList.pop_front();
		getUserInfobyId(friendid, &friendinfo);
		//7.把好友信息发给客户端
		m_pServer->SendData(loginsock, (char*)&friendinfo, sizeof(friendinfo));
		//8.如果好友在线，通知好友登录人已上线
		if (m_mapIdtoSocket.find(friendid) == m_mapIdtoSocket.end()) {
			continue;//如果好友不在线，进入下一次循环
		}
		//9.如果好友在线，取出好友的套接字，给好友发送登录人的上线通知
		SOCKET friendsock = m_mapIdtoSocket[friendid];
		m_pServer->SendData(friendsock, (char*)&logininfo, sizeof(logininfo));
	}	
}
//根据id 查用户信息
void CKernel::getUserInfobyId(int id, STRU_TCP_FRIEND_INFO* info)
{
	//保存id
	info->userid = id;
	//根据id 查用户信息
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select name,icon,feeling from t_user where id=%d;", id);
	if (!m_sql.SelectMySql(sqlBuf, 3, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	if (3 == resultList.size()) {
		strcpy_s(info->name, resultList.front().c_str());
		resultList.pop_front();
		info->icon = atoi(resultList.front().c_str());
		resultList.pop_front();
		strcpy_s(info->feeling, resultList.front().c_str());
		resultList.pop_front();
	}
	//3.判断用户在不在线
	if (m_mapIdtoSocket.find(id) != m_mapIdtoSocket.end()) {
		info->state = status_online;
	}
	else {
		info->state = status_offline;
	}
}
