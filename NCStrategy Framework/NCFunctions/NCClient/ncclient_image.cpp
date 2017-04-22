#include <ncclient_image.h>

/*===============================================================
 * 构造函数：NCClient_IMAGE()
 *
 * 说明：
 *      设置服务器地址，初始化客户端的各参数
 ================================================================
 */
NCClient_IMAGE::NCClient_IMAGE()
{
    // 客户端名称
    Name = NAME_CLIENT_IMAGE;
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
 * 析构函数：~NCClient_IMAGE()
 *
 * 说明：
 *      退出客户端，断开与服务器的连接
 ================================================================
 */
NCClient_IMAGE::~NCClient_IMAGE()
{
    close(ServerSocketFD);
}

/*===============================================================
 * 函数：ClientInit()
 *
 * 说明：服务器客户端connect成功后，对该客户端进行数据初始化工作；
 *
 * 注：
 *      派生类NCClient_IMAGE的初始化函数，初始化了一系列图像客户端的数据；
 ================================================================
 */
void NCClient_IMAGE::ClientInit()
{
    Img_Mission = IMG_NO;

    VideoPath01 = "/home/grq/style.mp4";

    video = VideoCapture(VideoPath01);
    if(!video.isOpened())
        cout<<"Video Opened FAILED."<<endl;
    else
        cout<<"Video Opened SUCCESS."<<endl;

    ColorInfo = {40, 80, 96, 187, 30, 200};
    video >> frame;
    if(frame.empty())
    {
        cout<<"Frame is Empty."<<endl;
    }
    else
    {
        cout<<"Frame is NOT Empty."<<endl;
        imshow(IMG_WIN_NO, frame);
    }
}

/*===============================================================
 * 函数：ClientProcessing()
 *
 * 说明：客户端处理数据部分
 *
 * 注：
 *      客户端按照不同的工作任务进行处理；
 *      此处做测试，安置了三个不同的任务；
 ================================================================
 */
void NCClient_IMAGE::ClientProcessing()
{
    video >> frame;
    if(frame.empty())
        cout<<Name<<" Client : Frame is Empty."<<endl;
    else
    {
        // 不同图像任务的处理
        switch(Img_Mission)
        {
        case IMG_NO:
            qsImg_NO();
            break;
        case IMG_GRAY:
            qsImg_Gray();
            break;
        case IMG_BIN:
            qsImg_Bin();
            break;
        default:
            cout<<"ERROR : "<<Name<<" Client: IMG_PROCESS: NO THIS Img_Mission!"<<endl;
       }

        // 不同图像任务的显示
        if(isMissionChanged)
        {
            switch(Img_Mission)
            {
            case IMG_NO:
                destroyWindow(IMG_WIN_GRAY);
                destroyWindow(IMG_WIN_BIN);
                break;
            case IMG_GRAY:
                destroyWindow(IMG_WIN_NO);
                destroyWindow(IMG_WIN_BIN);
                break;
            case IMG_BIN:
                destroyWindow(IMG_WIN_NO);
                destroyWindow(IMG_WIN_GRAY);
                break;
            default:
                cout<<"ERROR : "<<Name<<" Client: IMG_SHOW_CHANGE: NO THIS Img_Mission!"<<endl;
                break;
            }
            // 重置isMissionChange任务更改变量
            isMissionChanged = false;
        }

        switch(Img_Mission)
        {
        case IMG_NO:
            imshow(IMG_WIN_NO, frame); break;
        case IMG_GRAY:
            imshow(IMG_WIN_GRAY, imgGray);  break;
        case IMG_BIN:
            imshow(IMG_WIN_BIN, imgBin); break;
        default:
            cout<<"ERROR : "<<Name<<" Client: IMG_SHOW: NO THIS Img_Mission!"<<endl;
            imshow(IMG_WIN_NO, frame); break;
        }
    }
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
void NCClient_IMAGE::ReceiveFromServer()
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
            cout<<"    ERROR errno: "<<errno<<endl;
            cout<<"    ERROR CODE: "<<strerror(errno)<<endl;
            close(ServerSocketFD);
            exit(0);
        }
    }
}

/*===============================================================
 * 函数：CmdProcess()
 *
 * 说明：客户端按照本身的处理方法，对服务器传来的指令进行反应
 *      此处根据服务器传来的指令，判断出确实指向图像客户端后，进行判断后面
 * 的具体指令；
 *      如果与图像客户端设定的指令匹配成功，则改变图像客户端的当前任务；
 ================================================================
 */
void NCClient_IMAGE::CmdProcess()
{
    // 如果判断服务器传来的指令是针对该客户端，则该客户端进行处理
    if(RecvName == "IMAGE")
    {
        cout<<"Server Totol Command:"<<endl<<"    "<<RecvTotalCmd<<endl;
        cout<<"    IMAGE COMMAND:"<<RecvCmd<<endl;

        Image_Mission CurMission = Img_Mission;

        if(RecvCmd == IMG_CMD_NO)
        {
            cout<<"    Raw Frame."<<endl;
            Img_Mission = IMG_NO;
        }
        else if(RecvCmd == IMG_CMD_GRAY)
        {
            cout<<"    BGR2GRAY Done."<<endl;
            Img_Mission = IMG_GRAY;
        }
        else if(RecvCmd == IMG_CMD_BIN)
        {
            cout<<"    BGR2Binary Done."<<endl;
            Img_Mission = IMG_BIN;
        }
        else
        {
            cout<<"    "<<Name<<" Client: UNKNOWN CMD."<<endl;
        }

        // 检测当前图像任务是否改变
        if(CurMission != Img_Mission)   isMissionChanged = true;
    }
}


/*===============================================================
 * 函数：ClientDataUpdate()
 *
 * 说明：客户端数据更新；
 *      IMAGE客户端中的waitkey，可实现图像的显示，详细原理见opencv使用说明；
 ================================================================
 */
void NCClient_IMAGE::ClientDataUpdate()
{
    waitKey(50);
}

/*===============================================================
 * 函数：SendToServer()
 *
 * 说明：向服务器传输数据
 *      仅仅为IMAGE客户端作测试用，静待日后更改；
 ================================================================
 */
void NCClient_IMAGE::SendToServer()
{
    if(isClientNameSet)
        CommunicateTest();
    // 该部分只执行一次
    else
    {
        InitClientName2Server();
        isClientNameSet = true;
    }
}

// 琦少恶趣味系列之图像简单处理函数
void NCClient_IMAGE::qsImg_NO()
{
    ;
}
void NCClient_IMAGE::qsImg_Gray()
{
    cvtColor(frame, imgGray, CV_BGR2GRAY);
}
void NCClient_IMAGE::qsImg_Bin()
{
    HSVThresh(frame, imgBin, ColorInfo);
}
