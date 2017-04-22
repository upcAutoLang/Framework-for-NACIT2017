/*===============================================================
 * Version:
 *  v1.01: 由ImageClient复制出新的客户端，并命名为SendClient，往后ImageClient将测试图像处理进程，SendClient将测试串口进程
 *  v1.03:
 *      隔壁的ImageClient实现了图像的简单处理，以及简单的任务切换，并同时持续发送信息；
 *      添加了一些自定义字符串处理函数；
 *  v1.04:
 *      将部分代码归类并整理；
 *  v1.05:
 *      将NCClient及其派生IMAGE, SEND客户端封装成类，并成功运行；
 ================================================================
 */

#include <ncclient_send.h>

int main(int argc, const char * argv[])
{
    NCClient_SEND Client_SEND;

    // 设置服务器地址
    Client_SEND.SetServerAddress();
    // 创建socket
    Client_SEND.CreateSocket();
    // 连接完成后，则开始完成客户端工作
    Client_SEND.ClientFunction();

    return 0;

}
