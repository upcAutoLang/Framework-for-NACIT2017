// These Functions are based on Linux System.
// My System is Ubuntu 14.04 LTS.

#ifndef SYS_FUN_H
#define SYS_FUN_H

#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <time.h>
#include <strstream>
#include <sstream>
#include <string.h>

#include "string_fun.h"

using namespace std;

// 创建文件夹，如果已经存在，则返回false；如果不存在则创建文件夹，且返回true
bool CreateFolder(string name, bool output = true);

#endif // SYS_FUN_H
