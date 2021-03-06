#ifndef NCSERVER_H
#define NCSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <strstream>
#include <algorithm>

#include "ncclient_macros.h"
#include "ncserver_macros.h"
#include "ncstage.h"
#include "ncstage_basic.h"
#include "ncstage_test.h"

#include "string_fun.h"
#include "sys_fun.h"

using namespace std;

class NCServer
{
public:
    NCServer();
    ~NCServer();

    // 服务器功能运行
    void Run();
    // 判断服务器端连接正常，是否可以进入运行状态
    bool isServerReady();
    // 设置超时时间
    void SetTimeValue(double time);
    // 设置客户端名称；
    void setClientName(string PackageName, ClientInfo &info);
    // 获取当前客户端的名称，根据输入的ClientInfo，返回对应的字符串；
    string getClientName(ClientInfo info);

    //============================
    //    网络连接相关
    //============================
    // 初始化服务器端socket信息
    void InitServerSock();
    // 初始化客户端文件描述符信息
    void InitClientFDSet();
    // 创建socket
    void CreateSocket();
    // 绑定socket
    void BindSocket();
    // 服务器监听
    void ServerListen();
    // 初始化文件描述符集合
    void InitFDSet();
    // 连接服务器端
    void ConnectServer();
    // 连接多个客户端
    void ConnectClients();
    // 判断服务器FD集合连接是否正常
    bool isSelectOK();
    // 向多个客户端发送数据
    void SendData();
    // 检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化
    void DetectFDSet();

    //============================
    //    信息处理相关
    //============================
    // 遍历所有客户端，处理对应客户端过来的消息
    // 在此处添加针对性连接客户端的代码
    void ClientDataProcess();
    // 识别为图像客户端传来的数据，对其进行处理
    void ImageClientDataProcess();
    // 识别为下位机客户端传来的数据，对其进行处理
    void SendClientDataProcess();

    void SendSurfMessage(string str);

    void SendStages2Surf();

    //============================
    //    策略相关
    //============================
    // 服务器功能运行；
    void Strategy();

    //============================
    //    NCStage相关
    //============================
    // 根据CurStage的目标Stage，改变CurStage的数据类型
    void ChangeStage(NCStage* &stage, Mission mission);
    // 将当前Stage初始化为NCStage_START，并根据输入的Mission设置目标Stage
    void SetStartStage(NCStage* &stage, Mission mission);

    string getMission();

    //============================
    //    文件记录相关
    //============================

    // 创建记录文件夹；
    void CreateRecordFolders();
    // 在文件夹中生成一系列记录文件；
    void CreateRecordFiles();
    // NCServer.dat, NCImage.dat, NCSend, NCInfo, NCStage记录函数
    void RecordNCServer(WORK_STATE state);
    void RecordNCImage(WORK_STATE state);
    void RecordNCSend(WORK_STATE state);
    void RecordNCInfo();
    void RecordNCStage();
    void RecordNCElog(string EStr);


protected:
    // 信息输入缓冲区
    char input_msg[BUFFER_SIZE];
    // 信息接受缓冲区
    char recv_msg[BUFFER_SIZE];
    // 服务器连接状态
    Server_State State;
    // 定义一定数量的客户端
    ClientInfo client_fds[CONCURRENT_MAX];
    // 服务器端的socket地址
    struct sockaddr_in server_addr;
    // 服务器端socket文件描述符
    int server_sock_fd;

    int client_sock_fd;
    // fd_set: 文件描述字（fd）的集合，它用1bit表示一个fd；
    // 可以将文件描述字当做一个文件的路径保存的地方；
    fd_set server_fd_set;
    // 集合中所有文件描述符的范围，所有文件描述符的最大值+1，不能错误！！
    int max_fd = -1;
    // 超时时间设置
    struct timeval tv;
    // 从其他客户端收到的消息
    string RecvCmd;

    //============================
    //    NCStage相关
    //============================
    NCStage* CurStage;
    int Mission_i;
    Mission tmpMission;

    //============================
    //    策略相关
    //============================

    //============================
    //    文件记录相关
    //============================

    string FolderPath;

    ofstream ImageStrm;
    ofstream ServerStrm;
    ofstream SendStrm;
    ofstream InfoStrm;
    ofstream StageStrm;
    ofstream ElogStrm;
};

#endif // NCSERVER_H
