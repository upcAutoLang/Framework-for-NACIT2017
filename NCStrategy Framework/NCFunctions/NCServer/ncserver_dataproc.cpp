#include "ncserver.h"

/*================================================================
 * 函数：ImageClientDataProcess()
 *
 * 说明：
 *      识别为图像客户端传来的数据，对其进行处理；
 =================================================================
 */
void NCServer::ImageClientDataProcess()
{
//    cout<<"Image Client Process Result:"<<endl<<"    ";
//    string tmp = recv_msg;
//    cout<<tmp.substr(0, 4)<<endl;

    SendSurfMessage(string(recv_msg));
}

/*================================================================
 * 函数：SendClientDataProcess()
 *
 * 说明：
 *      识别为下位机客户端传来的数据，对其进行处理；
 =================================================================
 */
void NCServer::SendClientDataProcess()
{
//    cout<<"Send Client Process Result:"<<endl<<"    ";
//    string tmp = recv_msg;
//    cout<<tmp.substr(0, 6)<<endl;
    SendSurfMessage(string(recv_msg));
}

// 向界面客户端发送信息，信息为string格式
void NCServer::SendSurfMessage(string str)
{
    // 遍历所有客户端
    for(int i = 0; i < CONCURRENT_MAX; i++)
    {
        if(client_fds[i].C_Name == CLIENT_SURFACE)
        {
            // 将需要传递的string字符串转换成char[]类型
            int j = 0;
            // bzero: 置字节字符串s的前n个字节为零, 且包括‘\0’
            bzero(input_msg, BUFFER_SIZE);
            string tmpstr = NAME_CLIENT_SURF;
            tmpstr += "@";
            tmpstr += str;
            for(j = 0; j < tmpstr.length(); j++)
                input_msg[j] = tmpstr[j];
            input_msg[j] = '\0';

            send(client_fds[i].C_FD, input_msg, BUFFER_SIZE, 0);
        }
    }
}

//
void NCServer::SendStages2Surf()
{
    string str;
    // 发送Stage信息给界面客户端
    str = "stage:";
    str += typeid(*CurStage).name();
    SendSurfMessage(str);

    // 发送Mission信息给界面客户端
    str = "mission:";
    str += getMission();
    SendSurfMessage(str);
}
