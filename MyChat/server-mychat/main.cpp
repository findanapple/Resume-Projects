#include<iostream>
#include"CKernel.h"
using namespace std;
struct student
{
	string no;
	string name;
	string age;
	string sex;
};
int main()
{
	
	//CMySql sql;
	////连接数据库
	//if (!sql.ConnectMySql("127.0.0.1","root","hd981230","im ")) {
	//	cout << "数据库打开失败" << endl;
	//	system("pause");
	//	return 0;
	//}
	////查询student 表
	//char sqlBuf[1024] = "";
	//list<string>lstRes;
	//sprintf_s(sqlBuf, "select * from student;");
	//if (!sql.SelectMySql(sqlBuf, 4/*列*/, lstRes)) {
	//	cout << "select error:" << sqlBuf << endl;
	//}
	//struct student student = {};
	//while (lstRes.size() > 0) {
	//	student.no = lstRes.front();
	//	lstRes.pop_front();
	//	student.name = lstRes.front();
	//	lstRes.pop_front();
	//	student.age = lstRes.front();
	//	lstRes.pop_front();
	//	student.sex = lstRes.front();
	//	lstRes.pop_front();
	//	cout << "number=" << student.no << "  name=" << student.name
	//		<< " age=" << student.age << " sex=" << student.sex << endl;
	//}
	////更新数据
	//char no[5] = "03";
	//sprintf_s(sqlBuf, "update student set sname ='改名了' where s='%s';",no);
	//if (!sql.UpdateMySql(sqlBuf)) {
	//	cout << "update error:" << sqlBuf << endl;
	//}
	/*InetMediator* m_server = new TcpServerMediator;
	if (!m_server->OpenNet()) {
		cout << "服务端开启失败 " << endl;
	}*/
	/*InetMediator* m_client = new TcpClientMediator;
	if (!m_client->OpenNet()) {
		cout << "客户端开启失败 " << endl;
	}
	char str[100] = "hello world zzzzzz";
	char str1[100] = "aaaahello world zzzzzz";
	m_client->SendData(0, str, strlen(str) + 1);
	m_server->SendData(0, str1, strlen(str1) + 1);*/
	CKernel kernel;
	if (kernel.startServer()) {
		while (1) {
			Sleep(1000);
		}
	}
	else {
		cout << "start server failed..." << endl;
	}
	
	system("pause");
	return 0;
}