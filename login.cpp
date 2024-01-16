#include "login.h"
#include "ui_login.h"
#include "widget.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QMessageBox>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{

    ui->setupUi(this);
    QVector<QToolButton*> qToolbtn;
    isShow.fill(false,8);

    //设置登录标题图标
    this->setWindowIcon(QIcon(":/images/qq.png"));
    this->setWindowTitle("QQ");

    //用户名列表
    QStringList Namelist;
    Namelist<<"蔡徐坤"<<"赵本三"<<"华晨宇"<<"朴树"<<"许巍"<<"新裤子"<<"陈奕迅";
    //图片资源列表
    QStringList Iconlist;
    Iconlist<<"person1"<<"person2"<<"person3"<<"person4"<<"person5"<<"person6"<<"person7";


    //显示用户列表
    for(int i=0; i<7; i++)
    {
        QToolButton *btn = new QToolButton(this);
        btn->setIcon(QPixmap(QString(":/images/%1.jpg").arg(Iconlist[i])));
        //btn->setIconSize(QPixmap(QString(":/images/%1.jpg").arg(Iconlist[i])).size());
        btn->setIconSize(QSize(70, 70));
        btn->setAutoRaise(true);//鼠标悬停变亮
        //设置网名
        btn->setText(QString("%1").arg(Namelist[i]));

        //设置显示格式
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//设置按钮风格文字在图标 旁边
        ui->verticalLayout_2->addWidget(btn);

        qToolbtn.push_back(btn);
        //isShow.push_back(false);
    }

    for(int i=0;i<7;i++)
    {
        connect(qToolbtn[i],&QToolButton::clicked,[=](){
            if(isShow[i] == true)
            {
                QMessageBox::warning(this,"警告","该聊天框已被打开");
                return;
            }

            //点击了打开聊天框
            isShow[i] = true;
            Widget *widget = new Widget(0,qToolbtn[i]->text());
            widget->setWindowIcon(qToolbtn[i]->icon());
            widget->setWindowTitle(qToolbtn[i]->text());
            widget->show();

            //关闭聊天框时将对应的isShow更改为false
            connect(widget,&Widget::closeWiddget,[=](){
                isShow[i] = false;
            });

        });
    }
}

login::~login()
{
    delete ui;
}
