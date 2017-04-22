#ifndef MISSION_PARAM
#define MISSION_PARAM

#include "global.h"
#include <thruster.h>

// 任务进行时间
Mission_Time mission_time;
Scuttle_ship_Time Ball_Time;  //记录每次撞球的时间

//***************************************************
//           现场可改动参数
//***************************************************

// 任务所在区域
char Mission_Part = 'C';

// A区过门角度
double PartA_Angle = 44.01;

// 是否撞绿球？
bool GreenBall_ReadyOK = false;

// 是否过时间门
bool StyleDoor_ReadyOK = true;

#define MAX_TIME                750

// 开始任务等待时间
#define Start_Waittime          7

//***************************************************
//           现场可改动深度、角度
//***************************************************

// 现场手动对的角度
#define START_ANGLE             90.01

// 存储对门的角度
float Start_Angle = 290.8;
// 设置过门任务深度值
float set_door_depth = 1.2;
// 导引线深度，即撞球深度
float GuidanceDepth = 2.5;

// 撞球任务角度
float Scuttle_Angle = 40.01;
// 撞球结束后，向上浮的深度值
double BallFinishedDepth = 1.5;
// 球与导引路径之间的角度(此处暂定25)
double Angle_Green_Line = 25;

// 风格过门开始某段时间内，是否识别为NEAR
bool Navigate_NEAROK = false;
// 风格过门，直行过门，前进的开始时间
int Navigate_GoAheadStartTime = 0;
// 风格过门，提前结束
bool Navigate_EndEarly = false;

// 风格过门任务深度值
double Navigate_Depth = 2.6;
// 寻线时的Navigate角度值
float Navigate_Angle = 0;

// 投放标志物任务开始时的角度
float Anchor_Angle = 320.01;
// 下潜投放标志物深度
double Anchor_Adj_LaunchDepth = 3;

// 八边形水声初始角度
double Oct_StartAngle = 0;
// 八边形水声需要深度
double Oct_Depth = 2.6;

// 上浮之前的前进时间
int Oct_Float_GoTime = 3;

//***************************************************
//            开始时间相关参数
//***************************************************

// 风格过门开始时间
int Navigate_WholeTime = 0;

// 水声导引投放标志物开始时间
int UnderAc_StartTime = 0;

// 投放标志物开始时间
int Anchor_StartTime = 0;

// 八边形任务开始时间
int Oct_StartTime = 0;

//***************************************************
//             基本不变的参数
//***************************************************

//****************************
//       过门相关数据
//****************************

// 对门时取角度标志
bool first_getangle = true;

// 导引路径角度对准计数器
int count_time = 0;

//****************************
//       撞球相关数据
//****************************


// 是否找到球
bool Find_Ball = false;
// 撞球任务阶段：
// 0：红球未调整，1：红球调整完毕;
// 2：绿球未调整，3：绿球调整完毕;
// 4：黄球未调整，5：黄球调整完毕;
// 6：撞完球调整角度；
// 7：任务刚开始阶段；
// 8：绿球撞后，寻找后面的导引路径；
int Ball_Adjust = BALL_INIT;
// 需要找的球颜色
int Ball_Color = BALL_RED;
bool Three_Ball=false;
// 前进撞球时的俯仰角
double StartPitch;
// 撞球任务最后阶段，是否正在找线
bool Ball_Finish_FindingLine = false;
// 撞球对准判断为真的时间
double Ball_Adjusted_Time = 0;
// 撞球是否对准标志位
bool Ball_Adjusted_OK = false;
// X, Y方向调整完毕标志位
bool x_Adjust = false;
bool y_Adjust = false;
// 发送撞球次数
int Ball_Com_Send_Num = 0;
// 确认撞上球，标志位
bool Ball_Crashed = false;
//
bool Ball_Crashed_IsStarted = false;
//
int Ball_Crashed_StartTime;
//
int Ball_Crashed_EndTime;
// 撞球结束后前进，发现线的标志位
bool Ball_FindLine_OK = false;
// 撞球结束阶段，已经找到线，已经在线正上方
bool Ball_AboveLine_OK = false;
// 撞球结束阶段，已经发送球与线之间的角度
bool Ball_SendBallLineAngle_OK = false;
// 撞球结束阶段，球与线角度对准后的时间
int Ball_BallLineAngle_Time = 0;
// 撞球对准阶段，判定对准时刻的NEAR状态下，被识别球的尺寸
int Ball_Adjusted_RectSize = BALL_NEAR_LIMIT;
// 撞球对准阶段，线性函数计算得到的撞球时间
int Ball_Crash_Time = 7;


//****************************
//       风格过门相关数据
//****************************

bool Navigate_channel_X=false;
bool Navigate_channel_Y=false;
// 180度调整完毕标志位
bool DoorAD = false;
// 180度转向完毕时的角度值
float Navigate_Door_Angle=0;
double Navigate_Time=0;
// 门转向状态，0--还没有转180度，1--已经转了180度
int Door_adjust=0;
int hold_time=0;
// X方向在准许范围保持时间
int keep_time = 0;
// 稳定在180度值的次数
int Door_adjust1=0;
bool NoFindDoor=false;
int Navigate_Turn=0;
int Turn_Times=0;
bool MF2GO=false;
int FindTimes=0;
float set_line_depth=0.8;//设置寻线任务深度值

//****************************
//     水声导引相关数据
//****************************

// 水声方向缓存数组
int Dir[7] = {0, 0, 0, 0, 0, 0, 0};
// 水声角度值缓存数组
float Ac_Angle[7] = {0, 0, 0, 0, 0, 0, 0};
// 用于计算水声方向的次数多少
float Count_amount = 0;
// 水声信号一个大周期之后，结算出来的角度
float GoAngle=0;
bool GoAngleOk=false;
// 每接受到水声信号7次，处理一次数据
int ACtimes = 7;
// 深度水声每接受到水声信号5次，处理一次数据
int Oct_ACtimes = 5;
// 当前时刻水声方向，上一周期水声方向
int AcDir = 11, LastAcDir = 0;
bool FirstTurn=false;
bool Straight=false;
bool AcAngleFinishb=false;
int Straight_Times=0;
bool SendAngleOK=false;
float Send_AcAngle=0;
// 当前角度到达发送角度精度范围内的次数（用于判断角度稳定）
int count_time1=0;
// 水声角度采集完毕
bool AcAngleFinish=false;

//****************************
//     投放标志物相关数据
//****************************

// 投放标志物任务偏移量缺省值
int Anchor_Offset_Default = -1000;
// 投放标志物任务阶段：
// 0：看到标志物盒子，移动到正中；
// 1：下潜到投放深度，调整投放角度，移动到正中；
// 2：调整左投放装置，发射；
// 3：调整右投放装置，发射；
// 4：任务结束，调整到任务结束深度，任务结束角度；
int Anchor_Adjust = ANCHOR_INIT;

// 初始状态下，目标箱子或盖子的X, Y方向偏移量
int Anchor_Init_OffsetX = Anchor_Offset_Default;
int Anchor_Init_OffsetY = Anchor_Offset_Default;
// 初始状态下，X, Y方向调整完毕
bool Anchor_Init_AdjustX_OK = false;
bool Anchor_Init_AdjustY_OK = false;

// 下潜状态下，目标箱子或盖子的X, Y方向偏移量
int Anchor_Adj_OffsetX = Anchor_Offset_Default;
int Anchor_Adj_OffsetY = Anchor_Offset_Default;
// 下潜状态下，X, Y方向调整完毕
bool Anchor_Adj_AdjustX_OK = false;
bool Anchor_Adj_AdjustY_OK = false;
// 下潜状态下，X, Y偏移量调整完毕后，调整角度前的角度初始值
double Anchor_Adj_OffsetXY_InitAngle = 40.01;

// INIT阶段结束时间
int Anchor_Init_OK_Time = 0;
// 投放标志物深度延迟发送计数器
int Anchor_Depth_ComSendNum = 0;
// 下潜阶段深度调整完毕标志位
bool Anchor_Adj_DepthOK = false;
// 下潜阶段角度调整完毕标志位
bool Anchor_Adj_AngleOK = false;

// 下潜投放标志物角度
double Anchor_Adj_LaunchAngle = 300.01;
// 下潜阶段角度发送延迟计数器
int Anchor_Adj_AngleCount = 0;

// ADJ阶段结束时间
int Anchor_Adj_OK_Time = 0;
// 准备发射左标志物状态下，X, Y方向调整完毕
bool Anchor_LEFT_AdjustX_OK = false;
bool Anchor_LEFT_AdjustY_OK = false;
// 左标志物投放完毕
bool Anchor_LEFT_Launched = false;

// 准备发射左标志物状态下，X, Y方向调整完毕
bool Anchor_RIGHT_AdjustX_OK = false;
bool Anchor_RIGHT_AdjustY_OK = false;
// 右标志物投放完毕
bool Anchor_RIGHT_Launched = false;

// 左目标对准时间
int Anchor_LEFT_OK_Time = 0;
// 右目标对准时间
int Anchor_RIGHT_OK_Time = 0;

//****************************
//     八边形浮起相关数据
//****************************


// 基本到达八边形下方标志位
bool Oct_BelowOct = false;
// 八边形水声方向缓存数组
int Oct_Dir[7] = {0, 0, 0, 0, 0, 0, 0};
// 八边形水声角度值缓存数组
float Oct_Ac_Angle[7] = {0, 0, 0, 0, 0, 0, 0};
// 八边形水声角度采集完毕标志位
bool Oct_AngleOK = false;
// 八边形水声信号采集次数
int Oct_Counttime = 0;
// 用于计算八边形水声方向的次数多少
float Oct_Count_amount = 0;
// 当前时刻八边形水声方向，上一周期八边形水声方向
int Oct_AcDir = 11, Oct_LastAcDir = 11;
// 水声信号一个大周期之后，结算出来的角度
float Oct_DepthAngle = 0;
bool Oct_DepthAngleOk = false;
// 开始计时，3秒后浮起
int Oct_FloatStart_Time = 0;
bool Oct_FloatStart_OK = false;
//// 上周期垂直角度
//double Oct_LastAngle = 0;

bool Oct_SwitchAC = false;
int Oct_BeforeSwitchTime = 0;

// 八边形接收到数据个数
int Oct_DataNum = 0;

// 竖直阵，当前时刻八边形水声方向，上一周期八边形水声方向
int Oct_VerAcDir = 11, Oct_VerLastAcDir = 11;

// 竖直阵单独作用标志位
bool Oct_VerWork = false;
//
bool Oct_FwdDone = false;

// 八边形水声角度值起作用的值
vector<double> Oct_Fun_Angle;

//
bool Oct_VerWork_Work = true;

// 八边形浮起
bool Oct_FloatUP_Done = false;


//****************************
//     红色硬币
//****************************

double Oct_CurAngle = -1, Oct_LastAngle = -1, Oct_LastBeforeAngle = -1;

//****************************
//     任意水声数据
//****************************

bool IsAimToMission = false;

// 任意pinger情况下，判断当前开的pinger是否为八边形pinger
bool Random_Oct = true;

#endif // MISSION_PARAM

