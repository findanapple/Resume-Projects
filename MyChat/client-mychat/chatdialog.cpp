#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QTime>
ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
//显示窗口信息
void ChatDialog::slot_setInfo(QString name,int id)
{
    qDebug()<<__func__;
    //保存用户信息
    m_name=name;
    m_id=id;
    //标题栏，与【ip】的聊天
    setWindowTitle(QString("正在与[%1]聊天").arg(m_name));
}
//在控件上显示收到的聊天内容
void ChatDialog:: slot_setChatMsg(QString content)
{
    qDebug()<<__func__;
    ui->tb_chat->append(QString("[%1] [%2]").arg(m_name)
                        .arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}
//处理聊天对象不在线
void ChatDialog::slot_setUserOffline()
{
    qDebug()<<__func__;
    //格式：【用户名】 时间  用户不在线
    QString str=QString("[%1] %2 用户不在线").arg(m_name)
            .arg(QTime::currentTime().toString("hh:mm:ss"));
    ui->tb_chat->append(str);
}

void ChatDialog::on_pb_send_clicked()
{
    qDebug()<<__func__;
    //先获取输入的内容
    QString content=ui->te_chat->toPlainText();
    if(content.isEmpty()){
        return;
    }
    //清空te的内容
    ui->te_chat->clear();
    //显示到上面的tb控件
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //发送数据到ckernel
    Q_EMIT SIG_SendChatMsg(m_id,content);

}

