#ifndef DRAW_FUN
#define DRAW_FUN

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

// 绘制拟合曲线
void PaintFitLine(Mat src, Mat &dst, float *line,
                  const Scalar& color = CV_RGB(0,255,0), int thickness = 3);

// 绘制旋转矩形
void PaintRotatedRect(Mat src, Mat &dst, RotatedRect rect,
                      const Scalar& color = CV_RGB(0, 255, 0), int thickness = 3);

// 绘制矩形
void PaintRect(Mat src, Mat &dst, Rect rect,
               const Scalar& color = CV_RGB(0, 255, 0), int thickness = 3);
#endif // DRAW_FUN

