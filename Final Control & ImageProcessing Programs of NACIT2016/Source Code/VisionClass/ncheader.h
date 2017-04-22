#ifndef NCHEADER
#define NCHEADER

#include "highgui.h"
#include <vector>
#include <SupportFunctions/string_fun.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

//********************
//   各种判断依据
//********************

// 判断导引路径点、线判定阈值
//// 大水池用值
//#define     LINE_SIZE_LIMIT               20
// 游泳池用值
#define     LINE_SIZE_LIMIT             40


// 判断导引路径的远近
#define     DOOR_FAR_LIMIT              45
// 判断有、无导引路径
#define     DOOR_LINE_SIZELIMIT         10
// 寻门过程中，要求最大轮廓的最小值
#define     DOOR_CONTOURMAXLIMIT        20
// 寻门过程中，2、3轮廓尺寸比值下限
#define     DOOR_LENGTHRATIO_2_3        3
// 风格过门过程中，要求最大轮廓的最小值
#define     STYLE_CONTOURMAXLIMIT       10
//
#define     STYLE_ERASEAREA_RATIO       200

// 判断球距离等级的条件
#define     BALL_NOBALL_LIMIT           10
#define     BALL_FAR_LIMIT              15
#define     BALL_NEAR_LIMIT             28
#define     BALL_ATTACH_LIMIT           100
#define     BALL_ATTACHED_LIMIT         300

// 判断风格过门距离等级的条件
#define     STYLE_NODOOR_LIMIT          5
#define     STYLE_MAXFILTER_LIMIT       5
#define     STYLE_SMALLFILTER_LIMIT     15
#define     STYLE_FAR_LIMIT             10
#define     STYLE_NEAR_LIMIT            90

// 判断鱼雷切板相关状态的条件
#define     TORPEDO_HOLESIZE_LIMIT      20
#define     TORPEDO_HOLESIZE_RATIO      10
#define     TORPEDO_REDEDGHES_LIMIT     50
#define     TORPEDO_ATTACH_ONE_LIMIT    250
#define     TORPEDO_ISFACING_RATIO      1.2
// 判断鱼雷切板距离等级的条件
#define     TORPEDO_NODOOR_LIMIT        20
#define     TORPEDO_FAR_LIMIT           40
#define     TORPEDO_NEAR_LIMIT          150
#define     TORPEDO_ATTACH_LIMIT        400

// 判断投放标志物任务中的条件
#define     ANCHOR_BOXFILTER_LIMIT      10
#define     ANCHOR_COVERFILTER_LIMIT    15
#define     ANCHOR_AIMFILTER_LIMIT      30
// 判断投放标志物任务中的距离等级条件
#define     ANCHOR_FAR_LIMIT            20
#define     ANCHOR_NEAR_LIMIT           80

// 红色硬币
#define     REDCOIN_LIMIT               40
//********************
//     各种状态
//********************

/* 导引路径状态 */
enum
{
    GUIDE_NOLINE = 0,
    GUIDE_POINT = 1,
    GUIDE_LINE = 2
};

/* 过门状态 */
enum
{
    DOOR_NODOOR = 0,
    DOOR_HALFLEFT = 1,
    DOOR_HALFRIGHT = 2,
    DOOR_WHOLEDOOR = 3,
    DOOR_TOPONLY = 4,
    DOOR_OTHER = 5
};

/* 过门状态中导引路径状态 */
enum
{
    DOOR_NOLINE = 0,
    DOOR_FOUNDLINE = 1
};

/* 过门状态中导引路径距离航行器是否较远 */
enum
{
    DOOR_NEAR = 0,
    DOOR_FAR = 1
};

/* 球的状态 */
enum
{
    BALL_FAR = 0,
    BALL_NEAR = 1,
    BALL_ATTACH = 2,
    BALL_ATTACHED_RED = 3,
    BALL_ATTACHED_YELLOW = 4,
    BALL_ATTACHED_GREEN = 5
};

/* 球存在与否 */
enum
{
    BALL_NOBALL = 0,
    BALL_HAVE = 1
};

/* 球的颜色 */
enum
{
    BALL_RED = 0,
    BALL_YELLOW,
    BALL_GREEN
};

/* 风格过门距离状态 */
enum
{
    STYLE_FAR = 0,
    STYLE_NEAR = 1
};

/* 风格过门形状状态 */
enum
{
    STYLE_NODOOR = 0,
    STYLE_HALFLEFT = 1,
    STYLE_HALFRIGHT = 2,
    STYLE_WHOLEDOOR = 3,
    STYLE_BOTTOMONLY = 4,
    STYLE_OTHER = 5
};

/* 风格过门角度状态 */
enum
{
    STYLE_CENTER = 0,
    STYLE_LEFT,
    STYLE_RIGHT,
    STYLE_UNKNOWNANGLE
};

/* 鱼雷切板距离状态 */
enum
{
    TORPEDO_NOBOARD = 0,
    TORPEDO_FAR,
    TORPEDO_NEAR,
    TORPEDO_ATTACH_NOHOLE,
    TORPEDO_ATTACH_ONE,
    TORPEDO_ATTACH_CROSS,
    TORPEDO_ATTACH_LEFT,
    TORPEDO_ATTACH_RIGHT,
    TORPEDO_ATTACH_TOP
};

/* 投放标志物状态 */
enum
{
    ANCHOR_NOANCHOR = 0,
    ANCHOR_COVERONLY,
    ANCHOR_AIMBOX
};

/* 投放标志物距离状态 */
enum
{
    ANCHOR_FAR = 0,
    ANCHOR_NEAR
};

/* 目标位置状态 */
enum
{
    TARGET_NOTARGET = 0,
    TARGET_LEFT,
    TARGET_RIGHT,
    TARGET_CENTER
};


/*===========================================================================
 * 类：Target
 *
 * 说明：存储单个目标区域的信息
/*===========================================================================
*/
class Target
{
public:
    // Target构造函数
    Target()
    {
        center = Point2f(-1, -1);
        width = -1;
        rect = Rect(-1, -1, -1, -1);
        TarOffsetX = -1;
        TarOffsetY = -1;
        IsInTarget = false;
    }
    Target(Point2f CenterPoint, int wid = 90)
    {
        center = CenterPoint;
        width = wid;
        rect = Rect(center.x - wid / 2,
                    center.y - wid / 2,
                    wid,
                    wid);
        TarOffsetX = -1;
        TarOffsetY = -1;
        IsInTarget = false;
    }

    // 给出定点，计算其X, Y偏差，并判断该点是否在Target内
    void TargetOffset(Point2f Point)
    {
        TarOffsetX = Point.x - center.x;
        TarOffsetY = Point.y - center.y;
        if(abs(TarOffsetX) < width / 2
                && abs(TarOffsetY) < width / 2)
            IsInTarget = true;
        else
            IsInTarget = false;
    }

    // Target的ROI范围
    Rect rect;
    // Target中心点
    Point2f center;
    // Target宽度
    int width;
    // 给定点距离Target的X, Y方向偏移量
    int TarOffsetX;
    int TarOffsetY;
    // 是否给定点在Target范围中
    bool IsInTarget;
};


/*===========================================================================
 * 类：Hole
 *
 * 说明：存储单个孔洞的信息
/*===========================================================================
*/
class Hole
{
public:
    // Hole构造函数
    Hole()
    {
        HoleHave = false;
        HoleRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
        Center = Point2f(320, 240);
        TargetState = TARGET_NOTARGET;

        CenterTarget = Target(Center, 40);
        LeftTarget = Target(Point2f(320*2 * 0.4, 240*2 * 0.45), 40);
        RightTarget = Target(Point2f(320*2 * 0.6, 240*2 * 0.50), 40);
    }
    Hole(Size2f size)
    {
        HoleHave = false;
        HoleRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
        Center = Point2f(size.width / 2, size.height / 2);
        TargetState = TARGET_NOTARGET;

        CenterTarget = Target(Center, 40);
        LeftTarget = Target(Point2f(size.width * 0.4, size.height * 0.45), 40);
        RightTarget = Target(Point2f(size.width * 0.6, size.height * 0.50), 40);
    }

    // 计算该Hole是否在三个感兴趣目标之中
    int CalHoleState(RotatedRect rect)
    {
        HoleRect = rect;
        Center = rect.center;

        LeftTarget.TargetOffset(Center);
        RightTarget.TargetOffset(Center);
        CenterTarget.TargetOffset(Center);

        if(!LeftTarget.IsInTarget
                && !RightTarget.IsInTarget
                && !CenterTarget.IsInTarget)
            TargetState = TARGET_NOTARGET;
        else
        {
            if(LeftTarget.IsInTarget)
                TargetState = TARGET_LEFT;
            else if(RightTarget.IsInTarget)
                TargetState = TARGET_RIGHT;
            else if(CenterTarget.IsInTarget)
                TargetState = TARGET_CENTER;
        }
        return TargetState;
    }

    // Hole成员变量
    bool HoleHave;
    RotatedRect HoleRect;
    Point2f Center;
    Target LeftTarget;
    Target RightTarget;
    Target CenterTarget;
    int TargetState;
};




/*===========================================================================
 * 结构体：VisionInfo
 *
 * 说明：视觉处理得到的信息集合
/*===========================================================================
*/
struct VisionInfo
{
    //===============================================
    //              寻线相关数据
    //===============================================

    // 线状态：0--没有直线；1--线视为点；2--线；
    int linestate;
    // 直线信息
    float GuideLine[4];
    // 直线角度
    double LineAngle;
    // 直线感兴趣区域
    Rect LineROIArea;
    // 导引路径ROI区域X方向上的偏移量，负值左偏，正值右偏
    int LineOffsetX;
    // 导引路径ROI区域Y方向上的偏移量，负值左偏，正值右偏
    int LineOffsetY;
    // 判断导引路径是否在线正上方
    bool IsAboveLine;

    //===============================================
    //              过门相关数据
    //===============================================

    // 门状态：
    // 0、没有门；1、 |--型；2、  --|型；
    // 3、|--|型；4、--型；
    int doorstate;
    // 过门中看到的导引路径状态(ID: In Door)
    // 0、没看到线；1、看到线
    int lineIDstate;
    // 过门中看到的导引路径是否较远
    // 0、近；1、远；
    int lineIDFar;
    // 导引路径感兴趣区域
    Rect LineIDROI;
    // 导引路径ROI的X方向中心偏移量，负值左偏，正值右偏；
    int LineIDOffsetX;

    // 中线信息
    float doorline[4];
    // 分析后的轮廓向量
    vector<RotatedRect> DoorRects;
    // 经过缓存矩形处理后的矩形
    RotatedRect AverageRect;
    // X方向中心偏移量，负值左偏，正值右偏；
    int CenterOffsetX;

    //===============================================
    //              撞球相关数据
    //===============================================

    // 寻找的球的颜色
    int BallColor;
    // 球距离状态
    int ballsstate;
    // 视野中红黄绿球存在状况
    int RedHave;
    int YellowHave;
    int GreenHave;
    // 视野中红黄绿球的位置（对应的Have状态不为0时起作用）
    RotatedRect RedRect;
    RotatedRect YellowRect;
    RotatedRect GreenRect;
    // 视野中红黄绿球与屏幕中心的X, Y方向偏移量（对应的Have状态不为0时起作用）
    int RedOffsetX, RedOffsetY;
    int YellowOffsetX, YellowOffsetY;
    int GreenOffsetX, GreenOffsetY;
    // 是否已经触碰过红、绿球
    bool RedAttached;
    bool GreenAttached;

    //===============================================
    //              风格过门相关数据
    //===============================================

    // 风格门距离状态
    int StyleDoorState;
    // 分析后的风格过门轮廓向量
    vector<RotatedRect> StyleDoorVecRects;
    // 视野中风格过门拟合矩形
    RotatedRect StyleDoorRect;
    // 风格过门拟合矩形X方向中心偏移量，负值左偏，正值右偏；
    int StyleCenterOffsetX;
    // 风格过门拟合矩形Y方向中心偏移量，负值上偏，正值下偏；
    int StyleCenterOffsetY;
    // 风格门状态
    int StyleDoorSeenState;
    // 风格门角度状态
    int StyleDoorAngleState;

    //===============================================
    //              鱼雷切板相关数据
    //===============================================

    // 鱼雷切板距离等级与状态
    int TorBoardState;
    // 鱼雷切板拟合矩形
    RotatedRect TorBoardRect;
    // 鱼雷切板中心距屏幕X方向偏移量，负值左偏，正值右偏；
    int TorBoardOffsetX;
    // 鱼雷切板中心距屏幕Y方向偏移量，负值上偏，正值下偏；
    int TorBoardOffsetY;
    // 是否正对鱼雷切板
    bool IsFacing;
    // 左上，左下，右上，右下孔洞（用于NEAR状态）
    Hole ULHole, DLHole, URHole, DRHole;
    // 大小孔洞（用于ATTACH相关状态）
    Hole SmallHole, BigHole;
    // 当前目标孔洞
    Hole CurrentHole;

    //===============================================
    //            投放标志物相关数据
    //===============================================

    // 投放标志物任务状态
    int AnchorState;
    // 投放标志物任务距离状态
    int AnchorDistState;
    // 目标孔洞
    Hole AimHole;
    // 盖子孔洞
    Hole CoverHole;
    // 黄色图案孔洞
    Hole PatternHole;
    // 箱子外围外接矩形
    RotatedRect BoxRect;

    //===============================================
    //            红色硬币相关数据
    //===============================================

    bool RedCoinHave;
    RotatedRect RedCoinRect;
};

#endif // NCHEADER

