#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QCloseEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void slot_addFriend(QString ip,QString hostName);
    void slot_delFriend(QString ip);
private slots:
    void on_tw_friend_cellDoubleClicked(int row, int column);
    void closeEvent(QCloseEvent* event);
signals:
    //双击某一行
    void SIG_userClient(QString ip);
    //关闭窗口
    void SIG_close();
private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
