#ifndef CKERNEL_H
#define CKERNEL_H
#include "INetMediator.h"
#include "chatdialog.h"
#include "mainwindow.h"
#include <QObject>
#include<map>
using namespace std;
class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
signals:
public slots:
    //处理数据的槽函数
    void slot_readyData(long lSendip, char* buf, int nLen);
    //处理上线请求的函数
    void slot_onlien_rq(long lSendip, char* buf, int nLen);
    //处理上线回复的函数
    void slot_onlien_rs(long lSendip, char* buf, int nLen);
    //处理下线请求的函数
    void slot_offline_rq(long lSendip, char* buf, int nLen);
    //处理聊天请求的函数
    void slot_chat_rq(long lSendip, char* buf, int nLen);
    //处理文件发送的函数
    //处理文件接收的函数

    //处理双击用户所在行的函数
    void slot_userClient(QString ip);
    //处理发送数据
    void slot_SendChatMsg(QString content,QString ip);
    //处理关闭窗口的槽函数
    void slot_close();
private:
    MainWindow* m_mainWnd;
    InetMediator* m_pUdp;
    map<long,ChatDialog*>m_mapIpToChatDialog;//放的ip和聊天窗口
};

#endif // CKERNEL_H
