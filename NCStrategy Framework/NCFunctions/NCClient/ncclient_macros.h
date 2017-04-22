#ifndef NCCLIENT_MACROS_H
#define NCCLIENT_MACROS_H

// 局域网地址信息，由于本地间通信，故此处用回归地址127.0.0.1
#define LAN_ADDRESS "127.0.0.1"

// 发送、接受信息的缓冲区大小
#define BUFFER_SIZE 1024
// 服务器端口号
#define SERVER_PORT 11332
// 设置客户端超时时间
// 若为负值，则select函数使用阻塞操作
#define CLIENT_UPTIME   -1

#define NAME_CLIENT_BASE      "BASE"
#define NAME_CLIENT_IMAGE   "IMAGE"
#define NAME_CLIENT_SEND     "SEND"
#define NAME_CLIENT_SURF     "SURFACE"

/*===============================================================
 * 枚举：WORK_STATE
 *
 * 说明：客户端/服务器工作状态
 *      1. CS_RECV:  C/S接收状态
 *      2. CS_SEND: C/S发送状态
 ================================================================
 */
enum WORK_STATE
{
    CS_RECV,
    CS_SEND
};

/*===============================================================
 * 枚举：Client_NUM
 *
 * 说明：客户端名称
 *      目前测试使用，设置了两个客户端：
 *      1. CLIENT_IMAGE: 图像客户端
 *      2. CLIENT_SEND: 传输数据客户端（模拟下位机）
 ================================================================
 */
enum Client_NUM
{
    CLIENT_NULL,
    CLIENT_IMAGE,
    CLIENT_SEND,
    CLIENT_SURFACE
};

/*===============================================================
 * 枚举：Client_State
 *
 * 说明：枚举客户端状态信息
 *      用于存储客户端连接状态信息，设置该值主要是为了检测客户端与服务
 * 器之间的连接状态，调试时便于发现哪里出现了错误；
 *      目前测试阶段，只设定了三种状态，以后可继续添加：
 *      1. CLIENT_LINKING: 客户端正常连接
 *      2. CLIENT_UNLINK_SOCKET: 客户端在创建SOCKET时出现错误，连接失败
 *      3. CLIENT_UNLINK_NOSERVER: 服务器断开，导致客户端连接断开
 ================================================================
 */
enum Client_State
{
    CLIENT_LINKING,
    CLIENT_UNLINK_SOCKET,
    CLIENT_UNLINK_NOSERVER
};

/*===============================================================
 * 枚举：Client_Mode
 *
 * 说明：枚举客户端工作模式
 *      客户端的工作模式一般也就是两种状态：INPUT与OUTPUT，以后改动空间不大
 *      1. INPUT:  标准文件输入描述字方式工作（基本可以理解为键盘输入），准备用于服务器；
 *      2. OUTPUT: 标准文件输出描述字方式工作，准备用于各个客户端；
 ================================================================
 */
enum Client_Mode
{
    CLIENT_INPUT,
    CLIENT_OUTPUT
};

#endif // NCCLIENT_MACROS_H
