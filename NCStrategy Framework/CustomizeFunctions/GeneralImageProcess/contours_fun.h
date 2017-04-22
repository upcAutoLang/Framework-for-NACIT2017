#ifndef CONTOURS_FUN
#define CONTOURS_FUN

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "highgui.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;


// 矩形拟合，并绘制输入的轮廓
void PaintRectContours(Mat &src, Mat &dst,
                       vector<vector<Point> > &contours);

// 轮廓按面积排序
void ContoursAreaDownSort(vector<vector<Point> > &contours);

// 轮廓按长度排序
void ContoursLengthDownSort(vector<vector<Point> > &contours);

// 轮廓从左向右排序
void ContoursLeftDownSort(vector<vector<Point> > & contours);

// 轮廓从下向上排序
void ContoursUpWardDownSort(vector<vector<Point> > &contours);

// 根据排序后的名次删除冗余轮廓
void DeleteContoursByRank(vector<vector<Point> > &contours, int savenum = 3);

// 打印轮廓与图像中心点距离信息
void PrintContoursDistInfo(vector<vector<Point> > &contours,
                       vector<int> &XDistance,
                       vector<int> &YDistance);

// 输出轮廓与屏幕中心X、Y方向上的距离
void CalcuContoursDistance(Mat img,
                           vector<vector<Point> > &contours,
                           vector<int> &XDistance,
                           vector<int> &YDistance);

// 计算轮廓中心在图像中的坐标
void CalcuContoursCenterPoint(vector<vector<Point> > &contours,
                              vector<Point> &CenterPoints);

// 打印轮廓与图像中心点距离信息
void PrintContoursCenterPointInfo(vector<vector<Point> > &contours,
                                  vector<Point> &CenterPoints);

// 寻找轮廓的最小外包矩形
void ContoursMinAreaRect(vector<vector<Point> > &contours,
                         RotatedRect &MinAreaBox);

// 寻找RotatedRect向量的最小外包矩形
void RectsMinAreaRect(vector<RotatedRect> Rects,
                      RotatedRect &MinAreaBox);

// 寻找轮廓的最小外包矩形的中点
void ContoursMinAreaRectCenter(vector<vector<Point> > &contours,
                         Point2f &Center);

// 寻找轮廓的最小外包矩形的中线
void ContoursMinAreaRectMidLine(vector<vector<Point> > &contours,
                                Point2f &UpMidPoint, Point2f &DownMidPoint);

// 求几个缓存矩形的均值；
void AverageRect(RotatedRect &dst, RotatedRect src1, RotatedRect src2, RotatedRect src3);

// 根据一个轮廓，拟合出一个旋转矩形；
void ContourFitRotatedRect(vector<Point> contour, RotatedRect &rect);

// 根据一个轮廓，拟合出一个ROI矩形；
void ContourFitROIRect(vector<Point> contour, Rect &rect);

// 根据一个轮廓，算出拟合旋转矩形的长边；
void ContourRotatedRectLength(vector<Point> contour, float &length);

// Rect转RotatedRect
RotatedRect RotatedRect2Rect(Rect rect);

// 剔除自然光分量函数
void DeleteTopLightVoice(Mat src, vector<vector<Point> > &contours,
                         double upratio);

// 剔除摄像头底部光分量函数
void DeleteBottomLightVoice(Mat src, vector<vector<Point> > &contours,
                            double DownRatio);

// 按照轮廓尺寸，剔除尺寸过小的轮廓
void DeleteSmallContours(vector<vector<Point> > &contours, int limit);

// 计算两点连线的角度
double calcLineDegree(Point2f& firstPt, Point2f& secondPt);

// 计算RotatedRect长边的angle
double getRcDegree(RotatedRect box);

#endif // CONTOURS_FUN

