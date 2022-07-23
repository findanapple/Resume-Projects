#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT
signals:
    void SIG_UserItemClicked(int id);
public:
    explicit UserItem(QWidget *parent = nullptr);
    ~UserItem();
    //设置用户信息
    void slot_setInfo(int id,QString name,int state,
                      int iconid=1,QString feeling="比较懒，什么也没写");
    //设置用户下线
    void slot_setOffline();
private slots:
    void on_pb_iron_clicked();

public:
    Ui::UserItem *ui;
    int m_id;           //用户id
    QString m_name;     //用户昵称
    int m_state;        //用户状态
    int m_iconid;       //用户头像
    QString m_feeling;  //用户签名
};

#endif // USERITEM_H
