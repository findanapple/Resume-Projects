#include "useritem.h"
#include "ui_useritem.h"
#include<QBitmap>
UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}

void UserItem::slot_setInfo(int id, QString name, int state, int iconid, QString feeling)
{
    //1.保存用户信息
    m_id=id;
    m_name=name;
    m_state=state;
    m_iconid=iconid;
    m_feeling=feeling;
    //2.设置昵称和签名
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);
    //3.设置头像
    QString iconpath=QString(":/tx/%1.png").arg(iconid);
    //4.判断用户状态
    if(m_state){
        //在线，彩色图标
        ui->pb_iron->setIcon(QIcon(iconpath));
    }else{
        //不在线，灰色图标
        QBitmap bmp;
        bmp.load(iconpath);
        ui->pb_iron->setIcon(bmp);
    }
    //5.重绘
    this->repaint();
}
//设置用户下线
void UserItem::slot_setOffline()
{
    //设置用户状态为下线
    m_state=0;
    //设置用户头像为灰色
    QString iconpath=QString(":/tx/%1.png").arg(m_iconid);
    QBitmap bmp;
    bmp.load(iconpath);
    ui->pb_iron->setIcon(bmp);
    //重绘
    this->repaint();
}

void UserItem::on_pb_iron_clicked()
{
    //发送信号通知kernel类，发送用户id
    Q_EMIT SIG_UserItemClicked(m_id);
}

