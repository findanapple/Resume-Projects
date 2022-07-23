#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void setInfo(const QString ip);
    //在控件上显示收到的聊天内容
    void setChatMsg(QString content);

private slots:
    void on_pb_send_clicked();
signals:
    void SIG_SendChatMsg(QString ip,QString content);
private:
    Ui::ChatDialog *ui;
    QString m_ip;

};

#endif // CHATDIALOG_H
