#include <SpecialImageProcess/torpedoboard_fun.h>

/*===================================================================
 * 函数名：FindTorpedoBoardHoles
 *
 * 功能：根据输入的轮廓，找出鱼雷切板的四个洞，并按次排序
 *
 * 说明：
 *  一一一一一一一一一一一一一一一一一一
 * 丨                            丨
 * 丨   一一一一一       一一一      丨
 * 丨  丨       丨     丨 3  丨    丨
 * 丨  丨   2   丨     丨    丨    丨
 * 丨  丨       丨      一一一     丨
 * 丨   一一一一一   1             丨
 * 丨     一一一       一一一一一    丨
 * 丨    丨 4  丨     丨       丨  丨
 * 丨    丨    丨     丨   5   丨  丨
 * 丨     一一一       丨       丨 丨
 * 丨                 一一一一一一  丨
 *  一一一一一一一一一一一一一一一一一一
 *  1、输入所有轮廓，vector<vector<Point>> &contours；
 *  2、对所有轮廓从大到小排序；
 *  3、排序后留下5个轮廓，冗余轮廓删除；
 *  4、输入向量contours[0]为轮廓面积最大的向量，被视作最外围轮廓向量
 *  5、比较两个面积较大的孔：对比各自中点的x、y坐标，设定为2、5号轮廓
 *  6、比较两个面积较小的孔：对比各自中点的x、y坐标，设定为3、4号轮廓
 *  7、对contours向量重新赋值；
 *
 * 参数：
 *  vector<vector<Point>> &contours:    输入轮廓向量；
=====================================================================
*/
void FindTorpedoBoardHoles(vector<vector<Point>> &contours)
{
    vector<vector<Point>> newContours(5);
    vector<Point> CenterPoints;
    // 排序，从大到小
    ContoursAreaDownSort(contours);

    // 删除，剩5个轮廓
    DeleteContoursByRank(contours, 5);

    // 计算排序后各轮廓拟合矩形中心点坐标
    CalcuContoursCenterPoint(contours, CenterPoints);

    // 最大轮廓作为1号：newContours[0]
    newContours.at(0) = contours[0];

    // 比较两个大孔
    if (CenterPoints[1].x < CenterPoints[2].x
            && CenterPoints[1].y < CenterPoints[2].y)
    {
        newContours.at(1) = contours[1];
        newContours.at(4) = contours[2];
    }
    else
    {
        newContours.at(1) = contours[2];
        newContours.at(4) = contours[1];
    }

    // 比较两个小孔
    if (CenterPoints[3].x > CenterPoints[4].x
            && CenterPoints[3].y < CenterPoints[4].y)
    {
        newContours.at(2) = contours[3];
        newContours.at(3) = contours[4];
    }
    else
    {
        newContours.at(2) = contours[4];
        newContours.at(3) = contours[3];
    }

    // 对contours向量重新赋值
    contours.clear();
    for(int i = 0; i < 5; i++)
    {
        contours.push_back(newContours[i]);
    }
}

/*===================================================================
 * 函数名：TorpedoBoardPaintRectContours
 *
 * 功能：矩形拟合，绘制输入的轮廓，并对已经根据鱼雷切板序号排序的轮廓进行标号；
 *
 * 说明：
 *  PaintRectContours的扩展，专门用于鱼雷切板轮廓的绘制与编号；
 *
 * 参数：
 *  Mat &src:                           输入图像
 *  Mat &dst:                           输出图像
 *  vector<vector<Point>> &contours:    输入轮廓向量
 *  vector<Point> &CenterPoints:        输入轮廓拟合矩形中心点的向量
=====================================================================
 */
void TorpedoBoardPaintRectContours(Mat &src, Mat &dst,
                                   vector<vector<Point>> &contours,
                                   vector<Point> &CenterPoints)
{
    PaintRectContours(src, dst, contours);
    for(size_t i = 0; i < contours.size(); i++)
    {
//        void putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color,
//        int thickness=1, int lineType=8, bool bottomLeftOrigin=false )
        char c[8];
        itoa(i+1, c, 16);
        putText(dst, c, CenterPoints[i], FONT_HERSHEY_SIMPLEX,
                1, Scalar::all(255), 2);
    }
}
