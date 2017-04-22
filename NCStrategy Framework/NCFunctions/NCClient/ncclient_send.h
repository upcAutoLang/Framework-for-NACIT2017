#ifndef NCCLIENT_SEND_H
#define NCCLIENT_SEND_H

#include <ncclient.h>

class NCClient_SEND : public NCClient
{
public:
    NCClient_SEND();
    ~NCClient_SEND();

    // 服务器客户端connect成功后，对该客户端进行数据初始化工作
    virtual void ClientInit();
    // SEND客户端处理部分
    virtual void ClientProcessing();
    // SEND客户端与服务器的通讯
    virtual void ReceiveFromServer();
    // 客户端数据更新（例如图像的waitkey，每次发送数据之间的时间间隔）
    virtual void ClientDataUpdate();
    // 该客户端匹配到来自于服务器传来的信息后，处理服务器传来的指令
    virtual void CmdProcess();
    // 向服务器传输数据
    virtual void SendToServer();

protected:


};

#endif // NCCLIENT_SEND_H
