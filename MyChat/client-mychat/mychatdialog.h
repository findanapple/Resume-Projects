#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H

#include <QMainWindow>
#include<QVBoxLayout>
#include<useritem.h>
#include<QCloseEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class MyChatDialog; }
QT_END_NAMESPACE

class MyChatDialog : public QMainWindow
{
    Q_OBJECT
signals:
    //添加好友的信号
    void SIG_AddFriend();
    //关闭窗口信号
    void SIG_Close();
public:
    MyChatDialog(QWidget *parent = nullptr);
    ~MyChatDialog();
    //添加好友
    void slot_addFriend(UserItem* userItem);
    //设置ui属性
    void slot_setInfo(QString name,QString feeling,int icon);
    //重写窗口关闭事件
    void closeEvent(QCloseEvent*event);
private slots:
    void on_pb_tool1_clicked();
    //点击菜单项的处理函数
    void slot_dealMenu(QAction*action);
private:
    Ui::MyChatDialog *ui;
    //QVBoxLayout是一个垂直布局的层
    QVBoxLayout* m_layout;
    //菜单
    QMenu *m_menu;
};
#endif // MYCHATDIALOG_H
