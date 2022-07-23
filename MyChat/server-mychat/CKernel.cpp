#include "CKernel.h"
#include"./mediator/TcpServerMediator.h"
#include <time.h>
#define NetProtocalMap(a) m_netProtocalMap[a-_DEF_PROTOCAL_BASE-10]
//��̬��Ա����Ҫ�������ʼ��
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
//��������
bool CKernel::startServer()
{
	//1.�������ݿ�
	if (!m_sql.ConnectMySql("127.0.0.1", "root", "hd981230", "im")) {
			cout << "���ݿ��ʧ��" << endl;
			return false;
	}
	//2.��������
	m_pServer = new TcpServerMediator;
	if (!m_pServer->OpenNet()) {
		cout << "������ʧ��" << endl;
		return false;
	}
	return true;
}
//�رշ���
bool CKernel::closeServer()
{
	//��Դ����
	//1.�Ͽ������ݿ�����
	m_sql.DisConnect();
	//2.�ر�����
	if (m_pServer) {
		m_pServer->CloseNet();
		delete m_pServer;
		m_pServer = NULL;
	}
	return true;
}
//��������
void CKernel::dealData(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	//1.��ȡЭ��ͷ
	int nType = *(int*)buf;
	//2.�ж�Э��ͷ�Ƿ���ӳ�䷶Χ��
	if (nType >= _DEF_PROTOCAL_BASE + 10 &&
		nType <= _DEF_PROTOCAL_BASE + 10 + _DEF_PROTOCAL_COUNT) {
		//3.���������±꣬��ȡ��Ӧ����
		PFUN pf = NetProtocalMap(nType);
		//4.���ָ����ڣ����ö�Ӧ�Ĵ�����
		if (pf) {
			(this->*pf)(lSendip,buf, nLen);
		}
	}
	//5.����buf
	delete[]buf;
}
//����ע������
void CKernel::dealRegisterRQ(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	srand(time(NULL));
	//1.���
	STRU_TCP_REGISTER_RQ* rq=(STRU_TCP_REGISTER_RQ*)buf;
	//2.ȡ��tel��password��name���жϺϷ��ԣ�����tel���
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select tel from t_user where tel=%s;", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	
	//3.�ж�tel�Ƿ��Ѿ���ע��
	STRU_TCP_REGISTER_RS rs;
	if (resultList.size() > 0) {
		//��ע��
		rs.result = user_is_exist;
	}
	else {
		///4.���û��ע�ᣬ�������ݿ�д��ע������
		rs.result = register_success;
		
		sprintf(sqlBuf, "insert into t_user (tel,password,name,icon,feeling) values ('%s','%s','%s','%d','�Ƚ�����ʲôҲûд');",
			rq->tel,rq->password,rq->name,rand()%30);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "UpdateMySql error:" << sqlBuf << endl;
			return;
		}

	}
	//5.���ͻ��˷���ע����
	m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
}
//�����¼����
void CKernel::dealLoginRQ(long lSendip, char* buf, int nLen)
{
	cout << __func__ << endl;
	//1.���ȡ��password��tel
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	//2.����tel�鿴�Ƿ�������û�
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel=%s;", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 2, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//3.û������û���ֱ�ӷ���ʧ��
	STRU_TCP_LOGIN_RS rs;
	int nid = 0;
	string strpass;
	if (0==resultList.size()) {
		rs.result = user_not_exist;
		//8.���ͻ��˷��ص�¼���
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	else {
		//4.�У���ȡid������
		nid = atoi(resultList.front().c_str());
		resultList.pop_front();
		strpass = resultList.front();
		resultList.pop_front();
		//5.�ж������Ƿ�һ��
		if (0!=strcmp(strpass.c_str(),rq->password)) {
			//6.��һ�·��ص�¼ʧ��
			rs.result = password_error;
			//8.���ͻ��˷��ص�¼���
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
		}
		else {
			//7. ����һ�·��ص�¼�ɹ����û�id
			rs.result = login_success;
			rs.userid = nid;
			//8.���ͻ��˷��ص�¼���
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
			//9.����id��socket��ӳ���ϵ
			m_mapIdtoSocket[nid] = lSendip;
			//10.��ȡ�����б�																										
			getUserList(nid);
		}
	}
}
//������������
void CKernel::dealChatRQ(long lSendip, char* buf, int nLen)
{
	//1.���
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;
	//2.�����շ��Ƿ�����
	if (m_mapIdtoSocket.find(rq->userid) != m_mapIdtoSocket.end()) {
		//2.1����Է����ߣ���ȡ�Է��׽��֣�ת������
		SOCKET sock = m_mapIdtoSocket[rq->friendid];
		m_pServer->SendData(sock, buf, nLen);
	}
	else {
		//2.2����Է������ߣ��ظ��ͻ��˺��Ѳ�����
		STRU_TCP_CHAT_RS rs;
		rs.result = obj_offline;
		rs.userid = rq->userid;
		rs.friendid = rq->friendid;
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	
}
//������Ӻ�������
void CKernel::dealAddFriendRQ(long lSendip, char* buf, int nLen)
{
	//1.���
	STRU_TCP_ADDFRIEND_RQ* rq = (STRU_TCP_ADDFRIEND_RQ*)buf;
	//2.�鿴����Ӻ����Ƿ����
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id from t_user where name= '%s';", rq->friendname);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//3.���Ѵ��ڣ�ȡ��id
	if (resultList.size() > 0) {
		int friendid = atoi(resultList.front().c_str());
		resultList.pop_front();
		//4.���Ƿ�����
		if (m_mapIdtoSocket.count(friendid) > 0) {
			//4.1������ߣ�ȡ��sock��������Ӻ�������
			SOCKET sock = m_mapIdtoSocket[friendid];
			m_pServer->SendData(sock, buf, nLen);
		}
		else {
			//4.2��������ߣ��ظ��ͻ��˲�����
			STRU_TCP_ADDFRIEND_RS rs;
			rs.result = obj_offline;
			rs.friendid = friendid;
			rs.userid = rq->userid;
			strcpy_s(rs.friendname, rq->friendname);
			m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
		}	
	}
	else {
		//5.���Ѳ����ڣ��ظ��ͻ��˺��Ѳ�����
		STRU_TCP_ADDFRIEND_RS rs;
		rs.result = no_this_user;
		rs.friendid = 0;
		rs.userid = rq->userid;
		strcpy_s(rs.friendname, rq->friendname);
		m_pServer->SendData(lSendip, (char*)&rs, sizeof(rs));
	}
	
}
//������Ӻ��ѻظ�
void CKernel::dealAddFriendRS(long lSendip, char* buf, int nLen)
{
	//���
	STRU_TCP_ADDFRIEND_RS* rs = (STRU_TCP_ADDFRIEND_RS*)buf;
	//�ж϶Է��Ƿ�ͬ��
	if (add_success==rs->result) {
		//���ͬ�⣬�����ݿ�����Ӻ��ѹ�ϵ
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
		//���º����б�
		getUserList(rs->friendid);
	}
	//�����Ƿ�ͬ�⣬��Ҫת���ظ�
	if (m_mapIdtoSocket.count(rs->userid) > 0) {
		SOCKET sock = m_mapIdtoSocket[rs->userid];
		m_pServer->SendData(sock, buf, nLen);
	}
}
//������������
void CKernel::dealOfflineRQ(long lSendip, char* buf, int nLen)
{
	//���
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;
	//��ȡ�����û�id
	int userid = rq->userid;
	//���������û�id��ѯ���û����ѵ�id
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA= %d;", userid);
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//������ѯ���
	int friendid = 0;
	while (resultList.size() > 0) {
		//�����ߺ��ѷ�������֪ͨ
		//ȡ������id
		friendid = atoi(resultList.front().c_str());
		resultList.pop_front(); 
		//�жϺ����Ƿ�����
		if (m_mapIdtoSocket.count(friendid)>0) {
			//��������֪ͨ
			SOCKET sock = m_mapIdtoSocket[friendid];
			m_pServer->SendData(sock,buf,nLen);
		}
	}
	//�������û���socket��map���Ƴ�
	if (m_mapIdtoSocket.count(userid) > 0) {
		m_mapIdtoSocket.erase(userid);
	}
}
//��ʼ��Э��ӳ���
void CKernel::setProtocalMap() {
	memset(m_netProtocalMap, 0, _DEF_PROTOCAL_COUNT);
	NetProtocalMap(_DEF_PACK_REGISTER_RQ) = &CKernel::dealRegisterRQ;
	NetProtocalMap(_DEF_PACK_LOGIN_RQ) = &CKernel::dealLoginRQ;
	NetProtocalMap(_DEF_PACK_CHAT_RQ) = &CKernel::dealChatRQ;
	NetProtocalMap(_DEF_PACK_ADDFRINEND_RQ) = &CKernel::dealAddFriendRQ;
	NetProtocalMap(_DEF_PACK_ADDFRINEND_RS) = &CKernel::dealAddFriendRS;
	NetProtocalMap(_DEF_PACK_OFFLINE_RQ) = &CKernel::dealOfflineRQ;
}
//��ȡ�����б������Լ�
void CKernel::getUserList(int id)
{
	//1.��ѯ�Լ�����Ϣ
	STRU_TCP_FRIEND_INFO logininfo;
	getUserInfobyId(id, &logininfo);
	//2.��ȡ�û����׽��֣�����id��map��ȡ��
	if (m_mapIdtoSocket.find(id) == m_mapIdtoSocket.end()) {
		cout << "user:" << id << "is not login" << endl;																										
		return;
	}
	SOCKET loginsock = m_mapIdtoSocket[id];
	//3.���Լ�����Ϣ���͸��ͻ���
	m_pServer->SendData(loginsock, (char*)&logininfo, sizeof(logininfo));
	//4.�����û�id�������к���id
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select idB from t_friend where idA=%d;", id);  
	if (!m_sql.SelectMySql(sqlBuf, 1, resultList)) {
		cout << "SelectMySql error:" << sqlBuf << endl;
		return;
	}
	//5.�������ҽ��
	int friendid = 0;
	STRU_TCP_FRIEND_INFO friendinfo;
	while (resultList.size() > 0) {
		//6.ȡ������id�����ݺ���id���Һ�����Ϣ
		friendid = atoi(resultList.front().c_str());
		resultList.pop_front();
		getUserInfobyId(friendid, &friendinfo);
		//7.�Ѻ�����Ϣ�����ͻ���
		m_pServer->SendData(loginsock, (char*)&friendinfo, sizeof(friendinfo));
		//8.����������ߣ�֪ͨ���ѵ�¼��������
		if (m_mapIdtoSocket.find(friendid) == m_mapIdtoSocket.end()) {
			continue;//������Ѳ����ߣ�������һ��ѭ��
		}
		//9.����������ߣ�ȡ�����ѵ��׽��֣������ѷ��͵�¼�˵�����֪ͨ
		SOCKET friendsock = m_mapIdtoSocket[friendid];
		m_pServer->SendData(friendsock, (char*)&logininfo, sizeof(logininfo));
	}	
}
//����id ���û���Ϣ
void CKernel::getUserInfobyId(int id, STRU_TCP_FRIEND_INFO* info)
{
	//����id
	info->userid = id;
	//����id ���û���Ϣ
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
	//3.�ж��û��ڲ�����
	if (m_mapIdtoSocket.find(id) != m_mapIdtoSocket.end()) {
		info->state = status_online;
	}
	else {
		info->state = status_offline;
	}
}
