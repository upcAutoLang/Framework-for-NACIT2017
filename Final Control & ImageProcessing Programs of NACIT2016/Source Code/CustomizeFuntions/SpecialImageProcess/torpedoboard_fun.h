#ifndef TORPEDOBOARD_FUN
#define TORPEDOBOARD_FUN

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "highgui.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>

#include <GeneralImageProcess/contours_fun.h>

using namespace cv;
using namespace std;

// 根据输入的轮廓，找出鱼雷切板的四个洞，并按次排序
void FindTorpedoBoardHoles(vector<vector<Point>> &contours);

// 矩形拟合，并绘制输入的轮廓
void TorpedoBoardPaintRectContours(Mat &src, Mat &dst,
                                   vector<vector<Point>> &contours,
                                   vector<Point> &CenterPoints);
#endif // TORPEDOBOARD_FUN

