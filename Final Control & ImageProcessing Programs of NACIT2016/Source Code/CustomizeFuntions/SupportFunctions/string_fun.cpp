#include <SupportFunctions/string_fun.h>

/*===================================================================
 * 函数名：getExeDirect
 *
 * 功能：获取exe工作目录
 *
 * 说明：
 *  返回string字符串，字符串内容为运行的exe文件所在路径；
=====================================================================
 */
string getExeDirect()
{
    char buf[256];
    getcwd(buf, 256);
    string cwd;
    cwd = buf;
    return cwd;
}

/*===================================================================
 * 函数名：getUpDirect
 *
 * 功能：获取上一级工作目录
 *
 * 说明：
 *  返回string字符串，字符串内容为运行的exe上一级工作路径；
=====================================================================
 */
string getUpDirect()
{
    string tmp, cmd, tmp1;
    tmp = getExeDirect();
    tmp1 = tmp.substr(tmp.find_last_of('\\') + 1, tmp.length() - tmp.find_last_of('\\'));
    cmd = tmp.substr(0, tmp.length() - tmp1.length());
    return cmd;
}

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

// 获取系统时间
/*===================================================================
 * 函数名：getSysTime
 *
 * 功能：获取系统时间
 *
 * 说明：
 *  获取月、日、时、分、秒信息；
 *
 * 用法：
 *  返回值：例：0513_225430;
=====================================================================
 */
string getSysTime()
{
    string dst;

    //time_t是long类型，精确到秒，是当前时间和1970年1月1日零点时间的差
    const time_t t = time(NULL);
    struct tm *CurTime = localtime(&t);

    // 初始化
    dst = "";
    char tmp[3];
    char datetmp[64];
    strcpy(datetmp, "");

    // 月、日
    sprintf(tmp, "%02d", 1 + CurTime->tm_mon);
    strcat(datetmp, tmp);
    sprintf(tmp, "%02d", CurTime->tm_mday);
    strcat(datetmp, tmp);

    // 分隔符
    strcat(datetmp, "_");

    // 时、分、秒
    sprintf(tmp, "%02d", CurTime->tm_hour);
    strcat(datetmp, tmp);
    sprintf(tmp, "%02d", CurTime->tm_min);
    strcat(datetmp, tmp);
    sprintf(tmp, "%02d", CurTime->tm_sec);
    strcat(datetmp, tmp);

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
