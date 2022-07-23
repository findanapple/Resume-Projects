#include "ckernel.h"
#include "TcpClientMediator.h"
#include<QMessageBox>
#include<QDebug>
#include"PackDef.h"
#include "qregularexpression.h"
#include<QTextCodec>
#include<QInputDialog>

#define NetProtocalMap(a) m_netProtocalMap[a-_DEF_PROTOCAL_BASE-10]

//qstring转换为->gb2312
void CKernel::utf8toGB2312(char* gbBuf,int len,QString &utf8){
    QTextCodec*gb2312=QTextCodec::codecForName("gb2312");
    QByteArray ba= gb2312->fromUnicode(utf8);
    strcpy_s(gbBuf,len,ba.data());
}
//gb2312转化为->qstring
QString CKernel::GB2312toutf8(char* gbBuf){
    QTextCodec*gb2312=QTextCodec::codecForName("gb2312");
    return gb2312->toUnicode(gbBuf);
}
//初始化协议映射表
void CKernel::setProtocalMap(){
    memset(m_netProtocalMap,0,_DEF_PROTOCAL_COUNT);
    NetProtocalMap(_DEF_PACK_REGISTER_RS)       =&CKernel::slot_RegisterRs;
    NetProtocalMap(_DEF_PACK_LOGIN_RS)          =&CKernel::slot_LoginRs;
    NetProtocalMap(_DEF_PACK_FRIEND_RQ)         =&CKernel::slot_FriendInfoRq;
    NetProtocalMap(_DEF_PACK_CHAT_RQ)           =&CKernel::slot_ChatRq;
    NetProtocalMap(_DEF_PACK_CHAT_RS)           =&CKernel::slot_ChatRs;
    NetProtocalMap(_DEF_PACK_ADDFRINEND_RQ)     =&CKernel::slot_AddFriendRq;
    NetProtocalMap(_DEF_PACK_ADDFRINEND_RS)     =&CKernel::slot_AddFriendRs;
    NetProtocalMap(_DEF_PACK_OFFLINE_RQ)     =&CKernel::slot_OfflineRq;
}

CKernel::CKernel(QObject *parent)
    : QObject{parent},m_id(0)
{
    //主窗口new对象，显示
    m_mainWnd=new MyChatDialog;
    //绑定添加好友的信号和槽
    connect(m_mainWnd,SIGNAL(SIG_AddFriend()),
            this,SLOT(slot_AddFriend()));
    //绑定好友列表窗口关闭的信号和槽函数
    connect(m_mainWnd,SIGNAL(SIG_Close()),
            this,SLOT(slot_Offline()));

    //m_mainWnd->showNormal();
    //登录界面new对象，并显示
    m_LoginDialog=new logindialog;
    m_LoginDialog->showNormal();
    //绑定处理登录信息提交的信号和槽
    connect(m_LoginDialog,SIGNAL(SIG_LoginCommit(QString,QString)),
            this,SLOT(slot_LoginCommit(QString, QString)));
    //绑定处理注册信息提交的信号和槽
    connect(m_LoginDialog,SIGNAL(SIG_RegisterCommit(QString,QString,QString)),
            this,SLOT(slot_RegisterCommit(QString, QString,QString)));
    //绑定处理窗口关闭的信号和槽函数
    connect(m_LoginDialog,SIGNAL(SIG_Close()),
            this,SLOT(slot_DestroyInstance()));
    //中介者对象
    m_tcpclientMediator=new TcpClientMediator;
    //处理数据信号——槽绑定
    connect(m_tcpclientMediator,SIGNAL(SIG_ReadyData(long, char*, int)),
            this,SLOT(slot_ReadyData(long, char*, int)));
    //打开网络
    if(!m_tcpclientMediator->OpenNet()){
        QMessageBox::about(m_mainWnd,"提示","打开网络失败");
        exit(0);
    }
    setProtocalMap();
    //测试
//    for(int i=0;i<5;++i){
//        UserItem* item=new UserItem;
//        item->slot_setInfo(i+1,QString("用户i").arg(i),1,i+1);
//        m_mainWnd->slot_addFriend(item);
//    }
//    for(int i=0;i<5;++i){
//        UserItem* item=new UserItem;
//        item->slot_setInfo(i+1,QString("用户i").arg(i),0,i+1);
//        m_mainWnd->slot_addFriend(item);
//    }

}
CKernel::~CKernel(){
    //回收主窗口对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd =NULL;
    }
    //回收中介者对象
    if(m_tcpclientMediator){
        m_tcpclientMediator->CloseNet();
        delete m_tcpclientMediator;
        m_tcpclientMediator =NULL;
    }
}

void CKernel::slot_ReadyData(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.获取协议头
    int nType=*(int*)buf;
    //2.判断协议头是否在映射范围内
    if(nType>=_DEF_PROTOCAL_BASE+10&&
            nType<=_DEF_PROTOCAL_BASE+10+_DEF_PROTOCAL_COUNT){
        //3.计算数组下标，获取对应函数
        PFUN pf=NetProtocalMap(nType);
        //4.如果指针存在，调用对应的处理函数
        if(pf){
            (this->*pf)(buf,nLen);
        }
    }
    //5.回收buf
    delete []buf;
}
//处理注册请求回复
void CKernel::slot_RegisterRs(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_TCP_REGISTER_RS*rs=(STRU_TCP_REGISTER_RS*)buf;
    //2.根据注册结果提示用户
    switch(rs->result)
    {
    case user_is_exist:
        QMessageBox::about(m_LoginDialog,"提示","注册失败，用户已存在");
        break;
    case register_success:
        QMessageBox::about(m_LoginDialog,"提示","注册成功");
        break;
    default:
        break;
    }
}
//处理登录请求回复
void CKernel::slot_LoginRs(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_TCP_LOGIN_RS*rs=(STRU_TCP_LOGIN_RS*)buf;
    m_id=rs->userid;
    //2.根据注册结果提示用户
    switch(rs->result)
    {
    case user_not_exist:
        QMessageBox::about(m_LoginDialog,"提示","登陆失败，用户不存在");
        break;
    case password_error:
        QMessageBox::about(m_LoginDialog,"提示","登陆失败，密码错误");
        break;
    case login_success:

        QMessageBox::about(m_LoginDialog,"提示","登录成功");
        //页面跳转
        m_LoginDialog->hide();
        m_mainWnd->showNormal();
        break;
    default:
        break;
    }
}
//处理好友信息请求
void CKernel::slot_FriendInfoRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_TCP_FRIEND_INFO*info=(STRU_TCP_FRIEND_INFO*)buf;
    //2.进行编码转换
    QString name=GB2312toutf8(info->name);
    QString feeling=GB2312toutf8(info->feeling);

    //3.判断是不是自己
    if(m_id==info->userid){
        //3.1是自己，更新登录用户界面
        m_mainWnd->slot_setInfo(name,feeling,info->icon);
        //3.2保存用户信息
        m_name=name;
        //3.3处理完直接返回
        return;
    }else{
    //4.不是自己，查看当前窗口是否已经有这个好友
        if(m_mapIdtoUserItem.find(info->userid)==m_mapIdtoUserItem.end()){
            //5.如果没有，添加到控件
            //5.1创建一个useritem
            UserItem*item=new UserItem;
            //5.2给控件赋值
            item->slot_setInfo(info->userid,name,info->state,info->icon,feeling);
            //5.3绑定控件点击事件的信号的槽函数
            connect(item,SIGNAL(SIG_UserItemClicked(int)),
                   this,SLOT(slot_UserItemClicked(int)));
            //5.4创建聊天窗口并设置
            ChatDialog* chat=new ChatDialog;
            chat->slot_setInfo(name,info->userid);
            //5.5绑定聊天窗口发送数据的信号和槽函数
            connect(chat,SIGNAL(SIG_SendChatMsg(int, QString)),
                    this,SLOT(slot_SendChatMsg(int, QString)));
            //5.6把聊天窗口放在map上管理
            m_mapIdtoChatDialog[info->userid]=chat;
            //5.7把useritem添加到好友列表
            m_mainWnd->slot_addFriend(item);
            //5.8把useritem放在map中管理
            m_mapIdtoUserItem[info->userid]=item;
        }else{
            //6.如果有，更新控件信息
            UserItem* item=m_mapIdtoUserItem[info->userid];
            //如果之前是下线状态，现在是上线状态，可以弹窗提示用户已上线，或者打印日志
            if(item &&status_offline==item->m_state&&status_online==info->state){
                qDebug()<<QString("用户[%1]已上线").arg(name);
            }
            item->slot_setInfo(info->userid,name,info->state,info->icon,feeling);
        }
    }
}
//处理聊天信息请求
void CKernel::slot_ChatRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_CHAT_RQ*rq=(STRU_TCP_CHAT_RQ*)buf;
    //查看聊天窗口是否存在
    if(m_mapIdtoChatDialog.find(rq->userid)!=m_mapIdtoChatDialog.end()){
        //存在，根据id查找窗口并显示
        ChatDialog* chat=m_mapIdtoChatDialog[rq->userid];
        //设置聊天内容
        chat->slot_setChatMsg(rq->content);
        //chat->showNormal();
    }

}
//处理聊天信息回复
void CKernel::slot_ChatRs(char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_CHAT_RS*rs=(STRU_TCP_CHAT_RS*)buf;
    //查看聊天窗口是否存在
    if(m_mapIdtoChatDialog.count(rs->friendid)>0){
        //存在，根据id查找窗口并显示
        ChatDialog* chat=m_mapIdtoChatDialog[rs->friendid];
        //显示该好友不在线
        chat->slot_setUserOffline();
    }
}
//处理好友添加的请求
void CKernel::slot_AddFriendRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_ADDFRIEND_RQ*rq=(STRU_TCP_ADDFRIEND_RQ*)buf;
    //弹窗提示用户，选择是否同意
    STRU_TCP_ADDFRIEND_RS rs;
    QString str=QString("用户[%1]想添加你为好友，是否同意?").arg(rq->username);
    if(QMessageBox::Yes==QMessageBox::question(m_mainWnd,"添加好友",str)){
        rs.result=add_success;
    }else{
        rs.result=user_refuse;
    }
    //构造请求结果，发给服务端
    rs.friendid=m_id;
    rs.userid=rq->userid;
    strcpy_s(rs.friendname,rq->friendname);
    m_tcpclientMediator->SendData(0,(char*)&rs,sizeof(rs));
}
//处理好友添加的回复
void CKernel::slot_AddFriendRs(char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_ADDFRIEND_RS*rs=(STRU_TCP_ADDFRIEND_RS*)buf;
    //好友昵称转码
    QString name=GB2312toutf8(rs->friendname);
    //根据结果提示不同信息
    switch(rs->result){
    case no_this_user:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友失败，没有用户[%1]").arg(name));
        break;
    case user_refuse:
        QMessageBox::about(m_mainWnd,"提示",QString("用户[%1]拒绝添加好友").arg(name));
        break;
    case user_offline:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友失败，用户[%1]不在线").arg(name));
        break;
    case add_success:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友成功"));
        break;
    default:
        break;
    }
}
//处理下线请求
void CKernel::slot_OfflineRq(char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_OFFLINE_RQ*rq=(STRU_TCP_OFFLINE_RQ*)buf;
    //根据好友id获取useritem，把好友头像置为灰色
    if(m_mapIdtoUserItem.count(rq->userid)>0){
        UserItem* item=m_mapIdtoUserItem[rq->userid];
        item->slot_setOffline();
    }
}
//界面处理添加好友请求
void CKernel::slot_AddFriend()
{
    qDebug()<<__func__;
    //1.弹出窗口，输入好友的昵称
    QString strName=QInputDialog::getText(m_mainWnd,"添加好友","输入好友名字");
    //2.校验用户输入的昵称，昵称不能为空，长度不能超过20
    if(strName.length()<=0||strName.length()>20){
        QMessageBox::about(m_mainWnd,"提示","输入名字不合法");
        return;
    }
    //3.校验昵称是不是自己
    if(strName==m_name){
        QMessageBox::about(m_mainWnd,"提示","不可以添加自己为好友");
        return;
    }
    //4.校验昵称是不是已经是好友，如果已经是好友，提示
    for(auto ite=m_mapIdtoUserItem.begin();ite!=m_mapIdtoUserItem.end();++ite){
        UserItem* item=*ite;
        if(strName==item->m_name){
            QMessageBox::about(m_mainWnd,"提示","已经是好友了");
            return;
        }
    }
    //匹配姓名
    QRegularExpression rg_name("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegularExpressionMatch match_name=rg_name.match(m_name);
    if(!match_name.hasMatch()){
        QMessageBox::about(m_mainWnd,"提示","姓名可以包含中文、英文、数字以及下划线");
        return;
    }
    //5.发送添加好友请求到服务端
    STRU_TCP_ADDFRIEND_RQ rq;
    rq.userid=m_id;
    //好友昵称需要编码，因为需要用其去查数据库
    utf8toGB2312(rq.friendname,sizeof(rq.friendname),strName);
    //用户名称不需要编码，只用于ui显示，不涉及服务器和数据库处理
    strcpy_s(rq.username,m_name.toStdString().c_str());
    m_tcpclientMediator->SendData(0,(char*)&rq,sizeof(rq));
}
//处理登录注册关闭窗口事件
void CKernel::slot_DestroyInstance()
{
    qDebug()<<__func__;
    //资源回收
    //好友列表对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=nullptr;
    }
    //登录注册窗口对象
    if(m_LoginDialog){
        m_LoginDialog->hide();
        delete m_LoginDialog;
        m_LoginDialog=nullptr;
    }
    //聊天窗口对象
    for(auto ite=m_mapIdtoChatDialog.begin();ite!=m_mapIdtoChatDialog.end();){
        ChatDialog*chat=*ite;
        chat->hide();
        delete chat;
        chat=nullptr;
        ite=m_mapIdtoChatDialog.erase(ite);
    }
    //中介者对象
    if(m_tcpclientMediator){
        m_tcpclientMediator->CloseNet();
        delete m_tcpclientMediator;
        m_tcpclientMediator=nullptr;
    }
    exit(0);
}
//处理好友列表界面关闭窗口事件
void CKernel::slot_Offline()
{
    qDebug()<<__func__;
    //通知服务器下线了
    STRU_TCP_OFFLINE_RQ rq;
    rq.userid=m_id;
    m_tcpclientMediator->SendData(0,(char*)&rq,sizeof(rq));
    //回收资源
    slot_DestroyInstance();
}
//处理登录信息提交
void CKernel::slot_LoginCommit(QString tel, QString password)
{
    qDebug()<<__func__;
    //1.QString 转换成 stdstring
    std::string strTel=tel.toStdString();
    std::string strPassword=password.toStdString();
    //2.封包
    STRU_TCP_LOGIN_RQ rq;
    strcpy_s(rq.tel,strTel.c_str());
    strcpy_s(rq.password,strPassword.c_str());
    //3.发送登录请求
    m_tcpclientMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理注册信息提交
void CKernel::slot_RegisterCommit(QString name, QString tel, QString password)
{
    qDebug()<<__func__;
    //1.QString 转换成 stdstring
    std::string strTel=tel.toStdString();
    std::string strPassword=password.toStdString();
    //std::string strName=name.toStdString();
    //2.封包
    STRU_TCP_REGISTER_RQ rq;
    strcpy_s(rq.tel,strTel.c_str());
    strcpy_s(rq.password,strPassword.c_str());
    //strcpy_s(rq.name,strName.c_str());
    utf8toGB2312(rq.name,sizeof(rq.name),name);
    //3.发送登录请求
    m_tcpclientMediator->SendData(0,(char*)&rq,sizeof(rq));
}
//处理点击某个用户头像的请求
void CKernel::slot_UserItemClicked(int id)
{
    //显示聊天窗口
    if(m_mapIdtoChatDialog.find(id)!=m_mapIdtoChatDialog.end()){
        ChatDialog* chat =m_mapIdtoChatDialog[id];
        chat->showNormal();
    }
}
//处理聊天内容请求
void CKernel::slot_SendChatMsg(int id, QString content)
{
    //1.封包
    STRU_TCP_CHAT_RQ rq;
    rq.userid=m_id;
    rq.friendid=id;
    //2.兼容中文
    std::string strContent=content.toStdString();
    strcpy_s(rq.content,strContent.c_str());
    //3.聊天内容发给服务器
    m_tcpclientMediator->SendData(0,(char*)&rq,sizeof(rq));
}
