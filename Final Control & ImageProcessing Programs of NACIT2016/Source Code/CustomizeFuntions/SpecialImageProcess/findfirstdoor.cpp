#include <SpecialImageProcess/findfirstdoor.h>

/*===================================================================
 * 函数名：FindDoorRect
 *
 * 功能：寻找第一个门，返回一个识别矩形；
 *
 * 说明：
 *  1、轮廓按照面积大小进行降序排序；
 *  2、删除剩余5个轮廓；
 *  3、判断自然光分量：若5个轮廓中有外接矩形的中心在图像上限之上，则被视为自然光分量，剔除；
 *  4、比较降序排列的第2、3个轮廓拟合矩形的面积，若第三个轮廓面积远小于第二个，则认为只识别出两个轮廓；
 *  5、拟合出识别矩形；
 *
 * 参数：
 *  vector<vector<Point> > &contours:       输入轮廓向量
 *  Mat src:                                输入图像，用于获取图像尺寸信息
 *  RotatedRect &Rect:                      拟合出的识别矩形
=====================================================================
*/
void FindDoorRect(vector<vector<Point> > &contours, Mat src, RotatedRect &Rect)
{
    int height = src.size().height;
    // 所有轮廓根据面积进行冒泡法降序排序，并输出所有检测出来的轮廓面积
    ContoursAreaDownSort(contours);
    int contoursize = contours.size();

    // 为图像设限，在该条线之上出现的轮廓被视为自然光分量轮廓，被剔除；
    int UpLimit = height / 3;

    vector<RotatedRect> box;
    if(contoursize > 5)
    {
        // 先留下5个最大的轮廓
        DeleteContoursByRank(contours, 5);

        for(int i = 0; i < 5; i++)
        {
            RotatedRect tmpRect;
            Mat pointsf;
            Mat(contours[i]).convertTo(pointsf, CV_32F);
            tmpRect = minAreaRect(pointsf);
            box.push_back(tmpRect);
        }

        // 检查五个轮廓的外接矩形，若外接矩形的中心在图像上限之上，则被视为自然光分量，剔除之；
        for(int i = 5 - 1; i > 0; --i)
        {
            if(box[i].center.y < UpLimit)
            {
                contours.erase(contours.begin() + i);
            }
        }
    }

    // 更新此时的轮廓数量；
    contoursize = contours.size();
    // 比较降序排列的第2、3个轮廓拟合矩形的面积，若第三个轮廓面积远小于第二个，则认为只识别出两个轮廓；
    if(contoursize >= 3)
    {
        DeleteContoursByRank(contours, 3);

        RotatedRect SecondRect, ThirdRect;
        double SecondRectArea, ThirdRectArea;

        Mat pointsf;
        // 第i个轮廓中，格式转为32位Float，存于Mat类的pointsf中
        // contours[i]为向量，转换为1*n的Mat
        Mat(contours[1]).convertTo(pointsf, CV_32F);
        SecondRect = minAreaRect(pointsf);
        Mat(contours[2]).convertTo(pointsf, CV_32F);
        ThirdRect = minAreaRect(pointsf);

        SecondRectArea = SecondRect.size.width * SecondRect.size.height;
        ThirdRectArea = ThirdRect.size.width * ThirdRect.size.height;

        if(SecondRectArea / 5 > ThirdRectArea)
            contours.erase(contours.end()-1);
    }
    ContoursMinAreaRect(contours, Rect);
}
