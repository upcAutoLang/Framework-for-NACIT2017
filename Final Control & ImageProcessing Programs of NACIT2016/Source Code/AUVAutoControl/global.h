#ifndef GLOBAL_H
#define GLOBAL_H
//定义各种全局变量
#define CONTROLTIME 500 //控制周期 单位：ms

//是否开启调试输出模式
#define DEBUGSWITCH true

//深度预设值
#define EC_Length (39-1) //接受下位机数据长度,共39位，减去结束符
#define AC_Length  (14-2)
//调整角度时的精度
#define Proper_Angle            4
//调整深度时的精度
#define Proper_Depth            0.2
//调整时偏移量的精度
#define Proper_OffCenter        60
#define Proper_BALL             30
#define LargeOffset_BALL        100
#define Line_Proper_OffCenter   20
#define ProperOffset_ANCHOR     50
#define LargeOffset_ANCHOR      100

#define Large_AcDepth           25


// 默认撞球已经完成的时间
#define Ball_AttachedTime       15
// 默认撞球前后俯仰角变化量的阈值
#define Ball_AtaachedPitch      5

////大气压强
//#define Atmos 0.0975

// 06.20 大气压强
#define Atmos 0.0980

// 深度误差值
#define Depth_Error             0.0

//通信端口值
#define COMUnderAc1 "COM12"  //水声通信端口1
#define COMUnderAc2 "COM13"  //水声通信端口2
#define COMExecute "COM7"  //下位机通信端口

#define MAXTIME 30 //最大时间，单位：min，超过此时间浮起

#define DOTASKBALL true //撞球任务开关
#define DOTASKTIMG true  //时间门任务开关
#define DOTASKEMIT true  //发射任务开关
#define DOTASKSUPY true  //补给任务开关
#define DOTASKOCTU true  //八角形回收
#define DOTASKBODY true  //物体回收

#define Left 1
#define Right 2

enum Task
{
    START,
    THROUGHDOOR,
    SCUTTLE_SHIP,
    NAVIGATE_CHANNEL,
    WEIGHT_ANCHOR,
    SET_COURSE,
    BURY_TREASURE,
    BODYBACK,
    UNDERAC,
    RANDOMAC
};

enum
{
    BALL_RED_NOADJ,
    BALL_RED_ADJ,
    BALL_GREEN_NOADJ,
    BALL_GREEN_ADJ,
    BALL_YELLOW_NOADJ,
    BALL_YELLOW_ADJ,
    BALL_ANGLE_ADJ,
    BALL_INIT,
    BALL_FINISH,
    BALL_FINDLINE
};

// 投放标志物阶段
enum
{
    ANCHOR_INIT,
    ANCHOR_ADJ,
    ANCHOR_LEFT,
    ANCHOR_RIGHT,
    ANCHOR_FINISH
};


Task iamtask;
int missionflag[7]={0,0,0,0,0,0,0};//任务完成标志
#endif // GLOBAL_H

