#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include<QIcon>
#include<QDebug>
#include<QMenu>
#include<QCursor>
#include<QMessageBox>
MyChatDialog::MyChatDialog(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyChatDialog)
{
    ui->setupUi(this);
    setWindowTitle("通信软件1.0");
    //初始化layout指针
    m_layout=new QVBoxLayout;
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(3);//设置每个空间彼此的间距
    //设置控件的层
    ui->wdg_list->setLayout(m_layout);
    //创建菜单对象，指定父控件，父控件负责子控件的回收
    m_menu=new QMenu(this);
    //添加菜单项(此处可以扩展功能)
    m_menu->addAction("添加好友");
    m_menu->addAction("系统设置");
    //绑定菜单项的点击事件和处理函数
    connect(m_menu,SIGNAL(triggered(QAction*)),
          this,SLOT(slot_dealMenu(QAction*)));
}

MyChatDialog::~MyChatDialog()
{
    delete ui;
}
//添加好友
void MyChatDialog::slot_addFriend(UserItem *userItem)
{
    //往层上添加控件
    m_layout->addWidget(userItem);
}
//设置ui属性
void MyChatDialog::slot_setInfo(QString name, QString feeling, int icon)
{
    qDebug()<<__func__;
    ui->lb_name->setText(name);
    ui->le_feeling->setText(feeling);
    ui->pb_iron->setIcon(QIcon(QString(":/tx/%1.png").arg(icon)));
}
//重写窗口关闭事件
void MyChatDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;
    //忽略关闭事件，在kernel里面控制窗口关闭
    //提示窗口，确认是否关闭窗口
    event->ignore();
    if(QMessageBox::Yes==QMessageBox::question(this,"退出提示","是否确退出？")){
        //通知kernel类关闭窗口，回收资源
        Q_EMIT SIG_Close();
    }
}

//工具1
void MyChatDialog::on_pb_tool1_clicked()
{
    //按鼠标点击的位置获取菜单
    QPoint p=QCursor::pos();
    //获取菜单的绝对高度
    QSize size=m_menu->sizeHint();
    m_menu->exec(QPoint(p.x(),p.y()-size.height()));
}
//点击菜单项的处理函数
void MyChatDialog::slot_dealMenu(QAction *action)
{
    if(action->text()=="添加好友"){
        //发送添加好友的信号给kernel
        Q_EMIT SIG_AddFriend();
    }else if(action->text()=="系统设置"){
        //处理系统设置
    }
}

