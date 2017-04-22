#include <ncclient_send.h>

/*===============================================================
 * 构造函数：NCClient_SEND()
 *
 * 说明：
 *      设置服务器地址，初始化客户端的各参数
 ================================================================
 */
NCClient_SEND::NCClient_SEND()
{
    // 客户端名称
    Name = NAME_CLIENT_SEND;
    // 设置服务器地址
    SetServerAddress();
    // 测试用的数据，对其初始化
    send_i = 0;
    // 设置超时时间
    overtime = CLIENT_UPTIME;
    // 切换任务标志位置为false
    isMissionChanged = false;
}

/*===============================================================
 * 析构函数：~NCClient_SEND()
 *
 * 说明：
 *      退出客户端，断开与服务器的连接
 ================================================================
 */
NCClient_SEND::~NCClient_SEND()
{
    close(ServerSocketFD);
}

// 服务器客户端connect成功后，对该客户端进行数据初始化工作
/*===============================================================
 * 函数：ClientInit()
 *
 * 说明：服务器客户端connect成功后，对该客户端进行数据初始化工作；
 *
 * 注：
 *      由于此处仅仅测试使用，所以并没有什么数据需要初始化……
 ================================================================
 */
void NCClient_SEND::ClientInit()
{
    ;
}

/*===============================================================
 * 函数：ClientProcessing()
 *
 * 说明：客户端处理数据部分
 *
 * 注：
 *      由于此处仅仅测试使用，所以并没有什么数据需要处理……
 ================================================================
 */
void NCClient_SEND::ClientProcessing()
{

}

/*===============================================================
 * 函数：ReceiveFromServer()
 *
 * 说明：从服务器接受数据
 * 1. 检测服务器套接字文件描述符(server_sock_fd)；
 * 2. 指令进行处理
 *      A. 切分服务器发来的信息；
 *      B. 客户端用自己的指令分析函数，进行数据的处理；
 * 3. 若接收信息出错，则返回错误信息；
 ================================================================
 */
void NCClient_SEND::ReceiveFromServer()
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
            // 将服务器传来的信息切分：客户端名称 + 对应指令
            CutServerCmd();

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

/*===============================================================
 * 函数：CmdProcess()
 *
 * 说明：客户端按照本身的处理方法，对服务器传来的指令进行反应
 *      此处仅仅识别了服务器传来消息是对应SEND客户端，由于只作测试功能，
 *  所以并没有进一步解读下面的具体操作
 ================================================================
 */
void NCClient_SEND::CmdProcess()
{
    if(RecvName == NAME_CLIENT_SEND)
    {
        cout<<"Server Totol Command:"<<endl<<"    "<<RecvTotalCmd<<endl;
    }
}

/*===============================================================
 * 函数：ClientDataUpdate()
 *
 * 说明：客户端数据更新；
 *      SEND客户端中每次发送数据之间的时间间隔usleep();
 ================================================================
 */
void NCClient_SEND::ClientDataUpdate()
{
    usleep(300000);
}

/*===============================================================
 * 函数：SendToServer()
 *
 * 说明：向服务器传输数据
 *      仅仅为SEND客户端作测试用，静待日后更改；
 ================================================================
 */
void NCClient_SEND::SendToServer()
{
    if(isClientNameSet)
        CommunicateTest();
    // 该部分只执行一次，用于在服务器端对该客户端的名称进行对应操作
    else
    {
        InitClientName2Server();
        isClientNameSet = true;
    }
}
