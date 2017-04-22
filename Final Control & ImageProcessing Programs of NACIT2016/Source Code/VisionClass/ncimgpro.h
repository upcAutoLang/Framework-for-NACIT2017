#ifndef NCIMGPRO_H
#define NCIMGPRO_H

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>
#include <direct.h>

#include "cv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <FGCamera.h>

#include <GeneralImageProcess/contours_fun.h>
#include <GeneralImageProcess/findline_fun.h>
#include <GeneralImageProcess/imageprocessing_fun.h>
#include <SupportFunctions/xmls_fun.h>

#include <ncheader.h>

using namespace std;
using namespace cv;

class NCImgPro
{
public:
    //=================================
    //         基本操作
    //=================================
    NCImgPro();
    // 初始化摄像头消除畸变的信息
    void InitCameraUndist();
    // 初始化各颜色阈值信息
    void InitHSVRGBInfo();

    // 为俯视、前视摄像头原图像赋值
    void setDownFrame(Mat src);
    void setFwdFrame(Mat src);
    // 获取俯视、前视摄像头原图像
    void getDownFrame(Mat &dst);
    void getFwdFrame(Mat &dst);
    // 获取俯视、前视摄像头消除畸变后的图像
    void getDownFrameUndist(Mat &dst);
    void getFwdFrameUndist(Mat &dst);
    // 获取俯视、前视摄像头拉伸后的图像
    void getDownFrameStrench(Mat &dst);
    void getFwdFrameStrench(Mat &dst);
    // 获取俯视、前视摄像头预处理后的图像
    void getDownFramePrePro(Mat &dst);
    void getFwdFramePrePro(Mat &dst);
    // 获取门的二值化图像
    void getimgBinDoor(Mat &dst);
    void getimgBinLine(Mat &dst);
    void getimgBinLineID(Mat &dst);
    void getimgBinBallsFar(Mat &dst);
    void getimgBinRedBall(Mat &dst);
    void getimgBinYellowBall(Mat &dst);
    void getimgBinGreenBall(Mat &dst);
    void getimgBinYelGreBalls(Mat &dst);
    void getimgBinStyleDoor(Mat &dst);
    void getimgBinTorBoard(Mat &dst);
    void getimgBinTorRedEdge(Mat &dst);
    void getimgBinAnchorBox(Mat &dst);
    void getimgBinAnchorCover(Mat &dst);
    void getimgBinAnchorPattern(Mat &dst);
    void getimgBinOctRedCoins(Mat &dst);

    // 俯视摄像头标定重绘
    void DownFrameCalib();
    // 前视摄像头标定重绘
    void FwdFrameCalib();

    // 俯视摄像头裁剪
    void DownFrameCut();
    // 前视摄像头裁剪
    void FwdFrameCut();

    // 俯视摄像头拉伸
    void DownFrameStrench();
    // 前视摄像头拉伸
    void FwdFrameStrench();

    // 俯视摄像头图像预处理
    void DownFramePrePro();
    // 前视摄像头图像预处理
    void FwdFramePrePro();


    //=================================
    //          识别部分
    //=================================

    // 寻找门，生成二值化图像
    void BinDoor(int num = 5);
    // 寻找导引路径，生成二值化图像
    void BinLine(int num = 4);
    // 寻找三个球，生成二值化图像
    void BinBalls(int state = 0, int num = 7);
    // 寻找风格过门，生成二值化图像；
    void BinStyleDoor(int state = 0, int num = 7);
    // 寻找鱼雷切板，生成二值化图像；
    void BinTorBoard(int num = 7);
    // 寻找标志物箱子，生成二值化图像；
    void BinAnchors(int num = 7);
    //
    void BinOctCoins();

protected:
    //========================================
    //            Mat图像相关
    //========================================

    // 裁剪尺寸
    int cutwidth, cutheight;

    // 摄像头采集到的原图像
    Mat DownFrame, FwdFrame;
    // 处理畸变后的图像
    Mat DownFrameUdt, FwdFrameUdt;
    // 处理畸变，拉伸后的图像
    Mat DownStrench, FwdStrench;

    // 处理畸变、裁剪中间部分后的图像，被用于后续的处理
    Mat DownImg, FwdImg;

    // 经过处理的二值化图像
    Mat imgBinLine, imgBinDoor;
    // 过门过程中前视摄像头看到的导引路径
    Mat imgBinLineID;
    // 撞球过程处理得到的二值图
    Mat imgBinBallsFar, imgBinRed, imgBinYelGre, imgBinYellow, imgBinGreen;
    // 风格过门过程得到的二值图
    Mat imgBinStyleDoor;
    // 鱼雷切板二值图
    Mat imgBinTorBoard;
    // 鱼雷切板红色边缘二值图
    Mat imgBinTorRedEdge;
    // 投放标志物盖子二值图
    Mat imgBinAnchorCover;
    // 投放标志物箱子二值图
    Mat imgBinAnchorBox;
    // 投放标志物内部黄色图案轮廓二值图
    Mat imgBinAnchorPattern;
    //
    Mat imgBinRedCoins;

    //========================================
    //            畸变处理相关
    //========================================

    // 原图像尺寸
    Size FrameSize;
    // 俯视、前视摄像头内参数矩阵与畸变矩阵
    Mat DownIntrinMat, DownDistortMat;
    Mat FwdIntrinMat, FwdDistortMat;

    //========================================
    //         判断所需轮廓相关
    //========================================

    // 前视、俯视摄像头图像处理后的轮廓
    vector<vector<Point> > DownContours, FwdContours;

    //========================================
    //           颜色阈值信息相关
    //========================================

    // 过门需要的图像阈值信息
    HSVInfo DoorHSV1, DoorHSV2;
    RGBInfo DoorRGB1, DoorRGB2;

    HSVInfo LineIDHSV1, LineIDHSV2;
    RGBInfo LineIDRGB1, LineIDRGB2;

    // 寻线需要的图像阈值信息
    HSVInfo LineHSV1, LineHSV2;
    RGBInfo LineRGB1, LineRGB2;

    //********************************
    // 撞球需要的图像阈值信息（有补光状态下）
    //********************************

    // 远状态时的颜色信息
    HSVInfo BallsFarHSV1, BallsFarHSV2;
    RGBInfo BallsFarRGB1, BallsFarRGB2;
    // 近状态时的颜色信息
    HSVInfo RedNearHSV1, RedNearHSV2;
    RGBInfo RedNearRGB1, RedNearRGB2;
    HSVInfo YelGreNearHSV1, YelGreNearHSV2;
    RGBInfo YelGreNearRGB1, YelGreNearRGB2;
    // 贴近状态时的颜色信息
    HSVInfo RedAttHSV1, RedAttHSV2;
    RGBInfo RedAttRGB1, RedAttRGB2;
    HSVInfo YelGreAttHSV1, YelGreAttHSV2;
    RGBInfo YelGreAttRGB1, YelGreAttRGB2;

    //********************************
    //   撞球图像阈值信息（无补光状态下）
    //********************************

    // 近状态时的颜色信息
    HSVInfo RedHSV1, RedHSV2;
    RGBInfo RedRGB1, RedRGB2;
    HSVInfo YellowHSV1, YellowHSV2;
    RGBInfo YellowRGB1, YellowRGB2;
    HSVInfo GreenHSV1, GreenHSV2;
    RGBInfo GreenRGB1, GreenRGB2;
    HSVInfo YelGreHSV1, YelGreHSV2;
    RGBInfo YelGreRGB1, YelGreRGB2;

    //********************************
    // 风格过门图像阈值信息（无补光状态下只使用Near）
    //********************************

    // 远距离状态风格过门颜色信息
    HSVInfo StyleDoorFarHSV1, StyleDoorFarHSV2;
    RGBInfo StyleDoorFarRGB1, StyleDoorFarRGB2;

    // 近距离状态风格过门颜色信息
    HSVInfo StyleDoorNearHSV1, StyleDoorNearHSV2;
    RGBInfo StyleDoorNearRGB1, StyleDoorNearRGB2;

    //********************************
    // 鱼雷切板图像阈值信息（无补光状态下）
    //********************************

    // 鱼雷切板颜色信息
    HSVInfo TorBoardHSV1, TorBoardHSV2;
    RGBInfo TorBoardRGB1, TorBoardRGB2;

    // 鱼雷切板红色边缘颜色信息
    HSVInfo TorRedEdgeHSV1, TorRedEdgeHSV2;
    RGBInfo TorRedEdgeRGB1, TorRedEdgeRGB2;

    //********************************
    //  标志物俯视阈值信息（补光状态下）
    //********************************

    // 投放标志物盖子颜色信息
    HSVInfo AnchorCoverHSV1, AnchorCoverHSV2;
    RGBInfo AnchorCoverRGB1, AnchorCoverRGB2;

    // 投放标志物箱子颜色信息
    HSVInfo AnchorBoxHSV1, AnchorBoxHSV2;
    RGBInfo AnchorBoxRGB1, AnchorBoxRGB2;

    // 投放标志物箱内黄色图案颜色信息
    HSVInfo AnchorPatternHSV1, AnchorPatternHSV2;
    RGBInfo AnchorPatternRGB1, AnchorPatternRGB2;

    //
    HSVInfo RedCoinHSV1, RedCoinHSV2;
    RGBInfo RedCoinRGB1, RedCoinRGB2;

};

#endif // NCIMGPRO_H
