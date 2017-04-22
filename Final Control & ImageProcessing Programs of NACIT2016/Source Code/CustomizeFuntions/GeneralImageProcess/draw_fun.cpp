#include <GeneralImageProcess/draw_fun.h>

/*===================================================================
 * 函数名：PaintFitLine
 *
 * 功能：绘制拟合曲线
 *
 * 说明：
 *  1、将原图src复制到dst中；
 *  2、生成足够长的线，保证绘制出的图线完全穿过图片；
 *
 * 参数：
 *  Mat src:                输入图像
 *  Mat &dst:               输出目标图像
 *  float *line:            输出拟合直线的信息
 *  const Scalar &color:    绘制图像的颜色信息
 *  int thickness:          绘图直线厚度
=====================================================================
 */
void PaintFitLine(Mat src, Mat &dst, float *line, const Scalar& color, int thickness)
{
    Mat tmp;
    src.copyTo(tmp);
    if (*line == NULL)
    {
        cout<<"Paint Line Fail."<<endl;
        return ;
    }
    //*********************************
    //         绘制点与线
    //*********************************

    // 直线的两个端点
    CvPoint pt1, pt2;

    // 画出线段的两个端点(避免线太短，以线上一个随机点向两侧延伸line[0]*t )
    float t;
    t = (float)(tmp.size().width + tmp.size().height) ;
    pt1.x = cvRound(line[2] - line[0]*t);
    pt1.y = cvRound(line[3] - line[1]*t);
    pt2.x = cvRound(line[2] + line[0]*t);
    pt2.y = cvRound(line[3] + line[1]*t);

    cv::line(tmp, pt1, pt2, color, thickness);
    tmp.copyTo(dst);
}

/*===================================================================
 * 函数名：PaintRotatedRect
 *
 * 功能：绘制旋转矩形
 *
 * 说明：
 *  1、将原图src复制到dst中；
 *  2、在输入图像src的对应位置上绘制旋转矩形；
 *
 * 参数：
 *  Mat src:                输入图像
 *  Mat &dst:               输出目标图像
 *  RotatedRect rect:       输出矩形的信息
 *  const Scalar &color:    绘制图像的颜色信息
 *  int thickness:          绘图直线厚度
=====================================================================
 */
void PaintRotatedRect(Mat src, Mat &dst, RotatedRect rect,
                      const Scalar& color, int thickness)
{
    Mat tmp;
    src.copyTo(tmp);
    Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        line(tmp, vertices[i], vertices[(i+1)%4], color, thickness);
    tmp.copyTo(dst);
}

/*===================================================================
 * 函数名：PaintRect
 *
 * 功能：绘制矩形
 *
 * 说明：
 *  1、将原图src复制到dst中；
 *  2、在输入图像src的对应位置上绘制矩形；
 *
 * 参数：
 *  Mat src:                输入图像
 *  Mat &dst:               输出目标图像
 *  Rect rect:              输出矩形的信息
 *  const Scalar &color:    绘制图像的颜色信息
 *  int thickness:          绘图直线厚度
=====================================================================
 */
void PaintRect(Mat src, Mat &dst, Rect rect, const Scalar& color, int thickness)
{
    Mat tmp;
    src.copyTo(tmp);
    Point2f vertices[4];
    vertices[0] = Point2f(rect.x, rect.y);
    vertices[1] = Point2f(rect.x + rect.width, rect.y);
    vertices[2] = Point2f(rect.x + rect.width, rect.y + rect.height);
    vertices[3] = Point2f(rect.x, rect.y + rect.height);

    for (int i = 0; i < 4; i++)
        line(tmp, vertices[i], vertices[(i+1)%4], color, thickness);
    tmp.copyTo(dst);
}





