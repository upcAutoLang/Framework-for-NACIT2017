#ifndef NCVISION_H
#define NCVISION_H

#include <nccamera.h>
#include <ncmodel.h>
#include <ncdownmodel.h>
#include <ncfwdmodel.h>
#include <ncimgpro.h>
#include <ncheader.h>

#include <GeneralImageProcess/draw_fun.h>

class NCVision
{
public:
    NCVision();
    //**********************************
    //          各种初始化
    //**********************************

    // Vision类初始化
    void InitVision();
    // （调试，实时采集）Vision类初始化
    void InitVisionDebug();
    // 前视、俯视摄像头参数初始化；
    void InitCamerasParam();
    // （调试，实时采集）前视、俯视摄像头参数初始化；
    void InitCamerasParamDebug();
    // VInfo初始化；
    void InitVisionInfo();
    // 设置俯视、前视视频地址（调试用）；
    void setGrabFramesVideosPaths();
    // 俯视、前视摄像头开始捕获视频
    void StartCamerasGrab();
    // （调试，实时采集）俯视、前视摄像头开始捕获视频
    void StartCamerasGrabDebug();

    //**********************************
    //          图像捕获
    //**********************************

    // 实时获取俯视、前视原图像
    void GrabFrames();
    // （调试，实时采集）获取俯视、前视原图像
    void GrabFramesDebug();
    // 视频中获取俯视、前视原图像；
    void GrabFramesFromVideos();


    //**********************************
    //        数据传递函数
    //**********************************

    // 将俯视、前视原图像传给ImgPro
    void sendFrames();
    // 将俯视原图像传给ImgPro
    void sendDownFrame();
    // 将前视原图像传给ImgPro
    void sendFwdFrame();


    //**********************************
    //        set, get函数
    //**********************************

    // 设置传感器工作深度值（不同的任务有不同的深度值）；
    void setSensorHeight(double src);
    // 获取图像坐标系中导引路径应有的宽度
    void getLineWidthInCam(double &dst);
    // 获取图像坐标系中导引路径应有的长度
    void getLineLengthInCam(double &dst);


    // 获取俯视摄像头原图像；
    void getDownFrame(Mat &dst);
    // 获取前视摄像头原图像；
    void getFwdFrame(Mat &dst);
    // 获取俯视摄像头预处理图像；
    void getDownPrePro(Mat &dst);
    // 获取前视摄像头预处理图像；
    void getFwdPrePro(Mat &dst);

    // 获取寻线二值图
    void getBinLine(Mat &dst);
    // 获取过门二值图
    void getBinDoor(Mat &dst);
    // 获取撞球远状态图像
    void getBinBallsFar(Mat &dst);
    // 获取撞球NEAR, ATTACH状态下红球二值化图像
    void getBinRedBall(Mat &dst);
    // 获取撞球NEAR, ATTACH状态下黄绿球二值化图像
    void getBinYelGreBalls(Mat &dst);

    // 获得图像信息；
    void getVisionInfo(VisionInfo &dst);
    //***********************************
    //          图像处理函数
    //***********************************

    // 俯视、前视图像预处理
    void FramesPrePro();
    // 寻线任务，二值化处理
    void BinLinePro();
    // 过门任务，二值化处理
    void BinDoorPro();
    // 撞球任务，二值化处理
    void BinBallsPro(int state);
    // 风格过门任务，二值化处理
    void BinStyleDoor(int state);
    // 鱼雷切板任务，二值化处理
    void BinTorBoard();
    // 投放标志物任务，二值化处理
    void BinAnchor();
    //
    void BinOctCoins();

    //***********************************
    //          缓存处理函数
    //***********************************

    // 更新DoorState缓存向量；
    void UpdateTmpDoorState(int src);
    // 更新DoorRect缓存向量；
    void UpdateTmpDoorRect(int doorstate, RotatedRect src);
    // 更新LineIDState缓存向量；
    void UpdateTmpLineIDState(int src);
    // 更新LineIDROI缓存向量；
    void UpdateTmpLineIDROI(int lineIDstate, Rect src);
    // 更新BallState缓存向量；
    void UpdateTmpBallsState(int ballstate);
    // 更新StyleDoorState缓存向量；
    void UpdateTmpStyleDoorState(int stylestate);
    // 更新StyleDoorRect缓存向量(num = 2)；
    void UpdateTmpStyleDoorRect(int stylestate, RotatedRect src);
    // 更新StyleDoorSeenState缓存向量；
    void UpdateTmpStyleDoorSeenState(int stylestate);
    // 更新StyleDoorRect缓存向量(num = 3)；
    void UpdateTmpStyleDoorSeenRect(int stylestate, RotatedRect src);

    // 处理Door相关缓存向量；
    void ProcessTmpDoor(int &doorstate, RotatedRect &rect);
    // 处理LineID相关缓存向量；
    void ProcessTmpLineID(int &lineIDstate, Rect &ROI);
    // 处理ballstate相关缓存向量；
    void ProcessTmpBallState(int &ballstate);
    // 处理StyleDoorState相关缓存向量(num = 2)；
    void ProcessTmpStyleDoor(int &stylestate, RotatedRect &rect);
    // 处理StyleDoorState相关缓存向量(num = 3)；
    void ProcessTmpStyleSeenDoor(int &stylestate, RotatedRect &rect);

    // 求DoorRect缓存向量中的旋转矩形的平均值；
    void AverageTmpDoorRect(RotatedRect &dst);
    // 求tmpLineIDRI缓存向量中的ROI矩形的平均值；
    void AverageTmpLineIDROI(Rect &ROI);
    // 求StyleDoorRect缓存向量中的旋转矩形的平均值；
    void AverageTmpStyleDoorRect(RotatedRect &dst);

    //***********************************
    //          分析函数
    //***********************************

    // 分析二值图，获得寻线信息；
    void AnalyseGuideLine(int num = 6);
    // 分析二值图，获得过门信息；
    void AnalyseFirstDoor(int num = 7);
    // 分析二值图，获得撞球信息；
    void AnalyseBalls(int ColorNum, int num = 7);
    // 分析二值图，获得风格过门信息；
    void AnalyseStyleDoor(int polesNum = 3, int num = 7);
    // 分析鱼雷切板，获得相关信息；
    void AnalyseTorBoard(int num = 7);
    // 分析投放标志物，获得相关信息；
    void AnalyseAnchor(int num = 7);
    //
    void AnalyseOctCoins();

    //***********************************
    //          记录函数
    //***********************************

    // 生成流输出文件名，该文件名根据系统时间改变
    void CreateRecordFileName();
    // 记录VisionInfo，用流输出至文件；
    void RecordVisionInfo(int StrNum);

    //***********************************
    //        调试使用的cout函数
    //***********************************

    // cout过门任务信息；
    void PrintFirstDoorInfo(int num = 6);
    // cout寻线任务信息；
    void PrintLineInfo(int num = 6);
    // cout撞球任务信息；
    void PrintBallsInfo();
    // cout风格过门任务信息(num = 2)；
    void PrintStyleDoorInfo();
    // cout风格过门任务信息(num = 3)；
    void PrintStyleSeenDoorInfo();
    // cout鱼雷切板任务信息
    void PrintTorBoardInfo();
    // cout投放标志物任务信息
    void PrintAnchorInfo();

    //***********************************
    //        调试使用的显示函数
    //***********************************

    // 显示过门图像
    void DrawFirstDoor(int num = 6);
    // 显示寻线图像
    void DrawGuideLine(int num = 6);
    // 显示撞球图像
    void DrawBalls();
    // 显示风格过门图像(num = 2)
    void DrawStyleDoor();
    // 显示风格过门图像(num = 3)
    void DrawStyleSeenDoor();
    // 显示鱼雷切板任务图像
    void DrawTorBoard();
    // 显示投放标志物任务图像
    void DrawAnchor();
    //
    void DrawOctCoins();

private:
    // 视觉系统内容
    NCCamera DownCamera;
    NCCamera FwdCamera;
    NCDownModel DownModel;
    NCFwdModel FwdModel;
    NCImgPro ImgPro;
    VisionInfo VInfo;

    // 各任务二值图
    Mat BinLine;
    Mat BinDoor;

    // 缓存向量
    vector<int> tmpDoorState;
    vector<RotatedRect> tmpDoorRect;
    vector<int> tmpLineIDState;
    vector<Rect> tmpLineIDROI;
    vector<int> tmpBallState;
    vector<int> tmpStyleDoorState;
    vector<RotatedRect> tmpStyleDoorRect;
    vector<int> tmpStyleDoorSeenState;

    // 记录文件相关
    string RecordFileName;
};

#endif // NCVISION_H
