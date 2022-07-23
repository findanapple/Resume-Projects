#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_addFriend(QString ip, QString hostName)
{
    qDebug()<<__func__;
    //1.先把总行号+1
    ui->tw_friend->setRowCount(ui->tw_friend->rowCount()+1);
    //2.在最后一行添加两个item
    QTableWidgetItem* item1=new QTableWidgetItem(ip);
    ui->tw_friend->setItem(ui->tw_friend->rowCount()-1,0,item1);
    QTableWidgetItem* item2=new QTableWidgetItem(hostName);
    ui->tw_friend->setItem(ui->tw_friend->rowCount()-1,1,item2);
}

void MainWindow::slot_delFriend(QString ip)
{
    qDebug()<<__func__;
    for(int i=0;i<ui->tw_friend->rowCount();i++){
        if(ip==ui->tw_friend->item(i,0)->text()){
            ui->tw_friend->removeRow(i);
            break;
        }
    }
    this->update();
}

void MainWindow::on_tw_friend_cellDoubleClicked(int row, int column)
{
    qDebug()<<__func__;
    //qDebug()<<QString("[%1],[%2]").arg(row).arg(column);
    //获取点击那一行第0列的ip地址
    QString ip=ui->tw_friend->item(row,0)->text();
    qDebug()<<ip;
    Q_EMIT SIG_userClient(ip);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<__func__;
    if(QMessageBox::question(this,"提示","是否关闭")==QMessageBox::Yes){
        Q_EMIT SIG_close();
    }else{
        event->ignore();
    }

}

