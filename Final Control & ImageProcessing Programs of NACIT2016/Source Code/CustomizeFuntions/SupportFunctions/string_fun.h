#ifndef STRING_FUN
#define STRING_FUN

#include <stdlib.h>
#include <stdio.h>
#include "highgui.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <direct.h>
#include <time.h>
#include <sstream>

using namespace std;

// 获取exe工作目录
string getExeDirect();

// 获取上一级工作目录
string getUpDirect();

// double转换为string
string double2String(double d);

// 将其他数据转换为string
template <class T> string m_toStr(T tmp);

// 将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type> Type stringToNum(const string& str);

// 获取系统时间
string getSysTime();

#endif // STRING_FUN
