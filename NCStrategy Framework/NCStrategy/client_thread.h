#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H

#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QObject>
#include <QThread>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "ncclient_surface.h"

using namespace std;

class Thread_Client : public QThread
{
    Q_OBJECT

public:
    Thread_Client();
    ~Thread_Client();
    // 线程运行函数
    void run();
    // 停止函数（暂时没用）
    void stop();
    // 传递命令给服务器端，令服务器端执行该指令；
    void SurfSendCMD(QString str);

private:
    QMutex mutex;
    volatile bool stopped;
    NCClient_SURFACE Client_SURFACE;

signals:
    void SendMessage(QString str);

};

#endif // CLIENT_THREAD_H
