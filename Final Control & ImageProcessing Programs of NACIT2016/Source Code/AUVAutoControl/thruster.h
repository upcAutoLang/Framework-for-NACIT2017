#ifndef THRUSTER_H
#define THRUSTER_H

#include <nccamera.h>
#include <ncmodel.h>
#include <ncdownmodel.h>
#include <ncfwdmodel.h>
#include <ncimgpro.h>
#include <ncvision.h>

#include <SpecialImageProcess/findfirstdoor.h>
#include <SupportFunctions/string_fun.h>
#include <GeneralImageProcess/imageprocessing_fun.h>
#include <QString>
//1.	下传角度：*61240.00~
//6表示的是240.00共六位，2表示指令类型，240.00表示下发的是240.00度
//2.	下传深度：*620.0015~

const QString CAngle="*61240.00\r\n";
const QString CDepth="*620.0015\r\n";

//************************
//      主推
//************************

// 主推停止
const QString M_S  = "#130\r\n";

// 主推基本动作
const QString M_F1 = "#131\r\n";
const QString M_F2 = "#132\r\n";
const QString M_F3 = "#133\r\n";
const QString M_B1 = "#134\r\n";
const QString M_B2 = "#135\r\n";
const QString M_B3 = "#136\r\n";

// 主推微调
const QString M_FS1 = "$131\r\n";
const QString M_FS2 = "$132\r\n";
const QString M_FS3 = "$133\r\n";
const QString M_BS1 = "$134\r\n";
const QString M_BS2 = "$135\r\n";
const QString M_BS3 = "$136\r\n";

//************************
//      侧推
//************************

// 侧推停止
const QString S_S  = "#140\r\n";

// 侧推基本动作
const QString S_L1 = "#141\r\n";
const QString S_L2 = "#142\r\n";
const QString S_L3 = "#143\r\n";
const QString S_R1 = "#144\r\n";
const QString S_R2 = "#145\r\n";
const QString S_R3 = "#146\r\n";

// 侧推微调
const QString S_LS1 = "$141\r\n";
const QString S_LS2 = "$142\r\n";
const QString S_LS3 = "$143\r\n";
const QString S_RS1 = "$144\r\n";
const QString S_RS2 = "$145\r\n";
const QString S_RS3 = "$146\r\n";

const QString V_S="#150\r\n";

const QString Turn="#1530D0A\r\n";
const QString SpecialAction="#1600D0A\r\n";

// 投放标志物
const QString ANCHOR_LA = "#8712111111\r\n";
const QString ANCHOR_LB = "#8711111111\r\n";
const QString ANCHOR_RA = "#8711211111\r\n";
const QString ANCHOR_RB = "#8711111111\r\n";


bool SUpdate=false;  //关闭更新显示，默认为不关闭
bool GUpdate=false;

bool DepthOK=false;  //深度调整好标志位
bool AngleOK=false;  //角度调整好标志位
bool OffCenterOK=false;
bool OffCenterOK1=false;

bool image_show=false; //图像显示，默认为不显示
bool OSerial=true;     //串口打开状态，默认为打开
bool Mission_S=false;
bool safety_flag=false;



typedef struct      //发送数据时需要
{
    QString log;
    int log_Length;
}Send_Data;

typedef struct
{
    double Start_Time[8];
    double End_Time[8];
}Mission_Time;
// 各任务开始、结束时间
typedef struct
{
    double Start_Time[3];
    double End_Time[3];
}Scuttle_ship_Time;


Mat frame1, frame2;
NCVision Vision;
VisionInfo VInfo;
#endif // THRUSTER
