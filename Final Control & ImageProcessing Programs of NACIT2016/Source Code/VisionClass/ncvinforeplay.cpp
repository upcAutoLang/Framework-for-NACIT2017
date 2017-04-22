#include "ncvinforeplay.h"

NCVInfoReplay::NCVInfoReplay()
{
    DownImg = Mat(480, 640, CV_8UC3, Scalar(0));
    FwdImg = Mat(480, 640, CV_8UC3, Scalar(0));
}

/*===================================================================
 * 函数：ReadVisionInfo()
 *
 * 说明：从文件中读取信息集合，并存储在内存中；
 *  1、判断文件流是否打开，若没有打开，则输出错误信息，并返回；
 *  2、若打开文件流，则清除InfoString向量；
 *  3、遍历文件流，获取每行数据，将每行数据存于InfoString向量中；
/*===================================================================
 */
void NCVInfoReplay::ReadVisionInfo()
{
    if(!VInfoSrc.is_open())
    {
        cout<<"Read VInfo Record File Error: Open VisionInfo File Failed!"<<endl;
        return ;
    }
    else
    {
        InfoString.clear();
        while(!VInfoSrc.eof())
        {
            string tmp;
            getline(VInfoSrc, tmp);
            InfoString.push_back(tmp);
        }
    }
}

/*===================================================================
 * 函数：setVInfoFilePath(string path)
 *
 * 说明：设置VisionInfo文件存储路径；
 *
 * 参数：
 *  string path:     记录文件路径；
/*===================================================================
 */
void NCVInfoReplay::setVInfoFilePath(string path)
{
    VInfoPath = path;
}

/*===================================================================
 * 函数：OpenVInfoRecordFile()
 *
 * 说明：打开VisionInfo记录文件；
/*===================================================================
 */
void NCVInfoReplay::OpenVInfoRecordFile()
{
    VInfoSrc.open(VInfoPath);

    if(!VInfoSrc.is_open())
        cout<<"Open VInfo Record File Error: Open VisionInfo File Failed!"<<endl;
}

/*===================================================================
 * 函数：setVisionInfoByString(string str)
 *
 * 说明：从输入的string获取信息，并存入VInfo中；
 *  1、定义stringstream，从输入字符串str中依次读取信息；
 *  2、将读入的信息依次赋值进VInfo中；
 *
 * 参数：
 *  string str:      存储VInfo信息的单行string字符串；
/*===================================================================
 */
void NCVInfoReplay::setVisionInfoByString(string str)
{
    stringstream stream;
    stream.clear();
    stream.str(str);

    // 策略编号
    stream >> StrNum;

    //==========================
    //         寻线部分
    //==========================
    // 线状态
    stream >> VInfo.linestate;
    // 导引线数组
    for(int i = 0; i < 4; i++)
        stream >> VInfo.GuideLine[i];
    // 导引线角度
    stream >> VInfo.LineAngle;
    // ROI区域
    int LineROIX, LineROIY, LineROIWidth, LineROIHeight;
    stream >> LineROIX;
    stream >> LineROIY;
    stream >> LineROIWidth;
    stream >> LineROIHeight;
    VInfo.LineROIArea = Rect(LineROIX, LineROIY, LineROIWidth, LineROIHeight);

    //==========================
    //         寻门部分
    //==========================

    // 门状态
    stream >> VInfo.doorstate;
    // 导引线数组
    for(int i = 0; i < 4; i++)
        stream >> VInfo.doorline[i];

    // 处理缓存轮廓之后的平均矩形轮廓
    float centerX, centerY, width, height, angle;
    stream >> centerX;
    stream >> centerY;
    stream >> width;
    stream >> height;
    stream >> angle;

    // 门轮廓数量
    int numDoorRects;
    stream >> numDoorRects;

    Point2f center(centerX, centerY);
    Size2f RectSize(width, height);
    VInfo.AverageRect = RotatedRect(center, RectSize, angle);

    // 门轮廓信息
    VInfo.DoorRects.clear();
    for(int i = 0; i < numDoorRects; i++)
    {
//            RotatedRect(const Point2f& center, const Size2f& size, float angle);
        float centerX, centerY, width, height, angle;
        stream >> centerX;
        stream >> centerY;
        stream >> width;
        stream >> height;
        stream >> angle;

        Point2f center(centerX, centerY);
        Size2f RectSize(width, height);
        RotatedRect tmpRect = RotatedRect(center, RectSize, angle);
        VInfo.DoorRects.push_back(tmpRect);

    }

    // 门拟合矩形距屏幕中线偏移量
    stream >> VInfo.CenterOffsetX;
}

/*===================================================================
 *          调试程序~~
/*===================================================================
 */
void NCVInfoReplay::test()
{
    setVisionInfoByString(InfoString[0]);
    ReplayOneVInfo();
}

/*===================================================================
 * 函数：ShowOneFrame()
 *
 * 说明：显示前视、俯视单帧图像；
/*===================================================================
 */
void NCVInfoReplay::ShowOneFrame()
{
    imshow("Down Image", DownImg);
    imshow("Forward Image", FwdImg);
}

/*===================================================================
 * 函数：ReplayOneVInfo()
 *
 * 说明：根据策略编号，重绘一帧图像；
/*===================================================================
 */
void NCVInfoReplay::ReplayOneVInfo()
{
    switch(StrNum)
    {
    // 策略：寻线
    case 2:
    {
        int linestate = VInfo.linestate;
        float line[4];
        for(int i = 0; i < 4; i++)
            line[i] = VInfo.GuideLine[i];
        double angle = VInfo.LineAngle;
        Rect rect = VInfo.LineROIArea;

        DownImg = Mat(480, 640, CV_8UC3, Scalar(0));
        FwdImg = Mat(480, 640, CV_8UC3, Scalar(0));
        switch(linestate)
        {
        case 0:
        {
            // 无线，全屏黑；
            break;
        }
        case 1:
        {
            // 线视为点，绘制黄线；
            PaintFitLine(DownImg, DownImg, line, Scalar(255, 255, 0), 2);
            PaintRect(DownImg, DownImg, rect, Scalar(255), 3);
            break;
        }
        case 2:
        {
            // 线，绘制红线；
            PaintFitLine(DownImg, DownImg, line, Scalar(0, 0, 255), 2);
            PaintRect(DownImg, DownImg, rect, Scalar(255), 3);
            break;
        }
        default:
            break;
        }

        break;
    }
    // 策略：寻门
    case 1:
    {
        int doorstate = VInfo.doorstate;
        float line[4];
        for(int i = 0; i < 4; i++)
            line[i] = VInfo.doorline[i];
        RotatedRect AveRect;
        AveRect = VInfo.AverageRect;

        vector<RotatedRect> doorRects;
        int rectsSize = VInfo.DoorRects.size();
        for(int i = 0; i < rectsSize; i++)
        {
            doorRects.push_back(VInfo.DoorRects[i]);
        }
        int OffsetX = VInfo.CenterOffsetX;

        DownImg = Mat(480, 640, CV_8UC3, Scalar(0));
        FwdImg = Mat(480, 640, CV_8UC3, Scalar(0));

        // 当门状态为WHOLEDOOR状态时，绘制缓存平均矩形，绿色；
        if(doorstate == DOOR_WHOLEDOOR)

            PaintRotatedRect(FwdImg, FwdImg, AveRect,
                            Scalar(0, 255, 0), 3);

        // 绘制识别到的轮廓外接矩形，天蓝色；
        for(int i = 0; i < rectsSize; i++)
            PaintRotatedRect(FwdImg, FwdImg, doorRects[i],
                             Scalar(255, 255, 0), 2);

        break;
    }
    default:
        break;

    }
}

/*===================================================================
 * 函数：Replay()
 *
 * 说明：绘制连续图像：
 *  1、计算存储文件中的存储VInfo信息的string字符串数量；
 *  2、遍历这些字符串，每次过程中，先获取字符串中内容赋值给VInfo，再绘制一帧图像，
 * 最后显示一帧图像；
/*===================================================================
 */
void NCVInfoReplay::Replay()
{
    int size = InfoString.size();
    for(int i = 0; i < size; i++)
    {
        setVisionInfoByString(InfoString[i]);
        ReplayOneVInfo();
        ShowOneFrame();
        char c = (char)waitKey(100);
        if (c == 27)
        {
            break;
        }
    }
}
