#include <ncclient.h>

/*===============================================================
 * 构造函数：NCClient()
 *
 * 说明：
 *      设置服务器地址，初始化客户端的各参数
 ================================================================
 */
NCClient::NCClient()
{
    // 客户端名称
    Name = "BASE";
    // 设置服务器地址
    SetServerAddress();
    // 测试用的数据，对其初始化
    send_i = 0;
    // 设置超时时间
    overtime = 2.0;
    // 切换任务标志位置为false
    isMissionChanged = false;
}

/*===============================================================
 * 析构函数：~NCClient()
 *
 * 说明：
 *      退出客户端，断开与服务器的连接
 ================================================================
 */
NCClient::~NCClient()
{
    close(ServerSocketFD);
}

/*===============================================================
 * 函数：SetServerAddress()
 *
 * 说明：使该客户端连接到指定服务器（此处服务器地址为回归地址127.0.0.1）
 *
 * 注：sockaddr_in结构体原型：
 *      struct sockaddr_in {short sin_family; unsigned short sin_port;
 *                                      struct in_addr sin_addr; char sin_zero[8]; };
 * 功能：sockaddr, sockaddr_in都是用于处理网络通信地址的结构体
 *            short sin_family:                     2 Bytes, 如AF_INET, AF_INET6
 *            unsigned short sin_port:        2 Bytes 如htons(3490)
 *            struct in_addr sin_addr:          4 Bytes, 地址，其实是int类型
 *            char sin_zero[8]:                     如果需要的话，即为8 Bytes的0
 ================================================================
 */
void NCClient::SetServerAddress()
{
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(SERVER_PORT);
    ServerAddress.sin_addr.s_addr = inet_addr(LAN_ADDRESS);
    bzero(&(ServerAddress.sin_zero), 8);
}

/*===============================================================
 * 函数：CreateSocket()
 *
 * 说明：创建socket
 ================================================================
 */
void NCClient::CreateSocket()
{
    /*===============================================================
     * 函数原型：int socket(int domain, int type, int protocol);
     * 功能：创建socket
     *      int domain: 协议族，对于本地套接字来说，其值须被置为 AF_UNIX 枚举值
     *      int type: 套接字类型，可被设置为流式套接字SOCK_STREAM或SOCK_DGRAM（数据报式套接字）
     *      int protocol: 返回值为生成的套接字描述符，应被设置为0
     ================================================================
     */
    ServerSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    // socket创建失败，弹出错误信息
    if(ServerSocketFD == -1)
    {
        perror("Error: Server's Creating Socket ERROR.");
        ClientState = CLIENT_UNLINK_SOCKET;
    }
    else
        ClientState = CLIENT_LINKING;
}

/*===============================================================
 * 函数：ClientFunction()
 *
 * 说明：客户端实际工作部分
 *      连接完成后，则开始完成客户端工作
 *
 * 注：
 * 1. 该函数没有被设定为虚函数，但内部的具体函数是被设置为虚函数的，
 *      这样就保证了ClientFunction中每个重要部分函数的多态性，也就保证了
 *      该函数的多态性
 * 2. 客户端需要持续工作，所以整体是一个大循环；
 * 3. 进入循环之前，先对客户端进行数据初始化，即ClientInit()；
 * 4. 循环内部主要分四步：
 *      (1) 文件描述符(File Descriptor，即FD)集合初始化;
 *      (2) 客户端选择FD集；
 *      (3) 客户端处理数据；
 *      (4) 向服务器传输数据；
 *      (5) 从服务器接受数据；
 *      (6) 客户端数据更新；
 ================================================================
 */
void NCClient::ClientFunction()
{
    if(connect(ServerSocketFD,
               (struct sockaddr *)&ServerAddress,
               sizeof(struct sockaddr_in)) == 0)
    {
        // 服务器客户端connect成功后，对该客户端进行数据初始化工作
        ClientInit();
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

/*===============================================================
 * 函数：ClientInit()
 *
 * 说明：服务器客户端connect成功后，对该客户端进行数据初始化工作；
 *
 * 注：
 *      此处为基类虚函数，故只列出该函数的存在，并没有放入实际内容；
 * 从NCClient派生出的函数必须填入派生客户端的初始化数据；
 ================================================================
 */
void NCClient::ClientInit()
{
    ;
}

/*===============================================================
 * 函数：SetClientFDSet(Client_Mode mode)
 *
 * 说明：FD集合初始化与设置；
 *
 * 参数：
 * Client_Mode mode:        客户端工作模式：
 *                                          1. 标准输入描述字（键盘输入）；2. 标准输出描述字
 ================================================================
 */
void NCClient::SetClientFDSet(Client_Mode mode)
{
    // FD_ZERO: 将指定的文件描述符集合清空；
    // 对文件描述符集合进行设置之前，必须对其进行初始化；由于系统分配内存空间时并不作清空处理，所以如果此处不清空，结果是不可知的
    FD_ZERO(&ClientFDSet);

    // FD_SET: 在文件描述符集合中，增加一个新的文件描述符
    switch(mode)
    {
    case CLIENT_INPUT:
    {
        // 对于STDIN_FILENO，连接键盘输入
        FD_SET(STDIN_FILENO, &ClientFDSet);
        break;
    }
    case CLIENT_OUTPUT:
    {
        FD_SET(STDOUT_FILENO, &ClientFDSet);
        break;
    }
    default:
        cout<<Name<<" Client ERROR: File Descriptor Set's MODE is WRONG."<<endl;
        break;
    }

    // 连接服务器端
    FD_SET(ServerSocketFD, &ClientFDSet);
}

/*===============================================================
 * 函数：ClientProcessing()
 *
 * 说明：客户端处理数据部分
 *      各客户端真正实现多态性的所在，不同客户端在此处体现不同的功能；
 * 此处为基类虚函数，故只列出该函数的存在，并没有放入实际内容；
 ================================================================
 */
void NCClient::ClientProcessing()
{
    ;
}

/*===============================================================
 * 函数：ClientSelect(double time)
 *
 * 参数：
 * double time:         超时时间，单位秒
 *
 * 说明：
 *      客户端选择FD集，可读可写文件，设置超时时间
 ================================================================
 */
bool NCClient::ClientSelect(double time)
{
    tv.tv_sec = (int)time;
    tv.tv_usec = (int)(time - (int)time) * 1000000;

    /*===============================================================
     * 函数原型：
     *      int select (int maxfds, fd_set *readfds, fd_set *writefds,
     *                       fd_set *exceptfds, struct timeval *timeout);
     *
     * 功能：
     * select函数实现I/O端口的复用，传递给 select函数的参数会告诉内核：
     * 1、我们关心的文件描述符
     * 2、我们关心的每个描述符的状态（从一个描述符中读或写/关注一个描述符中是否出现异常）
     * 3、我们需要等待多长时间（无限长时间/固定时间/不等待）
     *
     * select函数返回后，内核告诉我们的信息：
     * 1、按照我们的要求，已经做好准备的描述符个数
     * 2、对于三种条件（读/写/异常）哪些描述符做好了准备
     *
     * 参数信息：
     *  int maxfdp: 集合中所有文件描述符的范围，所有文件描述符的最大值+1，不能错误！！
     *  fd_set *readfds: 指向fd_set结构的指针，包括文件描述符，我们要监视是否可以从这些文件中读取数据了
     *      如果集合中有一个文件可读，select会返回一个大于0的值，表示有文件可读；
     *      如果没有可读文件，则根据timeout参数判断是否超时。如果超出timeout时间，则返回0；如果发生错误返回负值
     *  fd_set *writefds: 指向fd_set结构的指针，应该包括文件描述符，我们要监视是否可以向这些文件中写入数据了。
     *      如果这个集合中有一个文件可写，select会返回大于0的值，表示有文件可写；
     *      如果没有可写的文件，则根据timeout判断是否超时；若超出timeout时间，select返回0，若发生错误，返回负值；
     *  fd_set *errorfds: 同上面两个参数的目的，用来监视文件错误异常文件；
     *  struct timeval *timeout: select的超时时间，至关重要的参数；
     *      不同的timeout值，可以使select处于三种状态：
     *      (1) NULL，不传入时间结构，select处于阻塞状态，一定等到文件描述符集合中某个文件描述符发生变化为止；
     *      (2) 时间值0秒0毫秒，纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行；此时文件无变化，则返回0；有变化，则返回一个正值；
     *      (3) timeout > 0的值，等待的超时时间，即在超时时间之内有事件，则返回；否则超时后不管怎样都一定返回，返回值如(2)；
     *
     ================================================================
    */
    int ret;
    if(tv.tv_sec < 0 || tv.tv_usec < 0)
    {
        ret = select(ServerSocketFD + 1,    // int maxfdp
                             &ClientFDSet,          // fd_set *readfds
                             NULL,                 // fd_set *writefds
                             NULL,                 // fd_set *errorfds
                             NULL);                   // struct timeval *timeout
    }
    else
    {
        ret = select(ServerSocketFD + 1,    // int maxfdp
                     &ClientFDSet,          // fd_set *readfds
                     NULL,                 // fd_set *writefds
                     NULL,                 // fd_set *errorfds
                     &tv);                   // struct timeval *timeout
    }
    // select错误信息
    if(ret < 0)
    {
        perror("Error: SEND Client Select ERROR.");
        return false;
    }
    else if(ret == 0)
    {
        cout<<Name<<" Client Select TIMEOUT."<<endl;
        return false;
    }
    else
        return true;
}

/*===============================================================
 * 函数：CommunicateTest()
 *
 * 说明：
 *      测试部分，向服务器传递数据
 *      该函数实现了服务器传输函数SendToServer()的实际功能，作为测试函数使用；
 ================================================================
 */
void NCClient::CommunicateTest()
{
    if(FD_ISSET(STDOUT_FILENO, &ClientFDSet))
    {
        // 测试部分用，持续向程序发送数据：
        bzero(input_msg, BUFFER_SIZE);
        char *numchar;
        numchar = int2char(send_i);
        send_i++;

        memset(input_msg, 0, strlen(input_msg));
        strcat(input_msg, string2char(Name));
        strcat(input_msg, " Client TEST: ");
        strcat(input_msg, numchar);
        printf("%s\n", input_msg);

        if(send(ServerSocketFD, input_msg, BUFFER_SIZE, 0) == -1)
            perror("Error: Client TEST Send ERROR.");
    }
}

/*===============================================================
 * 函数：SendToServer()
 *
 * 说明：向服务器传输数据
 *      此处作为基类，只定义了SendToServer()虚函数，并未真正实现；
 *      真正实现了该部分的代码在该基类NCClient中的CommunicateTest()中
 * 实现，所以可参照CommunicateTest()函数写派生类对应的SendToServer()函数内容；
 ================================================================
 */
void NCClient::SendToServer()
{
    ;
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
void NCClient::ReceiveFromServer()
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
            // 不同客户端，在此处有不同的处理
            CmdProcess();
        }
        else if(byte_num < 0)
            cout<<"Error: "<<Name<<" Client Receive ERROR"<<endl;
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
 * 函数：CutServerCmd()
 *
 * 说明：将服务器传来的信息切分为：客户端名称 + 对应指令
 *
 * 注：
 *      1. 将整条指令切分成两部分
 *      2. 将切分完毕的两部分（客户端名称 + 具体指令）全部大写化处理；
 ================================================================
 */
void NCClient::CutServerCmd()
{
    RecvTotalCmd = recv_msg;
    CutCommand(RecvTotalCmd, RecvName, RecvCmd);

    transform(RecvName.begin(), RecvName.end(),
              RecvName.begin(), ::toupper);
    transform(RecvCmd.begin(), RecvCmd.end(),
              RecvCmd.begin(), ::toupper);
}

/*===============================================================
 * 函数：CmdProcess()
 *
 * 说明：客户端按照本身的处理方法，对服务器传来的指令进行反应
 *      此处作为基类，只定义了CmdProcess虚函数，并未真正实现；
 *      真正实现了该部分的代码在该基类NCClient中的CommunicateTest()中
 * 实现，所以可参照CommunicateTest()函数写派生类对应的SendToServer()函数内容；
 ================================================================
 */
void NCClient::CmdProcess()
{

}

/*===============================================================
 * 函数：ClientDataUpdate()
 *
 * 说明：客户端数据更新；
 *      此处作为基类，只定义了ClientDataUpdate()虚函数，并未真正实现；
 *      真正实现了该部分的代码在该基类NCClient中的CommunicateTest()中
 * 实现，所以可参照CommunicateTest()函数写派生类对应的SendToServer()函数内容；
 *
 * 使用举例：
 *      例如IMAGE客户端中的waitkey，SEND客户端中每次发送数据之间的时间间隔usleep();
 ================================================================
 */
void NCClient::ClientDataUpdate()
{

}

/*===============================================================
 * 函数：PackageClientName()
 * 返回值：string
 *
 * 说明：将Client名称包装
 *      成员变量两端加上<NCName></NCName>，模拟HTML语言；
 ================================================================
 */
string NCClient::PackageClientName()
{
    string tmp;
    tmp += "<NCName>";
    tmp += Name;
    tmp += "</NCName>";
    return tmp;
}

/*===============================================================
 * 函数：InitClientName2Server()
 * 返回值：string
 *
 * 说明：客户端名称初始化，并发送给服务器
 *      客户端连接到服务器后第一个发给服务器的数据，用于确认该客户端的名称；
 ================================================================
 */
void NCClient::InitClientName2Server()
{
    if(FD_ISSET(STDOUT_FILENO, &ClientFDSet))
    {
        bzero(input_msg, BUFFER_SIZE);
        memset(input_msg, 0, strlen(input_msg));
        // 将客户端名称包装，并发送到服务器
        string tmp = PackageClientName();
//        strcat(input_msg, string2char(tmp));
        strcat(input_msg, tmp.c_str());
        strcat(input_msg, (char *)"\0");
        printf("%s\n", input_msg);
        int send_ret = send(ServerSocketFD, input_msg, BUFFER_SIZE, 0);
        if(send_ret == -1)
        {
            perror("Error: Client TEST Send ERROR.");
            cout<<"    Error Code: "<<strerror(errno)<<endl;
        }
        else if(send_ret == 0)
        {
            perror("Error: ");
            cout<<"    Error Code: "<<strerror(errno)<<endl;
        }
        else
            cout<<"Client "<<Name<<" Init Send DONE."<<endl;
    }
}
