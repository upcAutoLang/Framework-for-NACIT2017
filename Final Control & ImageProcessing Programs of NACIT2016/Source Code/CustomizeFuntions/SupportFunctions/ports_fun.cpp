#include <SupportFunctions/ports_fun.h>

/*===================================================================
 * 函数名：QString2Hex
 *
 * 功能：QString类型转换为十六进制形式的QByteArray类型，可以通过串口输出；
 *
 * 参数：
 *  QString str: 需要转换的QString字符串
=====================================================================
*/
QByteArray QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

/*===================================================================
 * 函数名：ConvertHexChar
 *
 * 功能：ASCII字符转换为十六进制字符
 *
 * 参数：
 *  char ch:    ASCII字符
=====================================================================
*/
char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

/*===================================================================
 * 函数名：Dec2QStringHex
 *
 * 功能：将十进制整形数转换成十六进制QString；
 *
 * 参数：
 *  int src:    需要被转换的十进制整数
=====================================================================
*/
QString Dec2QStringHex(int src)
{
    if(src > 0)
    {
        QString dst;
        dst = QString::number(src, 16).toUpper();
        return dst;
    }
    else
        return NULL;
}
