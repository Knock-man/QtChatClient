#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>
#include <QFont>
#include <QDateTime>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
Widget::Widget(QWidget *parent,QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    myname = name;

    port = 9090;
    udpSocket = new QUdpSocket(this);

    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);


    //发送按钮
    connect(ui->pushButton_Send,&QPushButton::clicked,[=](){
        sendMsg(Msg);//发送普通信息
    });

    //监听接受信号
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::receiveMessage);

    //新用户加入
   sendMsg(User_Enter);

   //退出按钮
   connect(ui->pushButton_exit,&QPushButton::clicked,[=](){
       this->close();
   });

   //字体
   connect(ui->fontComboBox_FontStyle,&QFontComboBox::currentFontChanged,[=](const QFont &font){
       //设置输入框字体样式
       ui->textEdit->setFontFamily(font.toString());
       ui->textEdit->setFocus();
   });

   //字体大小
   void (QComboBox:: *sizeBtn)(const QString &text) = &QComboBox::currentTextChanged;
   connect(ui->comboBox_FontSize,sizeBtn,this,[=](const QString &text)
   {
        ui->textEdit->setFontPointSize(text.toDouble());
        ui->textEdit->setFocus();
   });

   //加粗
   connect(ui->toolButton_B,&QPushButton::clicked,[=](bool checked){
       //是否按下
       if(checked)
       {
           ui->textEdit->setFontWeight(QFont::Bold);
       }else{
           ui->textEdit->setFontWeight(QFont::Normal);
       }
   });

   //倾斜
   connect(ui->toolButton_I,&QPushButton::clicked,[=](bool checked){
       //是否按下
        ui->textEdit->setFontItalic(checked);
        ui->textEdit->setFocus();
   });

   //下划线
   connect(ui->toolButton_U,&QToolButton::clicked,this,[=](bool checked){
       ui->textEdit->setFontUnderline(checked);
       ui->textEdit->setFocus();
   });

   //清空
   connect(ui->toolButton_Clean,&QToolButton::clicked,this,[=](){
       ui->textEdit->clear();
   });

   //字体颜色
   connect(ui->toolButton_Color,&QToolButton::clicked,this,[=](){
       QColor color=QColorDialog::getColor(color,this);
        ui->textEdit->setTextColor(color);
   });

   //保存聊天记录
   connect(ui->toolButton_Save,&QToolButton::clicked,this,[=](){
       if(ui->textBrowser->toPlainText().isEmpty()){
           QMessageBox::warning(this,"警告","保存内容不能为空");
           return;
       }
       //获取保存文件绝对路径/文件名(窗口标题,文件名,类型)
       QString fileName=QFileDialog::getSaveFileName(this,"保存聊天记录","聊天记录","(*.txt)");
       if(!fileName.isEmpty()){
           QFile file(fileName);
           file.open(QIODevice::WriteOnly|QFile::Text);
           QTextStream stream(&file);
           stream<<ui->textBrowser->toPlainText();
           file.close();
       }
   });


}

Widget::~Widget()
{
    delete ui;
}

//重写关闭事件
void Widget::closeEvent(QCloseEvent *event)
{
    emit this->closeWiddget();
    sendMsg(User_Left);
    udpSocket->close();
    udpSocket->destroyed();
}



//获取名字
QString Widget::getName()
{
    return myname;
}

//获取聊天信息
QString Widget::getMsg()
{
    QString msg = ui->textEdit->toHtml();//放回输入框的内容
    ui->textEdit->clear();//清空输入框
    ui->textEdit->setFocus();//将焦点重新设置在输入框上
    return msg;
}

//处理用户进入
void Widget::userEnter(QString username,QString time)
{
    bool isEmpty = ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty();
    if(isEmpty)//在线信息为空
    {
        //在线表格操作
        QTableWidgetItem *user = new QTableWidgetItem(username);//创建单元格
        ui->tableWidget->insertRow(0);//表格顶部插入一行
        ui->tableWidget->setItem(0,0,user);//单元格添加入表格

        //聊天栏操作
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->append(time+" 用户"+username+"已上线");

        //在线人数操作
        ui->label_onlineNumber->setText(QString("群在线人数:%1人").arg(ui->tableWidget->rowCount()));

        sendMsg(User_Enter);

    }

}

//处理用户离开
void Widget::userLeft(QString username,QString time)
{
    bool isEmpty = ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty();//在线列表是否存在
    if(!isEmpty)//在线信息不为空
    {
        //在线表格操作
        int row = ui->tableWidget->findItems(username,Qt::MatchExactly).first()->row();
        ui->tableWidget->removeRow(row);//表格顶部插入一行


        //聊天栏操作
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->append(time+" 用户"+username+"已下线");

        //在线人数操作
        ui->label_onlineNumber->setText(QString("群在线人数:%1人").arg(ui->tableWidget->rowCount()));
    }
}


//广播形式发送聊天信息
void Widget::sendMsg(msgType type)
{
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
   stream<<type<<this->getName();//用户名 消息类型写入流

   switch (type) {
       case Msg:{
            if(ui->textEdit->toPlainText()=="")
            {
                QMessageBox::warning(this,"警告","发送的聊天内容不能为空");
                return;
            }
            stream<<this->getMsg();//聊天信息写入流
            break;

       }
       case User_Enter:{
            break;
       }
       case User_Left:{
            break;
       }
   }

   //发送报文
   udpSocket->writeDatagram(array.data(),array.size(),QHostAddress::Broadcast,this->port);
}

//接受udp信息（槽函数）
void Widget::receiveMessage()
{
    qint64 size = this->udpSocket->pendingDatagramSize();//获取数据包大小
    QByteArray array = QByteArray(size,0);
    udpSocket->readDatagram(array.data(),size);//读取后缓冲区不会清空
    QDataStream stream(&array,QIODevice::ReadOnly);
    int mytype;
    QString name,msg;//用户名，聊天内容
    QString time = QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss");
    stream>>mytype;
    switch (mytype) {
        case Msg://聊天信息
            stream>>name>>msg;
            //聊天信息输出样式
            ui->textBrowser->setTextColor(QColor(Qt::blue));
            ui->textBrowser->append("["+name+"]"+time);
            ui->textBrowser->append(msg);
        case User_Left://用户退出信息
            stream>>name;
            userLeft(name,time);
            break;
        case User_Enter://用户进入信息
            stream>>name;
            userEnter(name,time);
            break;
    }

}

