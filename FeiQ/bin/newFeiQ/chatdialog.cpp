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

void ChatDialog::setInfo(QString ip)
{
    qDebug()<<__func__;
    m_ip=ip;
    //标题栏，与【ip】的聊天
    setWindowTitle(QString("正在与[%1]聊天").arg(ip));
}

void ChatDialog:: setChatMsg(QString content)
{
    qDebug()<<__func__;
    ui->tb_chat->append(QString("[%1] [%2]").arg(m_ip)
                        .arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
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
    Q_EMIT SIG_SendChatMsg(m_ip,content);

}

