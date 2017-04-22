#ifndef FINDLINE_FUN
#define FINDLINE_FUN

#include "cv.h"
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "highgui.h"
#include <math.h>
#include <iostream>

#include <GeneralImageProcess/contours_fun.h>

using namespace std;
using namespace cv;


// （初版本）二值化图像生成拟合直线，并返回一个角度值
void BinaryFitLine(Mat &src, float *line, double &angle, bool &noLine, int res = 10);
// （二代版本）二值化图像生成拟合直线，并返回一个角度值
void BinaryFitLine2(Mat src, float *line, double &angle, bool &noLine, int &lineOrpoint);

// ROI内，二值化图像生成拟合直线，并返回一个角度值
void BinaryROIFitLine(Mat src, Rect rect, float *line, double &angle, int res = 3);

// ROI区域过小时，连接ROI中心与屏幕中心的直线，并返回一个角度值；
void BinaryCenterScreenLine(Mat src, Rect rect, float *line, double &angle);


// （初版本）计算直线在图像中的位置（左/中/右）
int CalcuLMR(Mat &img, float *line, double percent = 0.1, bool DrawCenterLimitLine = false);

// （二代版本）计算直线在图像中的位置（左/中/右）
int CalcuLMR2(Mat &img, float *line, double percent = 0.1, bool DrawCenterLimitLine = false);
#endif // FINDLINE_FUN

