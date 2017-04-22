#ifndef NCCAMERA_H
#define NCCAMERA_H

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>

#include "cv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <FGCamera.h>

using namespace std;
using namespace cv;

class NCCamera
{
public:
    NCCamera();
    NCCamera(int CamNum, int wid = 780, int heig = 580);
    ~NCCamera();

    // 设置摄像头GUID
    void setCameraGUID(int CamNum);
    // 摄像头初始化
    void InitCamera(int CamNum);
    // 摄像头参数初始化
    void InitCameraParam();
    // 开始图像采集；
    void StartGrab();
    // 采集图像，将数据读入Mat格式的Frame中；
    void CatchOneFrame();
    // 从USB摄像头采集图像，将数据读入Mat格式的Frame中；
    void CatchOneFrameFromUSB();
    // 结束图像采集；
    void ReleaseCamera();
    // 获取Mat数据
    void getFrame(Mat &dst);

    // 调试用程序，从视频中采集图像，将数据读入Mat格式的Frame中；
    void CatchOneFrameFromVideo();
    // 对视频赋文件地址
    void setVideoCapture(string path);

private:
    CFGCamera Camera;
    UINT32 Result;
    FGNODEINFO NodeInfo[3];
    UINT32 NodeCnt;
    FGFRAME FGFrame;
    // 调试使用的视频变量
    VideoCapture cp;
    // 调试使用的实时采集USB摄像头变量
    VideoCapture video;

    // 当前设备的GUID
    UINT32HL CurGuid;
    // OpenCV图像
    Mat Frame;

    // 图像高度、宽度
    UINT32 width, height;
};

#endif // NCCAMERA_H
