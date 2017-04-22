#include "ncclient_surface.h"

int main(int argc, const char * argv[])
{
    NCClient_SURFACE Client_SURFACE;

    // 设置服务器地址
    Client_SURFACE.SetServerAddress();
    // 创建socket
    Client_SURFACE.CreateSocket();
    // 连接完成后，则开始完成客户端工作
    Client_SURFACE.SurfClientFunction();

    return 0;

}
