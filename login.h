#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QVector>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private:
    Ui::login *ui;

    //初始状态为未打开登录后的界面
    QVector<bool> isShow;
};

#endif // LOGIN_H
