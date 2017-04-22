#ifndef XMLS_FUN
#define XMLS_FUN

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "highgui.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>

#include <CustomizeStructs/imageprocessheader.h>

using namespace cv;
using namespace std;

// 从XML文件中获取HSV阈值信息
void GetHSVInfoFromXML(string fileName, int &HMin, int &HMax,
                       int &SMin, int &SMax, int &VMin, int &VMax);
void GetHSVInfoFromXML(string fileName, HSVInfo &Info);

// 从XML文件中获取RGB阈值信息
void GetRGBInfoFromXML(string fileName, int &RMin, int &RMax,
                       int &GMin, int &GMax, int &BMin, int &BMax);
void GetRGBInfoFromXML(string fileName, RGBInfo &Info);

// 从XML文件中获取并减运算相关HSV、RGB阈值信息
void GetJoinMinusInfoFromXML(string fileName, HSVInfo &HSVInfo1, HSVInfo &HSVInfo2,
                          RGBInfo &RGBInfo1, RGBInfo &RGBInfo2);

// 根据输入的XML路径，输出对应XML路径的摄像头相关标定信息：内参数矩阵、畸变矩阵、图像尺寸
void GetCameraCalibInfoFromXML(string XML_address, Size &FrameSize,
                               Mat &IntrinMat, Mat &DistortMat);

#endif // XMLS_FUN

