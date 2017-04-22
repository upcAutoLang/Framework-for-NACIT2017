#ifndef NCCLIENT_H
#define NCCLIENT_H

#include <string_fun.h>

#include <ncclient_macros.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>

using namespace std;

class NCClient
{
public:
    NCClient();
    ~NCClient();

    // 设置服务器地址
    void SetServerAddress();
    // 创建socket
    void CreateSocket();
    // 连接完成后，则开始完成客户端工作
    void ClientFunction();
    // 设置超时时间
    void SetTimeVal(double time);
    // 客户端选择FD集，可读可写文件，设置超时时间
    bool ClientSelect(double time);
    // 客户端处理部分
    virtual void ClientProcessing();
    // 服务器客户端connect成功后，对该客户端进行数据初始化工作
    virtual void ClientInit();
    // FD集合初始化与设置
    void SetClientFDSet(Client_Mode mode = CLIENT_OUTPUT);
    // 测试部分，向客户端传输数据
    void CommunicateTest();
    // 向服务器传输数据
    virtual void SendToServer();
    // 从服务器接受数据
    virtual void ReceiveFromServer();
    // 将服务器传来的信息切分：客户端名称 + 对应指令
    void CutServerCmd();
    // 该客户端匹配到来自于服务器传来的信息后，处理服务器传来的指令
    virtual void CmdProcess();
    // 客户端数据更新（例如图像的waitkey，每次发送数据之间的时间间隔）
    virtual void ClientDataUpdate();
    // 将Client名称包装
    string PackageClientName();
    // 客户端名称初始化，并发送给服务器
    void InitClientName2Server();

protected:
    // 接收到的信息
    char recv_msg[BUFFER_SIZE];
    // 要发送的信息
    char input_msg[BUFFER_SIZE];

    // 客户端名称
    string Name;
    // 从服务器接收到的全部指令
    string RecvTotalCmd;
    // 分割得到的客户端名称
    string RecvName;
    // 分割得到的指令
    string RecvCmd;
    // 客户端要连接的服务器地址信息
    struct sockaddr_in ServerAddress;
    // 服务器Socket的FD（文件描述符）
    int ServerSocketFD;
    // 客户端的连接状况
    Client_State ClientState;
    // fd_set: 文件描述字（fd）的集合，它用1bit表示一个fd；
    // 可以将文件描述字当做一个文件的路径保存的地方；
    fd_set ClientFDSet;
    // 超时时间设置
    struct timeval tv;
    double overtime;
    // 检测任务改变的变量
    bool isMissionChanged;
    // 已经向服务器端设置了该客户端的名称
    bool isClientNameSet = false;
    /*================================
     *   以下变量仅供构建框架使用，仅供测试
     =================================
     */
    // 客户端测试过程所使用的数据
    int send_i;
};

#endif // NCCLIENT_H
