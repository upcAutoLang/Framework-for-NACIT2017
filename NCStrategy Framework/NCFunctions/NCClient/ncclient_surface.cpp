#include "ncclient_surface.h"

NCClient_SURFACE::NCClient_SURFACE()
{
    // 客户端名称
    Name = NAME_CLIENT_SURF;
    // 设置服务器地址
    SetServerAddress();
//    // 测试用的数据，对其初始化
//    send_i = 0;
    // 设置超时时间
    overtime = CLIENT_UPTIME;
    // 切换任务标志位置为false
    isMissionChanged = false;
}

NCClient_SURFACE::~NCClient_SURFACE()
{
    close(ServerSocketFD);
}

// 服务器客户端connect成功后，对该客户端进行数据初始化工作
void NCClient_SURFACE::ClientInit()
{

}

// SEND客户端处理部分
void NCClient_SURFACE::ClientProcessing()
{
    ;
}

// SEND客户端与服务器的通讯
void NCClient_SURFACE::ReceiveFromServer()
{
    // 检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化
    if(FD_ISSET(ServerSocketFD, &ClientFDSet))
    {
        bzero(recv_msg, BUFFER_SIZE);
        long byte_num = recv(ServerSocketFD, recv_msg, BUFFER_SIZE, 0);
        if(byte_num > 0)
        {
            if(byte_num > BUFFER_SIZE)
                byte_num = BUFFER_SIZE;
            recv_msg[byte_num] = '\0';

            /*===============================================================
              * 指令处理部分
             ================================================================
             */
//            // 将服务器传来的信息切分：客户端名称 + 对应指令
//            CutServerCmd();

//            // 如果判断服务器传来的指令是针对该客户端，则该客户端进行处理
//            CmdProcess();

            RecvTotalCmd = recv_msg;
            CutCommand(RecvTotalCmd, RecvName, RecvCmd);
            transform(RecvName.begin(), RecvName.end(),
                      RecvName.begin(), ::toupper);

            // 如果判断服务器传来的指令是针对该客户端，则该客户端进行处理
            CmdProcess();
        }
        else if(byte_num < 0)
            cout<<"Error: "<<Name<<" Client Receive ERROR."<<endl;
        else
        {
            cout<<"Warning: The Server has Exited."<<endl;
            ClientState = CLIENT_UNLINK_NOSERVER;
            close(ServerSocketFD);
            exit(0);
        }
    }
}

// 客户端数据更新（例如图像的waitkey，每次发送数据之间的时间间隔）
void NCClient_SURFACE::ClientDataUpdate()
{

}

// 该客户端匹配到来自于服务器传来的信息后，处理服务器传来的指令
void NCClient_SURFACE::CmdProcess()
{
    if(RecvName == NAME_CLIENT_SURF)
    {
        Message = RecvCmd;
//        emit SendMessage(Message);
        cout<<"Client Surface Received: "<<endl<<"    ";
        cout<<RecvTotalCmd<<endl;
    }
}

// 向服务器传输数据
void NCClient_SURFACE::SendToServer()
{
    if(!isClientNameSet)
    {
        InitClientName2Server();
        isClientNameSet = true;
    }
}

string NCClient_SURFACE::getMessage()
{
    return Message;
}

void NCClient_SURFACE::SurfClientFunction()
{
    if(connect(ServerSocketFD,
               (struct sockaddr *)&ServerAddress,
               sizeof(struct sockaddr_in)) == 0)
    {
        while(1)
        {
        /*==============================
         * (1) 文件描述符(File Descriptor，即FD)集合初始化;
         *      由于客户端的主要任务在于处理数据并发送给上位机，所以基
         * 本任务在于输出数据，即标准输出描述字STDOUT_FILENO
         ===============================
         */
        // FD集合初始化与设置
        SetClientFDSet(CLIENT_OUTPUT);

        /*==============================
         * (2) 客户端选择FD集
         *      A. 首先执行ClientSelect函数，客户端选择FD集，可读可写文件，设置超时时间
         *      B. 判断ClientSelect函数的返回值
         *      C. 如果返回FALSE，则说明Select出现错误或TIMEOUT，则跳出该次循环
         ===============================
         */
        if(!ClientSelect(overtime))
            continue;
            ;

        /*==============================
         * (3) 客户端处理数据部分
         *      各客户端真正实现多态性的所在，不同客户端在此处体现不同的功能
         ===============================
         */
        ClientProcessing();

         /*==============================
          * (4) 向服务器传输数据
          *     此处作为基类，只定义了SendToServer()虚函数，并未真正实现；
          *     真正实现了该部分的代码在该基类NCClient中的CommunicateTest()中
          * 实现，所以可参照CommunicateTest()函数写派生类对应的SendToServer()函数内容；
          ===============================
         */
        SendToServer();
//            CommunicateTest();

        /*==============================
         * (5) 从服务器接受数据
         *     重点在于客户端不同，对指令处理的函数也不同；
         ===============================
        */
//            if(isClientNameSet)
        ReceiveFromServer();

        /*==============================
         * (6) 客户端数据更新
         *     例如图像的waitkey，每次发送数据之间的时间间隔；
         ===============================
         */
        ClientDataUpdate();
        }
    }
}
