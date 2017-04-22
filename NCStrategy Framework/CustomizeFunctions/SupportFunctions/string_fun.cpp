#include <string_fun.h>

// double转换为string
string double2String(double d)
{
    ostringstream os;
    if (os << d)
        return os.str();
    return "invalid conversion";
}

/*===================================================================
 * 函数名：m_toStr
 *
 * 功能：将其他数据转换为string
 *
 * 说明：
 *  模板函数，将其他数据转换为string；
 *
 * 用法：
 *  string str = m_toStr<int>(12345);
=====================================================================
 */
template <class T> string m_toStr(T tmp)
{
    stringstream ss;
    ss << tmp;
    return ss.str();
}

/*===================================================================
 * 函数名：getSysTime
 *
 * 功能：获取系统时间
 *
 * 参数：
 *      int form:  格式选项
 *          form == TIME_NOSPACE:   输出格式20170402100542
 *          form == TIME_STRING:    输出格式2017.04.02  10:05:42
 *          form == TIME_NOYEAR:    输出格式0402100542
 *          form == TIME_NORMAL:    输出格式：20170402_100542
 *          form == TIME_NOYMD:     输出格式：10:05:42
 *
 * 说明：
 *  获取月、日、时、分、秒信息；
 *
 * 用法：
 *  返回值：例：0513_225430;
=====================================================================
 */
string getSysTime(int form)
{
    string dst;

//    //time_t是long类型，精确到秒，是当前时间和1970年1月1日零点时间的差
//    const time_t t = time(NULL);

    // Linux下C++11的获取系统毫秒级别函数
    struct timeb tb;
    ftime(&tb);
    //time_t是long类型，精确到秒，是当前时间和1970年1月1日零点时间的差
    struct tm *CurTime = localtime(&tb.time);

    // 初始化
    dst = "";
    char tmp[4];
//    char datetmp[64];
    char datetmp[64] = "";
//    strcpy(datetmp, "");

    switch(form)
    {
    case TIME_NOSPACE:
        // 格式：20170402100542
        // 年、月、日
        sprintf(tmp, "%04d", 1900 + CurTime->tm_year);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", 1 + CurTime->tm_mon);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_mday);
        strcat(datetmp, tmp);
        // 时、分、秒
        sprintf(tmp, "%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);
        break;
    case TIME_STRING:
        // 格式：2017.04.02  10:05:42
        sprintf(tmp, "%04d", 1900 + CurTime->tm_year);
        strcat(datetmp, tmp);

        strcat(datetmp, (char *)".\0");
        sprintf(tmp, "%02d", 1 + CurTime->tm_mon);
        strcat(datetmp, tmp);
        strcat(datetmp, (char *)".\0");

//        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_mday);
        strcat(datetmp, tmp);
        strcat(datetmp, (char *)"  \0");

        // 时、分、秒
        sprintf(tmp, "%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        strcat(datetmp, (char *)":\0");

//        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        strcat(datetmp, (char *)":\0");

//        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);

//        sprintf(tmp, "%00d", 0);
//        strcat(datetmp, tmp);
        break;
    case TIME_NOYEAR:
        // 格式：0402100542
        // 月、日
        sprintf(tmp, "%02d", 1 + CurTime->tm_mon);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_mday);
        strcat(datetmp, tmp);
        // 时、分、秒
        sprintf(tmp, "%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);
        break;
    case TIME_NORMAL:
        // 格式：20170402_100542
        // 年、月、日
        sprintf(tmp, "%04d", 1900 + CurTime->tm_year);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", 1 + CurTime->tm_mon);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_mday);
        strcat(datetmp, tmp);
        // 时、分、秒
        sprintf(tmp, "_%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        sprintf(tmp, "%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);
        break;
    case TIME_NOYMD_NOMS:
        // 格式：10:05:42
        // 时、分、秒
        sprintf(tmp, "%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        sprintf(tmp, ":%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        sprintf(tmp, ":%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);
        break;
    case TIME_NOYMD_WITHMS:
        // 格式：10:05:42.243
        // 时、分、秒、毫秒
        sprintf(tmp, "%02d", CurTime->tm_hour);
        strcat(datetmp, tmp);
        sprintf(tmp, ":%02d", CurTime->tm_min);
        strcat(datetmp, tmp);
        sprintf(tmp, ":%02d", CurTime->tm_sec);
        strcat(datetmp, tmp);
        sprintf(tmp, ".%02d", tb.millitm/10);
        strcat(datetmp, tmp);
        break;
    default:
        break;
    }

    dst += datetmp;
    return dst;
}

// 将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type> Type stringToNum(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

/*===================================================================
 * 函数名：CutCommand
 *
 * 功能：将数据从@号为界，切分成两部分
 *
 * 参数：
 *      string src:  原字符串
 *      string &dst1:       @符号前，被切分的字符串
 *      string &dst2:       @符号后，被切分的字符串
 *
 * 用法：
 *      CutCommand(RecvTotalCmd, RecvName, RecvCmd);
 *      若RecvTotalCmd == image@bgr2gray，则：
 *      RecvName = "image"
 *      RecvCmd = "bgr2gray"
=====================================================================
 */
void CutCommand(string src, string &dst1, string &dst2)
{
    // 删除所有的换行符
    for(int i = src.length(); i>=0; i--)
        if(src[i] == '\n')      src.replace(i, 1, "");

    // 删除string两边空格
    src = DeleteSidesSpace(src);

//    src = src.replace("\\n", "");
    dst1 = src.substr(0, src.find_first_of('@'));
    dst2 = src.substr(src.find_last_of('@') + 1, src.length());
}

/*===================================================================
 * 函数名：DeleteSidesSpace(string src)
 *
 * 功能：删除输入参数src两边空格
 *
 * 参数：
 *      string src:  原字符串
 * 返回值：string
 *
 * 用法：
 *      src = DeleteSidesSpace(src);
 *      若src == "   image@bgr2gray   "，则返回后的src：
 *      src = "image@bgr2gray"
=====================================================================
 */
string DeleteSidesSpace(string src)
{
    src.replace(0, src.find_first_not_of(" "), "");
    src.replace(src.find_last_not_of(' ') + 1, src.length(), "");
    return src;
}

/*===================================================================
 * 函数名：int2char(int src)
 *
 * 功能：int数据类型转成char*
 *
 * 参数：
 *      int src:    需要转换的int数据
 * 返回值：char*
 *
 * 用法：
 *      char ×dst = int2char(i);
 *      若i = 234897，则dst为234897的字符串
=====================================================================
 */
char* int2char(int src)
{
    char *dst;
    string tmpstr;
    strstream tmpss;
    tmpss<<src;
    tmpss>>tmpstr;

    dst = string2char(tmpstr);
    return dst;
}

/*===================================================================
 * 函数名：string2char(int src)
 *
 * 功能：string数据类型转成char*
 *
 * 参数：
 *      string src:    需要转换的string字符串
 * 返回值：char*
 *
 * 用法：
 *      略
=====================================================================
 */
char* string2char(string src)
{
    char *dst;
    int len = src.length();
    dst = (char*)malloc((len + 1) * sizeof(char));
    memset(dst, 0, sizeof(dst) / sizeof(char));
    src.copy(dst, len, 0);
    return dst;
}

/*===================================================================
 * 函数名：string rmSpace(string src)
 *
 * 功能：移除string数据中的所有空格
 *
 * 参数：
 *      string src:    需要移除空格的string字符串
 * 返回值：string
=====================================================================
 */
string rmSpace(string src)
{
    // 移除空格
    for(int i = src.length(); i>=0; i--)
        if(src[i] == ' ')      src.replace(i, 1, "");
    return src;
}
