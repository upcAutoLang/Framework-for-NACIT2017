#include "ncserver.h"

/*================================================================
 * 构造函数: NCServer()
 =================================================================
 */
NCServer::NCServer()
{
    // 初始化服务器端socket信息
    InitServerSock();

    // 设置超时时间，此处设置为2s
    SetTimeValue(SERVER_UPTIME);
    State = SERVER_LINKING;
    // 创建记录文件夹；
    CreateRecordFolders();
    // 在文件夹中生成一系列记录文件；
    CreateRecordFiles();
    // 初始化客户端文件描述符信息
    InitClientFDSet();
}



/*================================================================
 * 析构函数: ~NCServer()
 *
 * 说明：
 *      退出时，关闭服务器与客户端的文件描述符
 =================================================================
 */
NCServer::~NCServer()
{
    close(server_sock_fd);
    close(client_sock_fd);
    ServerStrm.close();
    ImageStrm.close();
    SendStrm.close();
    InfoStrm.close();
    StageStrm.close();
    ElogStrm.close();
}

/*================================================================
 * 函数: InitClientFDSet()
 *
 * 说明：
 *      初始化客户端文件描述符信息
 =================================================================
 */
void NCServer::InitClientFDSet()
{
    for(int i = 0; i < CONCURRENT_MAX; i++)
    {
        client_fds[i].C_FD = 0;
        client_fds[i].C_Name = CLIENT_NULL;
    }
}

/*================================================================
 * 函数: InitServerSock()
 *
 * 说明：
 *      初始化服务器端socket信息
 =================================================================
 */
void NCServer::InitServerSock()
{
    /*===============================================================
     * 结构体原型：
     * struct sockaddr_in {short sin_family; unsigned short sin_port;
     *                                  struct in_addr sin_addr; char sin_zero[8]; };
     * 功能：sockaddr, sockaddr_in都是用于处理网络通信地址的结构体
     *      short sin_family: 2 Bytes, 如AF_INET, AF_INET6
     *      unsigned short sin_port: 2 Bytes 如htons(3490)
     *      struct in_addr sin_addr: 4 Bytes, 地址，其实是int类型
     *      char sin_zero[8]: 如果需要的话，即为8 Bytes的0
     ================================================================
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(LAN_ADDRESS);
    bzero(&(server_addr.sin_zero), 8);
}

/*================================================================
 * 函数: CreateSocket()
 *
 * 说明：
 *      创建socket
 =================================================================
 */
void NCServer::CreateSocket()
{
    if(State == SERVER_LINKING)
    {
        string Estr;
        /*===============================================================
         * 函数原型：int socket(int domain, int type, int protocol);
         * 功能：创建socket
         *      int domain: 协议族，对于本地套接字来说，其值须被置为 AF_UNIX 枚举值
         *      int type: 套接字类型，可被设置为流式套接字SOCK_STREAM或SOCK_DGRAM（数据报式套接字）
         *      int protocol: 返回值为生成的套接字描述符，应被设置为0
         ================================================================
         */
        server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        // socket创建失败，弹出错误信息
        if(server_sock_fd == -1)
        {
            perror("Error: Server's Creating Socket ERROR.");

            Estr = "ERROR: Server's Creating Socket ERROR: ";
            Estr += strerror(errno);
            RecordNCElog(Estr);

            State = SERVER_ERROR_SOCKET;
        }
        else
        {
            Estr = "Server's Creating Socket SUCCESS. ";
            RecordNCElog(Estr);
            State = SERVER_LINKING;
        }
    }
}

/*================================================================
 * 函数: BindSocket()
 *
 * 说明：
 *      绑定socket
 =================================================================
 */
void NCServer::BindSocket()
{
    if(State == SERVER_LINKING)
    {
        string Estr;
        /*===============================================================
         * 函数原型：int bind(int socket, const struct sockaddr *address, size_t address_len);
         * 功能：绑定socket与本地地址
         *      int socket: 服务器端的套接字描述符
         *      struct sockaddr *address: 需要绑定的本地地址
         *      size_t address_len: 该本地地址的字节长度
         ================================================================
         */
        int bind_result = bind(server_sock_fd, (struct sockaddr *)&server_addr,
                               sizeof(server_addr));
        // 绑定失败，进行错误处理
        if(bind_result == -1)
        {
            perror("Error: Server's Bind ERROR.");

            Estr = "ERROR: Server's Bind ERROR: ";
            Estr += strerror(errno);
            RecordNCElog(Estr);

            close(server_sock_fd);
            close(client_sock_fd);
            State = SERVER_ERROR_BIND;
        }
        else
        {
            Estr = "Server's Bind SUCCESS. ";
            RecordNCElog(Estr);
            State = SERVER_LINKING;
        }
    }
}

/*================================================================
 * 函数: ServerListen()
 *
 * 说明：
 *      服务器监听
 =================================================================
 */
void NCServer::ServerListen()
{
    if(State == SERVER_LINKING)
    {
        string Estr;
        /*===============================================================
         * 函数原型：int listen(int socket, int backlog);
         * 功能：服务器监听
         *      int socket: 服务器端的套接字描述符
         *      int backlog: 排队连接队列的长度，如果有多个客户端同时连接，需要进行排队
         ================================================================
         */
        if(listen(server_sock_fd, BACKLOG) == -1)
        {
            perror("Error: Server's Listen ERROR.");

            Estr = "ERROR: Server's Listen ERROR: ";
            Estr += strerror(errno);
            RecordNCElog(Estr);

            close(server_sock_fd);
            close(client_sock_fd);
            State = SERVER_ERROR_LISTEN;
        }
        else
        {
            Estr = "Server's Listen SUCCESS. ";
            RecordNCElog(Estr);
            State = SERVER_LINKING;
        }
    }
}

/*================================================================
 * 函数: isServerReady()
 * 返回值：bool
 *
 * 说明：
 *      判断服务器端连接正常，是否可以进入运行状态
 =================================================================
 */
bool NCServer::isServerReady()
{
    switch(State)
    {
    case SERVER_LINKING:
        return true;
    case SERVER_ERROR_SOCKET:
        cout<<"ERROR: Server ERROR in Creating Socket."<<endl;
        return false;
    case SERVER_ERROR_BIND:
        cout<<"ERROR: Server ERROR in Binding Socket."<<endl;
        return false;
    case SERVER_ERROR_LISTEN:
        cout<<"ERROR: Server ERROR in Server Listening."<<endl;
        return false;
    case SERVER_ERROR_STAGENULL:
        cout<<"ERROR: Server ERROR in STAGE END."<<endl;
        return false;
    case SERVER_ERROR_ACCEPT:
        cout<<"ERROR: Server ERROR in ACCEPT."<<endl;
        return false;
    case SERVER_END:
        cout<<"Stage Reached to the END."<<endl;
        return false;
    default:
        return false;
        break;
    }
}

/*================================================================
 * 函数：SetTimeValue(double time)
 * 参数：double time:      超时时间，单位秒
 *
 * 说明：
 *      设置超时时间
 =================================================================
 */
void NCServer::SetTimeValue(double time)
{
    tv.tv_sec = (int)time;
    tv.tv_usec = (time - (int)time) * 1e6;
}

/*================================================================
 * 函数：InitFDSet()
 *
 * 说明：
 *      初始化文件描述符集合
 =================================================================
 */
void NCServer::InitFDSet()
{
    // FD_ZERO: 将指定的文件描述符集合清空；
    // 对文件描述符集合进行设置之前，必须对其进行初始化；由于系统分配内存空间时并不作清空处理，所以如果此处不清空，结果是不可知的
    FD_ZERO(&server_fd_set);

    // FD_SET: 在文件描述符集合中，增加一个新的文件描述符
    // 增加键盘输入描述符
    FD_SET(STDIN_FILENO, &server_fd_set);
    if(max_fd < STDIN_FILENO)
        max_fd = STDIN_FILENO;
}

/*================================================================
 * 函数：ConnectServer()
 *
 * 说明：
 *      连接服务器端
 =================================================================
 */
void NCServer::ConnectServer()
{
    // 增加键盘输入服务器文件描述符
    FD_SET(server_sock_fd, &server_fd_set);
    // printf("server_sock_fd=%d\n", server_sock_fd);
    if(max_fd < server_sock_fd)
        max_fd = server_sock_fd;
}

/*================================================================
 * 函数：ConnectClients()
 *
 * 说明：
 *      连接多个客户端
 =================================================================
 */
void NCServer::ConnectClients()
{
    for(int i = 0; i < CONCURRENT_MAX; i++)
    {
        //printf("client_fds[%d]=%d\n", i, client_fds[i]);
        if(client_fds[i].C_FD != 0)
        {
            FD_SET(client_fds[i].C_FD, &server_fd_set);
            if(max_fd < client_fds[i].C_FD)
                max_fd = client_fds[i].C_FD;
        }
    }
}

/*================================================================
 * 函数：isSelectOK()
 * 返回值：bool
 *
 * 说明：
 *      判断服务器FD集合连接是否正常；
 *      返回true，则正常操作；返回false，则输出提示信息，并跳出该函数之外的循环；
 =================================================================
 */
bool NCServer::isSelectOK()
{
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
     ================================================================
     */
    int ret;
    if(tv.tv_sec < 0 || tv.tv_usec < 0)
        ret = select(max_fd + 1, &server_fd_set, NULL, NULL, NULL);
    else
        ret = select(max_fd + 1, &server_fd_set, NULL, NULL, &tv);
    // select错误信息
    if(ret < 0)
    {
        perror("Error: Server's Select ERROR: ");
        return false;
    }
    else if(ret == 0)
    {
        cout<<"Warning: Server's Select TIMEOUT."<<endl;
        return false;
    }
    else
        return true;
}

/*================================================================
 * 函数：SendData()
 *
 * 说明：
 *      向多个客户端发送数据；
 =================================================================
 */
void NCServer::SendData()
{
    // FD_ISSET(s, &fd_set): 检测文件描述符s是否在集合fd_set中；
    // 检测键盘输入是否在文件描述符集合中
    if(FD_ISSET(STDIN_FILENO, &server_fd_set))
    {
        // bzero: 置字节字符串s的前n个字节为零, 且包括‘\0’
        bzero(input_msg, BUFFER_SIZE);
        fgets(input_msg, BUFFER_SIZE, stdin);
        cout<<"KEYBOARD INPUT SEND: "<<input_msg<<endl;

        // 记录NCServer发送的数据
        RecordNCServer(CS_SEND);

        // 输入“.quit"则退出服务器
        if(strcmp(input_msg, QUIT_CMD) == 0)
        {
            string Estr;
            Estr = "Server has INPUT Quit CMD.";
            RecordNCElog(Estr);
            exit(0);
        }

        //++++++++++++++++++++++++++++++
        // 遍历客户端，向在文件描述符集合中的文件发送数据
        // 在此处添加针对性连接客户端的代码
        //++++++++++++++++++++++++++++++
        for(int i = 0; i < CONCURRENT_MAX; i++)
        {
            if(client_fds[i].C_FD != 0)
            {
                printf("Client [%d] = %d\n", i, client_fds[i].C_FD);
                if(client_fds[i].C_Name == CLIENT_SURFACE)
                {
                    // 向界面客户端发送信息，信息为string格式
                    SendSurfMessage(string(input_msg));
                }
                else
                    send(client_fds[i].C_FD, input_msg, BUFFER_SIZE, 0);
            }
        }
    }
}
/*================================================================
 * 函数：DetectFDSet()
 *
 * 说明：
 *      检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化；
 =================================================================
 */
void NCServer::DetectFDSet()
{
    // 检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化
    if(FD_ISSET(server_sock_fd, &server_fd_set))
    {
        //有新的连接请求
        struct sockaddr_in client_address;
        socklen_t address_len;

        /*===============================================================
         * 函数原型：int accept(int socket, struct sockaddr *address, size_t *address_len);
         * 功能：接受连接请求
         * 参数信息：
         *      int socket: 服务器端套接字描述符
         *      struct sockaddr *address: 当前连接客户端的本地地址
         *      size_t *address_len: 当前连接客户端本地地址的字节长度，既是输入，也是输出参数
        ================================================================
         */
        client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_address, &address_len);

        /*===============================================================
         * 显示服务器端、客户端的连接信息
         ================================================================
         */
        cout<<"New Connection Client: Socket FD is "<<client_sock_fd<<endl;
        if(client_sock_fd > 0)
        {
            int index = -1;
            // 向服务器列表中添加客户端
            for(int i = 0; i < CONCURRENT_MAX; i++)
            {
                if(client_fds[i].C_FD == 0)
                {
                    index = i;
                    client_fds[i].C_FD = client_sock_fd;
                    break;
                }
            }

            // 当前连入服务器的客户端大于0，则输出新连入客户端的信息
            if(index >= 0)
            {
                printf("Client [%d] Add In Success.\n", index);
                printf("    New Client Address: %s\n", inet_ntoa(client_address.sin_addr));
                printf("    New Client Port: %d\n", ntohs(client_address.sin_port));

                if(ElogStrm)
                {
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"Client ["<<index<<"] Add In Success."<<endl;
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"    New Client Address: "<<inet_ntoa(client_address.sin_addr)<<endl;
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"    New Client Port: "<< ntohs(client_address.sin_port)<<endl;
                }

            }
            // 已经接入过多客户端，服务器、客户端输出错误信息，并输出连入失败的客户端的地址与端口信息
            else
            {
                bzero(input_msg, BUFFER_SIZE);
                strcpy(input_msg, "Client Error: The Number of Clients Attached In Server has been MAX, CANNOT Add In New Client.\n");
                send(client_sock_fd, input_msg, BUFFER_SIZE, 0);
                printf("Server Error: The Number of Clients Attached In Server has been MAX, CANNOT Add In New Client. \n");
                printf("    Failed Client Address: %s\n",  inet_ntoa(client_address.sin_addr));
                printf("    Failed Client Port: %d\n", ntohs(client_address.sin_port));

                if(ElogStrm)
                {
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"Server Error: The Number of Clients Attached In Server has been MAX, CANNOT Add In New Client. "<<endl;
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"    Failed Client Address: "<<inet_ntoa(client_address.sin_addr)<<endl;
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"    Failed Client Port: "<<ntohs(client_address.sin_port)<<endl;
                }
            }
        }
        else
        {
            string Estr;
            Estr = "ERROR: Server Cannot ACCEPT Client Connect Request.";
            cout<<Estr<<endl;
            RecordNCElog(Estr);

            close(client_sock_fd);
            State = SERVER_ERROR_ACCEPT;
        }
    }
}

/*================================================================
 * 函数：ClientDataProcess()
 *
 * 说明：
 *      遍历所有客户端，处理对应客户端过来的消息；
 *      在此处添加针对性连接客户端的代码；
 =================================================================
 */
void NCServer::ClientDataProcess()
{
    // 遍历所有客户端
    for(int i = 0; i < CONCURRENT_MAX; i++)
    {
        // 检测该客户端的fd是否在fd集合中，且是否有变化，有变化则执行
        if(FD_ISSET(client_fds[i].C_FD, &server_fd_set))
        {
            bzero(recv_msg, BUFFER_SIZE);
            /*===============================================================
             * 函数原型：int recv( SOCKET s, char *buf, int  len, int flags)
             * 功能：不论是客户还是服务器应用程序都用recv函数从TCP连接的另一端接收数据。
             * 函数参数：
             *      SOCKET s: 指定接收端套接字描述符
             *      char *buf: 指明一个缓冲区，该缓冲区用来存放recv函数收到的数据
             *      int len: 指明buf的长度
             *      int flags: 一般置0
             ================================================================
             */
            // byte_num是实际收到缓冲区字符的数量，接受错误则返回SOCKET_ERROR
            long byte_num = recv(client_fds[i].C_FD, recv_msg, BUFFER_SIZE, 0);


            // 从客户端收到消息
            if (byte_num > 0)
            {
                if(byte_num > BUFFER_SIZE)
                    byte_num = BUFFER_SIZE;

                RecvCmd = (string)recv_msg;
                // 移除空格
                RecvCmd = rmSpace(RecvCmd);
                // 如果接收到<NCName></NCName>格式的数据，则将中间数据取出，并作为该client_fd的名称标示C_Name;
                setClientName(RecvCmd, client_fds[i]);

                switch(client_fds[i].C_Name)
                {
                case CLIENT_NULL:
                    break;
                case CLIENT_IMAGE:
                    // 识别为图像客户端传来的数据，对其进行处理
                    ImageClientDataProcess();
                    break;
                case CLIENT_SEND:
                    // 识别为下位机客户端传来的数据，对其进行处理
                    SendClientDataProcess();
                    break;
                case CLIENT_SURFACE:
                    break;
                default:
                    string Estr = "ERROR: No This Client FD Name.";
                    cout<<Estr<<endl;
                    RecordNCElog(Estr);
                    break;
                }

                cout<<"Client "<<getClientName(client_fds[i])<<" Message:"<<endl<<"    ";
                cout<<recv_msg<<endl;
                // 记录NCServer收到的数据
                RecordNCServer(CS_RECV);
            }
//            // 从客户端接收消息出错
//            else if(byte_num < 0)
//                printf("Error: Client [%d] Receives Messages From Server ERROR.\n", i);
            // 收到客户端退出消息
            else
            {
                FD_CLR(client_fds[i].C_FD, &server_fd_set);
                client_fds[i].C_FD = 0;
                printf("Warning: Client [%d] EXITED.\n", i);
                if(ElogStrm)
                {
                    ElogStrm<<getSysTime(TIME_NOYMD_WITHMS)<<'\t';
                    ElogStrm<<"Warning: Client ["<<i<<"] EXITED. "<<endl;
                }
            }
        }
    }
}

/*================================================================
 * 函数：setClientName(string PackageName, ClientInfo &info)
 *
 * 说明：
 *      设置客户端名称；
 *      使用类似于HTML语言的方式，为客户端命名；
 =================================================================
 */
void NCServer::setClientName(string PackageName, ClientInfo &info)
{
    if(PackageName.find("<NCName>") != -1
            && PackageName.find("</NCName>") != -1)
    {
        string ClientName;
        // 若字符串两端包含<NCName></NCName>，则提取出字符串中间的子串作为客户端的名称
        ClientName = PackageName.substr(PackageName.find_first_not_of("<NCName>"),
                                        PackageName.length() - (sizeof("<NCName>") + sizeof("</NCName>") ) / sizeof(char) + 2);

        if(ClientName == NAME_CLIENT_IMAGE)
            info.C_Name = CLIENT_IMAGE;
        else if(ClientName == NAME_CLIENT_SEND)
            info.C_Name = CLIENT_SEND;
        else if(ClientName == NAME_CLIENT_SURF)
            info.C_Name = CLIENT_SURFACE;
    }
}

/*================================================================
 * 函数：getClientName(ClientInfo info)
 * 返回值：string
 * 参数：ClientInfo info
 *
 * 说明：
 *      获取当前客户端的名称，根据输入的ClientInfo，返回对应的字符串；
 =================================================================
 */
string NCServer::getClientName(ClientInfo info)
{
    string dst;
    switch(info.C_Name)
    {
    case CLIENT_IMAGE:
        dst = NAME_CLIENT_IMAGE;
        break;
    case CLIENT_SEND:
        dst = NAME_CLIENT_SEND;
        break;
    case CLIENT_SURFACE:
        dst = NAME_CLIENT_SURF;
        break;
    default:
        dst = NAME_CLIENT_BASE;
        cout<<"ERROR: Get Client Name WRONG: No This Client Name."<<endl;
        break;
    }
    return dst;
}

/*================================================================
 * 函数：Run()
 *
 * 说明：
 *      服务器功能运行；
 *      1. 初始化CurStage，并为其设置初始Mission（此处设置为TEST任务）
 *      2. 判断当前State，如果不为SERVER_LINKING，则跳出循环；
 *          (1) 设置超时时间；
 *          (2) 初始化FD集合；
 *          (3) 连接服务器端；
 *          (4) 连接多个客户端；
 *          (5) 判断服务器FD集合连接是否正常，如果返回false，则continue该循环；
 *          (6) 向多个客户端发送数据；
 *          (7) 检测服务器套接字FD是否在FD集合中；
 *          (8) 遍历所有客户端，针对性的处理对应客户端传来的消息；
 *          (9) 策略执行（执行策略的过程中，依旧执行上述八项指令以获取并更新数据）；
 =================================================================
 */
void NCServer::Run()
{
    //================================================
    // 1. 将当前Stage初始化为Stage_START，并根据不同任务，设置目标Stage
    //================================================
    SetStartStage(CurStage, TEST);
    // 实现每经过三个Stage便切换任务，仅在测试阶段改变Mission用
    Mission_i = 0;
    tmpMission = TEST;

    while(isServerReady())
    {
        // 设置超时时间
        SetTimeValue(SERVER_UPTIME);

        // 初始化文件描述符集合
        InitFDSet();

        // 连接服务器端
        ConnectServer();

        // 连接多个客户端
        ConnectClients();

        // 判断服务器FD集合连接是否正常
        if(!isSelectOK())   continue;

        // 向多个客户端发送数据
        SendData();

        // 检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化
        DetectFDSet();

        // 遍历所有客户端，处理对应客户端过来的消息
        // 在此处添加针对性连接客户端的代码
        ClientDataProcess();

        // 策略执行
        Strategy();
    }
}

