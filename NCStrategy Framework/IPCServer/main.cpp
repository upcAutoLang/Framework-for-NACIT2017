/*===============================================================
 * Version:
 *  v1.00: 实现连接多个客户端的服务器端
 *  v1.01:
 *      1. 定义了客户端结构体ClientInfo，该结构体包括客户端的FD，以及客户端的名称（由枚举变量定义）
 *      2. 定义了枚举变量Client_NUM，用于区别不同客户端之间的作用
 *      3. 由ImageClient复制出新的客户端，并命名为SendClient，往后ImageClient将测试图像处理进程，SendClient将测试串口进程
 *  v1.02:
 *      在服务器广播的基础上，实现对客户端的对点传播
 *  v1.03:
 *      ImageClient实现了图像的简单处理，以及简单的任务切换，并同时持续发送信息；
 *      添加了一些自定义字符串处理函数；
 *  v1.04:
 *      将部分代码归类并整理；
 *  v1.06:
 *      将v1.04版本的Server代码封装成了NCServer类；
 *  v1.07--1.08:
 *      出现连接不稳定的bug，弃置
 *  v1.09:
 *      为连接NCServer的客户端进行了名称赋值，并进一步封装NCServer类；
 * v1.10:
 *      在NCServer中添加了NCStage_v1.07的功能并实现；
 *      将所有函数添加了注释；
 *  v1.11:
 *      整理了NCServer的函数，将其分类至几个不同的cpp文件中；
 *  v1.12:
 *      添加了记录函数，将不同数据添加进NCServer.dat, NCImage.dat, NCInfo.dat, NCSend.dat, NCStage.dat文件中；
 *  v1.13:
 *      添加了NCElog.dat的记录，该文件记录连接状态以及错误报告；
 *  v1.14:
 *      添加了初始化客户端文件描述符信息函数InitClientFDSet();
 *  v1.15:
 *      添加了专门与界面通讯的客户端NCClient_SURFACE；
 ================================================================
 */

#include "ncserver.h"
#include "ncclient_macros.h"
#include "ncserver_macros.h"

/*===============================================================
 * 发送协议：
 *      客户端名称@指令
 * 例：给IMAGE客户端发送指令，则在服务器输入:
 *      IMAGE@.....
 ================================================================
 */

int main(int argc, char *argv[])
{
    NCServer Server;

    // 创建socket
    Server.CreateSocket();

    // 绑定socket
    Server.BindSocket();

    // 服务器监听
    Server.ServerListen();

    // 服务器功能运行
    Server.Run();

    return 0;
}
















