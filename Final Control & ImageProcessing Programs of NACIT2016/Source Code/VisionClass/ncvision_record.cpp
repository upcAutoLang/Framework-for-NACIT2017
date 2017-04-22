#include <ncvision.h>

//***********************************
//          记录函数
//***********************************

/*===================================================================
 * 函数：CreateRecordFileName()
 *
 * 说明：
 *  生成流输出文件名，该文件名根据系统时间改变；
 *  默认前缀为VInfoRecord；
 *
 *  生成文件名例：VInfoRecord0514_115037.txt;
/*===================================================================
 */
void NCVision::CreateRecordFileName()
{
    // 根据系统时间更改文件名
    string filename = "VInfoRecord";
    filename += getSysTime();
    filename += ".txt";
    RecordFileName = filename;
}

/*===================================================================
 * 函数：RecordVisionInfo(int StrNum)
 *
 * 说明：
 *  记录VisionInfo，用流输出至文件；
 *
 * 注：必须在CreateRecordFileName()函数之后使用；
/*===================================================================
 */
void NCVision::RecordVisionInfo(int StrNum)
{
    ofstream outRecord;
    outRecord.open(RecordFileName, ios::out | ios::app);

    if(outRecord.is_open())
    {
        // 策略编号
        outRecord << StrNum <<" ";
        //==========================
        //         寻线部分
        //==========================

        // 线状态
        outRecord << VInfo.linestate<<" ";
        // 导引线数组
        for(int i = 0; i < 4; i++)
            outRecord << VInfo.GuideLine[i]<<" ";
        // 导引线角度
        outRecord << VInfo.LineAngle<<" ";
        // ROI区域
        outRecord << VInfo.LineROIArea.x<<" "<<VInfo.LineROIArea.y<<" ";
        outRecord << VInfo.LineROIArea.width<<" "<<VInfo.LineROIArea.height<<" ";

        //==========================
        //         寻门部分
        //==========================

        // 门状态
        outRecord << VInfo.doorstate<<" ";
        // 导引线数组
        for(int i = 0; i < 4; i++)
            outRecord << VInfo.doorline[i]<<" ";
        // 处理缓存轮廓之后的平均矩形轮廓
        outRecord << VInfo.AverageRect.center.x <<" ";
        outRecord << VInfo.AverageRect.center.y <<" ";
        outRecord << VInfo.AverageRect.size.width <<" ";
        outRecord << VInfo.AverageRect.size.height<<" ";
        outRecord << VInfo.AverageRect.angle<<" ";

        // 门轮廓数量
        int numDoorRects = VInfo.DoorRects.size();
        outRecord << numDoorRects<<" ";
        // 门轮廓信息
        for(int i = 0; i < numDoorRects; i++)
        {
            outRecord << VInfo.DoorRects[i].center.x <<" ";
            outRecord << VInfo.DoorRects[i].center.y <<" ";
            outRecord << VInfo.DoorRects[i].size.width <<" ";
            outRecord << VInfo.DoorRects[i].size.height<<" ";
            outRecord << VInfo.DoorRects[i].angle<<" ";
        }
        // 门拟合矩形距屏幕中线偏移量
        outRecord << VInfo.CenterOffsetX<<" ";

        outRecord << endl;
    }

    outRecord.close();
}
