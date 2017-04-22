#ifndef NCSERVER_MACROS_H
#define NCSERVER_MACROS_H

#include "ncclient_macros.h"

// 完成三次握手但没有accept的队列的长度
#define BACKLOG         5
// 应用层同时可以处理的连接
#define CONCURRENT_MAX      4
// 退出命令
#define QUIT_CMD    ".quit\n"
// 服务器select的超时时间值，单位秒，此处先设定为2s
#define SERVER_UPTIME   -1
// 记录操作的开关
#define RECORD  true

// 定义客户端结构体
struct ClientInfo
{
    int C_FD;
    Client_NUM C_Name;
};

// 枚举服务器状态信息
enum Server_State
{
    SERVER_LINKING,
    SERVER_ERROR_SOCKET,
    SERVER_ERROR_BIND,
    SERVER_ERROR_LISTEN,
    SERVER_ERROR_STAGENULL,
    SERVER_ERROR_ACCEPT,
    SERVER_END
};

#endif // NCSERVER_MACROS_H
