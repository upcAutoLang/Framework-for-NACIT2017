#ifndef PORTS_FUN
#define PORTS_FUN

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>

// QString类型转为可用十六进制形式发出的串口数据
QByteArray QString2Hex(QString str);

// ASCII字符转换为十六进制字符
char ConvertHexChar(char ch);

// 将十进制整形数转换成十六进制QString
QString Dec2QStringHex(int src);

#endif // PORTS_FUN

