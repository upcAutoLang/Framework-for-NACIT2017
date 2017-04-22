/*===============================================================
 * Version:
 *  v1.00: 实现连接多个客户端的服务器端
 *  v1.01:
 *      1. 定义了客户端结构体NCClient，该结构体包括客户端的FD，以及客户端的名称（由枚举变量定义）
 *      2. 定义了枚举变量Client_NUM，用于区别不同客户端之间的作用
 *      3. 由ImageClient复制出新的客户端，并命名为SendClient，往后ImageClient将测试图像处理进程，SendClient将测试串口进程
 *  v1.02:
 *      在服务器广播的基础上，实现对客户端的对点传播
 *  v1.03:
 *      ImageClient实现了图像的简单处理，以及简单的任务切换，并同时持续发送信息；
 *      添加了一些自定义字符串处理函数；
 *  v1.04:
 *      将部分代码归类并整理；
 *  v1.05:
 *      将NCClient及其派生IMAGE, SEND客户端封装成类，并成功运行；
 ================================================================
 */

#include <ncclient_image.h>

int main(int argc, const char *argv[])
{
    NCClient_IMAGE Client_IMAGE;

    // 设置服务器地址
    Client_IMAGE.SetServerAddress();
    // 创建socket
    Client_IMAGE.CreateSocket();
    // 连接完成后，则开始完成客户端工作
    Client_IMAGE.ClientFunction();

    return 0;
}
