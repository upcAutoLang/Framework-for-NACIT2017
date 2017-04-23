#ifndef NCSTRATEGYMAINWIN_H
#define NCSTRATEGYMAINWIN_H

#include <QMainWindow>

#include "client_thread.h"

//#include "ncclient_surface.h"

namespace Ui {
class NCStrategyMainWin;
}

class NCStrategyMainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit NCStrategyMainWin(QWidget *parent = 0);
    ~NCStrategyMainWin();

    //===========================
    //   QString字符串操作
    //===========================
    QString RemoveSubStr(QString str, QString substr);
    QString RemoveSubStr(QString str, char substr[]);

    // 界面显示Stage信息
    void ShowStage();
    // 界面显示Mission信息
    void ShowMission();
    // 界面显示Send Clien信息
    void ShowSendData();
    // 界面显示Image Client信息
    void ShowImageData();

private slots:
    //===========================
    //   界面控件操作槽函数
    //===========================
    void on_ServerRunButton_clicked();
    void on_ServerCMDSendButton_clicked();
    // 发送该信号，将从界面客户端获取的信息显示在Qt界面上
    void SurfaceShow(QString str);

private:
    Ui::NCStrategyMainWin *ui;
    // 界面客户端线程
    Thread_Client* threadClient;
    // 界面客户端是否运行的标志位
    // （也许没什么用，基本功能既然已经完成，笔者也懒得完善了）
    bool isSurfRun = false;
    // 存储界面客户端传来的信息
    QString ServerInfo;
};

#endif // NCSTRATEGYMAINWIN_H
