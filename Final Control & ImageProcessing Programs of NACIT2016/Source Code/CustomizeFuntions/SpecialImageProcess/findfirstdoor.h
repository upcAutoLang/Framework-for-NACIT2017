#ifndef FINDFIRSTDOOR
#define FINDFIRSTDOOR

#include <GeneralImageProcess/contours_fun.h>

// 寻找第一个门；
void FindDoorRect(vector<vector<Point> > &contours, Mat src,
                  RotatedRect &Rect);

#endif // FINDFIRSTDOOR

