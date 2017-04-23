#include "client_thread.h"

/*================================================================
 * 构造函数：Thread_Client()
 =================================================================
 */
Thread_Client::Thread_Client()
{
    stopped = false;
}

/*================================================================
 * 析构函数：~Thread_Client()
 =================================================================
 */
Thread_Client::~Thread_Client()
{
    Client_SURFACE.CloseAll();
}

/*================================================================
 * 函数：stop()
 *
 * 说明：目前没什么用，不过可以后期拓展
 =================================================================
 */
void Thread_Client::stop()
{
    mutex.lock();
    stopped = true;
    mutex.unlock();
}

/*================================================================
 * 函数：run()
 *
 * 说明：线程运行函数；
 *      流程基本上同于NCClient中的ClientFunction()函数；
 =================================================================
 */
void Thread_Client::run()
{
    // 设置服务器地址
    Client_SURFACE.SetServerAddress();
    // 创建socket
    Client_SURFACE.CreateSocket();

    string tmpstr;
    if(Client_SURFACE.getClientState() == CLIENT_UNLINK_SOCKET)
        tmpstr = "Error: Server's Creating Socket ERROR:";
    else
        tmpstr = "Server's Creating Socket SUCCESS.";

    emit SendMessage(QString::fromStdString(tmpstr));

    if(Client_SURFACE.ConnectServer() == 0)
    {
        // FD集合初始化与设置
        Client_SURFACE.SetClientFDSet(CLIENT_OUTPUT);
        Client_SURFACE.InitClientName2Server();
        while(1)
        {
            /*==============================
             * (1) 文件描述符(File Descriptor，即FD)集合初始化;
             *      由于客户端的主要任务在于处理数据并发送给上位机，所以基
             * 本任务在于输出数据，即标准输出描述字STDOUT_FILENO
             ===============================
             */
            // FD集合初始化与设置
            Client_SURFACE.SetClientFDSet(CLIENT_OUTPUT);

            /*==============================
             * (2) 客户端选择FD集
             *      A. 首先执行ClientSelect函数，客户端选择FD集，可读可写文件，设置超时时间
             *      B. 判断ClientSelect函数的返回值
             *      C. 如果返回FALSE，则说明Select出现错误或TIMEOUT，则跳出该次循环
             ===============================
             */
            if(!Client_SURFACE.ClientSelect(-1))
                continue;

            /*==============================
             * (3) 客户端处理数据部分
             *      各客户端真正实现多态性的所在，不同客户端在此处体现不同的功能
             ===============================
             */
            Client_SURFACE.ClientProcessing();

             /*==============================
              * (4) 向服务器传输数据
              *     此处作为基类，只定义了SendToServer()虚函数，并未真正实现；
              *     真正实现了该部分的代码在该基类NCClient中的CommunicateTest()中
              * 实现，所以可参照CommunicateTest()函数写派生类对应的SendToServer()函数内容；
              ===============================
             */
            Client_SURFACE.SendToServer();

            /*==============================
             * (5) 从服务器接受数据
             *     重点在于客户端不同，对指令处理的函数也不同；
             ===============================
            */
            Client_SURFACE.ReceiveFromServer();

            string tmpstr = Client_SURFACE.getMessage();
            emit SendMessage(QString::fromStdString(tmpstr));

            /*==============================
             * (6) 客户端数据更新
             *     例如图像的waitkey，每次发送数据之间的时间间隔；
             ===============================
             */
            Client_SURFACE.ClientDataUpdate();
        }
    }
}

/*================================================================
 * 函数：SurfSendCMD(QString str)
 * 参数：
 *      QString str:  传递给服务器端执行的指令
 *
 * 说明：
 *      传递命令给服务器端，令服务器端执行该指令；
 =================================================================
 */
void Thread_Client::SurfSendCMD(QString str)
{
    string tmpstr = str.toStdString();
    Client_SURFACE.SurfSendCMD(tmpstr);
}
