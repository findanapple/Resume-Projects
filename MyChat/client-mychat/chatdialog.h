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
    //显示窗口信息
    void slot_setInfo(QString name,int id);
    //在控件上显示收到的聊天内容
    void slot_setChatMsg(QString content);
    //处理聊天对象不在线
    void slot_setUserOffline();
private slots:
    void on_pb_send_clicked();
signals:
    void SIG_SendChatMsg(int id,QString content);
private:
    Ui::ChatDialog *ui;
    QString m_name;
    int m_id;

};

#endif // CHATDIALOG_H
