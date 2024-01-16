#include "widget.h"
#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Widget w;
    //w.show();
    //一开始显示登录界面
    login mylogin;
    mylogin.show();
    return a.exec();
}
