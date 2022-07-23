#include "myINet.h"//
#include "ckernel.h"
#include "INetMediator_UDP.h"
#include "ipConfig.h"
#include <QMessageBox>
#include <QDebug>
#include <QHostAddress>

CKernel::CKernel(QObject *parent)
    : QObject{parent}
{
    //创建主窗口对象并显示
    m_mainWnd=new MainWindow;
    m_mainWnd->show();
    //绑定双击
    QObject::connect(m_mainWnd,SIGNAL(SIG_userClient(QString)),
                     this,SLOT(slot_userClient(QString)));
    //绑定关闭窗口
    QObject::connect(m_mainWnd,SIGNAL(SIG_close()),
                     this,SLOT(slot_close()));
    //创建中介者的对象
    m_pUdp=new INetMediator_UDP;
    //绑定接受数据的信号和槽
    QObject::connect(m_pUdp,SIGNAL(SIG_ReadyData(long , char* , int )),
                     this,SLOT(slot_readyData(long , char* , int )));

    //打开网络
    if(!m_pUdp->OpenNet()){
        QMessageBox::warning(m_mainWnd,"提示","网络连接失败");
        exit(0);
    }
    //上线广播
    STRU_ONLINE rq;
    gethostname(rq.szName,_DEF_NAME_SIZE);
    m_pUdp->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));
}
CKernel::~CKernel()
{
    //回收聊天窗口
    for(auto ite=m_mapIpToChatDialog.begin();ite!=m_mapIpToChatDialog.end();){
        ChatDialog* chat =ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChatDialog.erase(ite);
    }
    //回收主窗口对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }
    //回收中介者对象
    if(m_pUdp){
        m_pUdp->CloseNet();
        delete m_pUdp;
        m_pUdp=NULL;
    }


}
//处理接受数据的函数
void CKernel::slot_readyData(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //去除协议头
    int nType=*(int*)buf;
    //根据协议头判断是什么请求
    switch(nType){
    case _DEF_PROTOCAL_ONLINE_RQ:
        slot_onlien_rq(lSendip,buf, nLen);
        break;
    case _DEF_PROTOCAL_ONLINE_RS:
        slot_onlien_rs(lSendip,buf, nLen);
        break;
    case _DEF_PROTOCAL_OFFLINE_RQ:
        slot_offline_rq(lSendip,buf, nLen);
        break;
    case _DEF_PROTOCAL_CHAT_RQ:
        slot_chat_rq(lSendip,buf, nLen);
        break;
//    case _DEF_PROTOCAL_FILEINFO_RQ:
//        break;
    default:
        break;
    }
    delete[] buf;
    //QMessageBox::about(m_mainWnd,QString("来自[%1]的信息").arg(QHostAddress(htonl(lSendip)).toString()),buf);
}
//处理上线请求的函数
void CKernel::slot_onlien_rq(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_ONLINE*rq=(STRU_ONLINE*)buf;
    //2.添加该用户到自己的好友列表
    m_mainWnd->slot_addFriend(INet::GetIpString(lSendip).c_str(),
                              QString::fromStdString(rq->szName));
    if(m_mapIpToChatDialog.count(lSendip)==0){
    //3.创建与该用户聊天的窗口，但是先不显示
        ChatDialog* chat=new ChatDialog;
        chat->setInfo(INet::GetIpString(lSendip).c_str());
        connect(chat,SIGNAL(SIG_SendChatMsg(QString,QString)),
                         this,SLOT(slot_SendChatMsg(QString,QString)));

        //4.在map中保存聊天窗口
        m_mapIpToChatDialog[lSendip]=chat;
    }
    //5.判断是不是自己，如果是自己就不用回复
    set<long>setIp=INet::GetValidIpList();
    if(setIp.count(lSendip)>0){
        return;
    }
    //6.给对方发上线回复
    STRU_ONLINE rs;
    rs.nType=_DEF_PROTOCAL_ONLINE_RS;
    gethostname(rs.szName,sizeof(rs.szName));
    m_pUdp->SendData(lSendip,(char*)&rs,sizeof(rs));
}
//处理上线回复的函数
void CKernel::slot_onlien_rs(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_ONLINE*rs=(STRU_ONLINE*)buf;
    //2.添加该用户到自己的好友列表
    m_mainWnd->slot_addFriend(INet::GetIpString(lSendip).c_str(),
                              QString::fromStdString(rs->szName));
    if(m_mapIpToChatDialog.count(lSendip)==0){
    //3.创建与该用户聊天的窗口，但是先不显示
        ChatDialog* chat=new ChatDialog;
        chat->setInfo(INet::GetIpString(lSendip).c_str());
        connect(chat,SIGNAL(SIG_SendChatMsg(QString,QString)),
                         this,SLOT(slot_SendChatMsg(QString,QString)));
        //4.在map中保存聊天窗口
        m_mapIpToChatDialog[lSendip]=chat;
    }
}
//处理下线请求的函数
void CKernel::slot_offline_rq(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1.从好友列表删除
    m_mainWnd->slot_delFriend(INet::GetIpString(lSendip).c_str());
    //2.回收聊天窗口，并从map中删除
    auto ite=m_mapIpToChatDialog.find(lSendip);
    if(ite!=m_mapIpToChatDialog.end()){
        ChatDialog* chat=ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChatDialog.erase(ite);
    }
}
//处理聊天请求的函数
void CKernel::slot_chat_rq(long lSendip, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_CHAT_RQ*rq=(STRU_CHAT_RQ*)buf;
    //打开聊天窗口
    if(m_mapIpToChatDialog.count(lSendip)>0){
        //取出窗口并显示
        ChatDialog* chat=m_mapIpToChatDialog[lSendip];
        //设置聊天内容
        chat->setChatMsg(rq->szContent);
        chat->showNormal();
    }
}
//处理双击用户所在行的函数
void CKernel::slot_userClient(QString ip)
{
    qDebug()<<__func__;
    //1.转换ip
    long lip=inet_addr(ip.toStdString().c_str());
    //判断窗口是否存在
    if(m_mapIpToChatDialog.count(lip)>0){
        //取出窗口并显示
        ChatDialog* chat=m_mapIpToChatDialog[lip];
        chat->showNormal();
    }
}
//处理发送数据的函数
void CKernel::slot_SendChatMsg(QString ip, QString content)
{
    qDebug()<<__func__;
    //1.打包
    STRU_CHAT_RQ rq;
    rq.nType=_DEF_PROTOCAL_CHAT_RQ;
    strcpy(rq.szContent,content.toStdString().c_str());
    //2.发数据
    m_pUdp->SendData(inet_addr(ip.toStdString().c_str()),(char*)&rq,sizeof(rq));
}
//关闭窗口
void CKernel::slot_close()
{
    //1.发送下线请求
    STRU_OFFLINE_RQ rq;
    m_pUdp->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));
    //2.回收聊天窗口
    for(auto ite=m_mapIpToChatDialog.begin();ite!=m_mapIpToChatDialog.end();){
        ChatDialog* chat =ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChatDialog.erase(ite);
    }
    //3.回收主窗口
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }
    //4.关闭网络
    if(m_pUdp){
        m_pUdp->CloseNet();
        delete m_pUdp;
        m_pUdp=NULL;
    }
    exit(0);
}
