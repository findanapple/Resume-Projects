#ifndef CKERNEL_H
#define CKERNEL_H

#include "INetMediator.h"
#include "PackDef.h"
#include "mychatdialog.h"
#include <QObject>
#include"logindialog.h"
#include<QMap>
#include"useritem.h"
#include"chatdialog.h"
class CKernel;
//函数指针
typedef void (CKernel::*PFUN)(char* buf, int nLen);
class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
signals:
public slots:
    //处理接收到的数据
    void slot_ReadyData(long lSendip, char* buf, int nLen);
    //处理注册请求回复
    void slot_RegisterRs(char* buf, int nLen);
    //处理登录请求回复
    void slot_LoginRs(char* buf, int nLen);
    //处理好友信息请求
    void slot_FriendInfoRq(char*buf,int nLen);
    //处理聊天信息请求
    void slot_ChatRq(char*buf,int nLen);
    //处理聊天信息回复
    void slot_ChatRs(char*buf,int nLen);
    //处理好友添加的请求
    void slot_AddFriendRq(char*buf,int nLen);
    //处理好友添加的回复
    void slot_AddFriendRs(char*buf,int nLen);
    //处理下线请求
    void slot_OfflineRq(char*buf,int nLen);

    //处理登录信息提交
    void slot_LoginCommit(QString tel,QString password);
    //处理聊天内容请求
    void slot_SendChatMsg(int id,QString content);
    //处理注册信息提交
    void slot_RegisterCommit(QString name,QString tel,QString password);
    //处理点击某个用户头像的请求
    void slot_UserItemClicked(int id);
    //界面处理添加好友请求
    void slot_AddFriend();
    //处理登录注册关闭窗口事件
    void slot_DestroyInstance();
    //处理好友列表界面关闭窗口事件
    void slot_Offline();

private:
    MyChatDialog *m_mainWnd;
    logindialog* m_LoginDialog;
    InetMediator*m_tcpclientMediator;
    //协议映射表(函数指针数组)
    PFUN m_netProtocalMap[_DEF_PROTOCAL_COUNT];
    //保存useritem和id的映射关系
    QMap<int,UserItem*>m_mapIdtoUserItem;
    //保存聊天窗口和id的映射关系
    QMap<int,ChatDialog*>m_mapIdtoChatDialog;
    int m_id;
    QString m_name;
private:
    //初始化协议映射表
    void setProtocalMap();
    //qstring转换为->gb2312
    static void utf8toGB2312(char* gbBuf,int len,QString &utf8);
    //gb2312转化为->qstring
    static QString GB2312toutf8(char* gbBuf);
};

#endif // CKERNEL_H
