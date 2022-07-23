#include "logindialog.h"
#include "ui_logindialog.h"

#include<QMessageBox>
#include<QRegularExpression>
#include<QRegularExpressionMatch>

logindialog::logindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logindialog)
{
    ui->setupUi(this);
}

logindialog::~logindialog()
{
    delete ui;
}

void logindialog::on_pb_clear_clicked()
{
    ui->le_tel->setText("");
    ui->le_password->setText("");
}

void logindialog::on_pb_commit_clicked()
{
    //1.获取控件上的数据
    QString tel=ui->le_tel->text();
    QString password=ui->le_password->text();
    //2.校验数据的合法性(tel不能为空，不能为空格，密码不能为空
    //长度限制，手机号必须是11位，密码不能超过10位
    //内容校验:手机号只能是数字，密码只能是数字、字母、下划线（需要使用正则表达式
            //手机号（^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$）
            //密码：^[a-zA-Z]\w{5,17}$）)
    QString teltemp=tel;
    if(tel.isEmpty()||password.isEmpty()||teltemp.remove(" ").isEmpty()/*去掉空格还是不是空字符串*/){
        QMessageBox::about(this,"提示","手机号或密码不能为空！");
        return;
    }
    //匹配手机号
    QRegularExpression rg_tel("^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\\d{8}$");
    QRegularExpressionMatch match_tel=rg_tel.match(tel);
    if(!match_tel.hasMatch()){
        QMessageBox::about(this,"提示","不符合手机号格式");
        return;
    }
    //匹配密码
    QRegularExpression rg_pw("^[a-zA-Z]\\w{5,17}$");
    QRegularExpressionMatch match_pw=rg_pw.match(password);
    if(!match_pw.hasMatch()){
        QMessageBox::about(this,"提示","密码以字母开头，长度在6~18之间，只能包含字母、数字和下划线");
        return;
    }
    //3.把数据发给kernel类
    Q_EMIT SIG_LoginCommit(tel,password);

}

void logindialog::on_pb_clear_register_clicked()
{
    ui->le_name_register->setText("");
    ui->le_tel_register->setText("");
    ui->le_password_register->setText("");
}

void logindialog::on_pb_commit_register_clicked()
{
    //1.获取控件上的数据
    QString tel=ui->le_tel_register->text();
    QString password=ui->le_password_register->text();
    QString name=ui->le_name_register->text();
    //2.校验数据的合法性(tel不能为空，不能为空格，密码不能为空
    //长度限制，手机号必须是11位，密码不能超过10位
    //内容校验:手机号只能是数字，密码只能是数字、字母、下划线（需要使用正则表达式
            //手机号： ^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$
            //密码(以字母开头，长度在6~18之间，只能包含字母、数字和下划线)：^[a-zA-Z]\w{5,17}$
            //姓名：中文、英文、数字包括下划线：^[\u4E00-\u9FA5A-Za-z0-9_]+$
    QString teltemp=tel;
    QString nametemp=name;
    if(tel.isEmpty()||name.isEmpty()||password.isEmpty()||teltemp.remove(" ").isEmpty()||nametemp.remove(" ").isEmpty()/*去掉空格还是不是空字符串*/){
        QMessageBox::about(this,"提示","手机号或密码或昵称不能为空！");
        return;
    }

    //匹配手机号
    QRegularExpression rg_tel("^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\\d{8}$");
    QRegularExpressionMatch match_tel=rg_tel.match(tel);
    if(!match_tel.hasMatch()){
        QMessageBox::about(this,"提示","不符合手机号格式");
        return;
    }
    //匹配密码
    QRegularExpression rg_pw("^[a-zA-Z]\\w{5,17}$");
    QRegularExpressionMatch match_pw=rg_pw.match(password);
    if(!match_pw.hasMatch()){
        QMessageBox::about(this,"提示","密码以字母开头，长度在6~18之间，只能包含字母、数字和下划线");
        return;
    }
    //匹配姓名
    QRegularExpression rg_name("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegularExpressionMatch match_name=rg_name.match(name);
    if(!match_name.hasMatch()){
        QMessageBox::about(this,"提示","姓名可以包含中文、英文、数字以及下划线");
        return;
    }
    //3.把数据发给kernel类
    Q_EMIT SIG_RegisterCommit(name,tel,password);
}
//重写关闭窗口事件
void logindialog::closeEvent(QCloseEvent *event)
{
    //1.接受关闭事件
    event->accept();
    //2.给kernel发信号，通知kernel窗口关闭
    Q_EMIT SIG_Close();
}




