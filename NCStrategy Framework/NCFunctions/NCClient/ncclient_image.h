#ifndef NCCLIENT_IMAGE_H
#define NCCLIENT_IMAGE_H

#include <ncclient.h>
#include <ncclient_imagemacros.h>

#include <imageprocessheader.h>
#include <imageprocessing_fun.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class NCClient_IMAGE : public NCClient
{
public:
    NCClient_IMAGE();
    ~NCClient_IMAGE();

    // 服务器客户端connect成功后，对该客户端进行数据初始化工作
    virtual void ClientInit();
    // IMAGE客户端处理部分
    virtual void ClientProcessing();
    // IMAGE客户端与服务器的通讯
    virtual void ReceiveFromServer();
    // 处理服务器传来的指令
    virtual void CmdProcess();
    // 客户端数据更新（例如图像的waitkey，每次发送数据之间的时间间隔）
    virtual void ClientDataUpdate();
    // 向服务器传输数据
    virtual void SendToServer();
    /*================================
     * 以下该处模拟一系列图像处理函数
     * 注：1. 此处仅使用几个简单的图像处理函数做调试与示范，以后希望还
     *   是换成类似于上一代NCVision已经封装好的对象，调用类的成员函数那种模式
     *      2. 函数前缀qs，意为琦少……仅仅恶趣味而已，请以后更正
     =================================
     */
    void qsImg_NO();
    void qsImg_Gray();
    void qsImg_Bin();

protected:
    // 当前图像任务，取值对应于Image_Mission枚举变量
    Image_Mission Img_Mission;

    /*================================
     *   以下变量仅供构建框架使用，仅供测试
     =================================
     */
    Mat frame;
    Mat imgBin, imgGray;
    string VideoPath01;
    VideoCapture video;
    HSVInfo ColorInfo;
};

#endif // NCCLIENT_IMAGE_H
