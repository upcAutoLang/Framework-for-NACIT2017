#ifndef STRING_FUN
#define STRING_FUN

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <string>
#include <time.h>
#include <sstream>
#include <strstream>
#include <unistd.h>
#include <fstream>

#include <sys/stat.h>
#include <sys/timeb.h>

using namespace std;

enum
{
    TIME_NOSPACE,
    TIME_STRING,
    TIME_NOYEAR,
    TIME_NORMAL,
    TIME_NOYMD_NOMS,
    TIME_NOYMD_WITHMS
};

// double转换为string
string double2String(double d);

// 将其他数据转换为string
template <class T> string m_toStr(T tmp);

// 将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type> Type stringToNum(const string& str);

// 获取系统时间
string getSysTime(int form = TIME_NOSPACE);


char* int2char(int src);
void CutCommand(string src, string &dst1, string &dst2);
char* string2char(string src);
string DeleteSidesSpace(string src);
string rmSpace(string src);


#endif // STRING_FUN
