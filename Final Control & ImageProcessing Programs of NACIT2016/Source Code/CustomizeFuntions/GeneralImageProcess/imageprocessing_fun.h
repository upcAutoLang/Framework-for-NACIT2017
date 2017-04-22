#ifndef IMAGEPROCESSING_FUN
#define IMAGEPROCESSING_FUN

#include "cv.h"
#include <opencv2/core/core.hpp>
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

// 使用HSV阈值，将输入图像处理成为二值化图像
void HSVThresh(Mat &src, Mat &dst,
               int HMin = 0, int HMax = 180,
               int SMin = 0, int SMax = 255,
               int VMin = 0, int VMax = 255,
               int Method = 3);
void HSVThresh(Mat &src, Mat &dst,
               HSVInfo Info, int Method = 3);

// 使用RGB阈值，将输入图像处理成为二值化图像
void RGBThresh(Mat &src, Mat &dst,
               int RMin = 0, int RMax = 255,
               int GMin = 0, int GMax = 255,
               int BMin = 0, int BMax = 255,
               int Method = 3);
void RGBThresh(Mat &src, Mat &dst,
               RGBInfo Info, int Method = 3);

// 根据输入原畸变图像、内参数矩阵、畸变矩阵、原图像尺寸，输出处理后的图像
bool UndistortCameraFrame(Mat &src, Mat &dst,
                          Size FrameSize, Mat IntrinMat, Mat DistortMat);

// 根据输入长宽，裁剪原图像的中心部分图像
void CutCenterImage(Mat &src, Mat &dst, int width, int height, int offsetX = 0, int offsetY = 0);

// 图像纵横方向拉伸
void StrenchImage(Mat src, Mat &dst, double ratioX, double ratioY);

// 二值化图像并运算
void BinMatJoin(Mat src1, Mat src2, Mat &dst);

// 二值化图像减运算，将第二张图像中的白色像素从第一张图像中减去；
void BinMatMinus(Mat src1, Mat src2, Mat &dst);

// 两个HSV并运算，再进行两个RGB减运算
void BinMatDoubleJoinMinus(Mat src, Mat &dst,
                           HSVInfo HSVInfo1, HSVInfo HSVInfo2,
                           RGBInfo RGBVoice1, RGBInfo RGBVoice2);
#endif // IMAGEPROCESSING_FUN

