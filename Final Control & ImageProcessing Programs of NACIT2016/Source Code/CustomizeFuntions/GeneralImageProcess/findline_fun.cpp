#include <GeneralImageProcess/findline_fun.h>


/*===================================================================
 * 函数名：BinaryFitLine
 *
 * 功能：二值化图像生成拟合直线，并返回一个角度值
 *
 * 说明：
 *  1、输入一个单通道图像，使用迭代器，遍历单通道图像所有像素，并按照输入的分辨率
 *  （若分辨率太高，拟合直线处理量会过大，计算速度变慢）抽样记录像素值为255的点，
 *  并将它们的x、y坐标并存入两个vector中；
 *  2、将已经遍历过的点的值赋给CvPoint数组，数目为count；
 *  3、将CvPoint数组作为输入参数，使用cvFitLine函数拟合曲线，曲线相关值输出到
 *  line数组中；
 *  4、根据计算出line的参数，计算出直线的角度；并规定：
 *      /偏：斜率为负；
 *      \偏：斜率为正；
 *      丨 ：斜率为零；
 *  5、若计算出的白点数量count小于分辨率res，则说明没有拟合出直线；
 *
 * 参数：
 *  Mat &src:           输入图像
 *  float *line:        输出拟合直线的信息
 *  double &angle:      输出拟合曲线的角度（角度制）
 *  bool &noLine:       是否能够拟合出直线，0表示没有拟合出直线，1表示拟合出直线
 *  int res:            遍历图像时，采样的分辨率
=====================================================================
 */
void BinaryFitLine(Mat &src, float *line, double &angle, bool &noLine, int res)
{
    if(src.type() != CV_8U)
    {
        cout<<"Wrong Pixel Format: "<< src.type();
        return ;
    }

    int count = 0;
    res = res - 1;
    int resCount = 0;
    vector<int> PointXVec;
    vector<int> PointYVec;

    // 得到起始位置的迭代器
    Mat_<uchar>::iterator it =
            src.begin<uchar>();
    // 得到起始位置的迭代器
    Mat_<uchar>::iterator itend =
            src.end<uchar>();

    // 遍历所有像素
    for(count = 0, resCount = 0; it != itend; ++it)
    {
        // 处理每个像素
        if( (*it == 255) && resCount == res )
        {
            PointXVec.push_back(it.pos().x);
            PointYVec.push_back(it.pos().y);

            ++count;
            resCount = 0;
        }
        else if ( (*it == 255) && resCount != res)
        {
            resCount++;
        }
        // 处理每个像素完成
    }


    // 将已经遍历过的点的值赋给CvPoint数组，数目为count
    CvPoint* points = (CvPoint*)malloc( count * sizeof(points[0]));
    // 点集, 存储count个随机点points
    CvMat pointMat = cvMat( 1, count, CV_32SC2, points );
//        cout<<pointMat.data.i;


    for(int i = 0; i < count; i++)
    {
        points[i].x = PointXVec[i];
        points[i].y = PointYVec[i];
    }

    if (count < res)
    {
        noLine = true;
        return ;
    }
    // 如果筛选出这个点，则做直线拟合
    if (count > 0)
    {
        // 曲线拟合
        cvFitLine(&pointMat,    // 存于Mat的点集
                  CV_DIST_L2,   // 距离评估方法
                  0,            // 距离类型参数
                  0.001,        // 半径精度
                  0.001,        // 角度精度
                  line          // 输出直线，浮点型指针
                  );

        // 计算角度值
        // /偏为负，\偏为正，丨为零
        double tan_angle = double(line[0] / line[1]);
        angle = atan(tan_angle);
        angle = angle * 180 / CV_PI;
//      angle = -angle;

//        cout<<line[0]<<" "<<line[1]<<" "<<line[2]<<" "<<line[3]<<endl;
//        cout<<"Angle: "<<angle<<endl;
    }

    free(points);
}

/*===================================================================
 * 函数名：BinaryFitLine2
 *
 * 功能：二值化图像生成拟合直线，并返回一个角度值
 *
 * 说明：
 *  1、输入一个单通道图像，查找所有轮廓，按面积大小对轮廓降序排序，删除只剩一个轮廓；
 *     （若没有找到轮廓，则输出相关错误信息，退出函数）
 *  2、拟合生成ROI正矩形与轮廓最小外接矩形；
 *  3、生成一个只包含ROI区域的二值化矩阵，该矩阵与输入矩阵等宽等高；
 *  4、根据轮廓最小外接矩形的长宽大小及比值，判断识别到的是点、线、或是轮廓太小忽略不计；
 *  5、如果识别为点、线，则计算line的相关数值；并根据计算出line的参数，计算出直线的角度；并规定：
 *      /偏：斜率为负；
 *      \偏：斜率为正；
 *      丨 ：斜率为零；
 *
 * 参数：
 *  Mat src:            输入图像
 *  float *line:        输出拟合直线的信息
 *  double &angle:      输出拟合曲线的角度（角度制）
 *  bool &noLine:       是否能够拟合出直线，0表示没有拟合出直线，1表示拟合出直线
 *  int &lineOrpoint:   点线判断；
 *                      0——轮廓太小，忽略不计；
 *                      1——点；
 *                      2——线；
=====================================================================
 */
void BinaryFitLine2(Mat src, float *line, double &angle, bool &noLine, int &lineOrpoint)
{
    if(src.type() != CV_8U)
    {
        cout<<"Wrong Pixel Format: "<< src.type();
        return ;
    }

    Mat srcCopy;
    src.copyTo(srcCopy);

    vector<vector<Point>> DownContours;

    findContours(srcCopy, DownContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    // 所有轮廓根据面积进行冒泡法降序排序，并输出所有检测出来的轮廓面积
    ContoursAreaDownSort(DownContours);
    int contoursize = DownContours.size();

    if(contoursize <= 0)
    {
        cout<<"Find Line error: Contours less than 0."<<endl;
        //*********************
        //  也许需要数据清除
        //*********************

        return ;
    }

    DeleteContoursByRank(DownContours, 1);
//    PaintRectContours(img, PaintLine, DownContours);
    Mat pointsf;
    Mat(DownContours[0]).convertTo(pointsf, CV_32F);

    Rect ROIRect;
    RotatedRect LineRotRect;
    ROIRect = boundingRect(pointsf);
    LineRotRect = minAreaRect(pointsf);

    Mat imgCut;
//        imgBinLine.copyTo(imgCut);
    imgCut.create(srcCopy.size(), CV_8U);

    for (int i = 0; i < imgCut.rows; ++i)
    {
        for(int j = 0; j < imgCut.cols; ++j)
        {
            if(j >= ROIRect.x && j <= (ROIRect.x + ROIRect.width)
                    && i >= ROIRect.y && i <= (ROIRect.y + ROIRect.height))
                imgCut.at<uchar>(i, j) = src.at<uchar>(i, j);
            else
                imgCut.at<uchar>(i, j) = 0;
        }
    }

    double linelen, linewid;
    linelen = LineRotRect.size.height;
    linewid = LineRotRect.size.width;
    double tmp;
    if(linelen < linewid)
    {
        tmp = linelen;
        linelen = linewid;
        linewid = tmp;
    }

    double ratio = linelen / linewid;

    if(ROIRect.width <= 25 && ROIRect.height <= 25)
    {
        noLine = true;
        lineOrpoint = 0;

        //*********************
        //  也许需要数据清除
        //*********************
    }
    else
        noLine = false;
    if(!noLine)
    {
        // 被当做点，则求点与屏幕中线的夹角
        if(ratio < 1.5 && ratio > 1 / 1.5)
        {
            lineOrpoint = 1;
            CvPoint* points = (CvPoint*)malloc( 2 * sizeof(points[0]));
            // 点集, 存储count个随机点points
            CvMat pointMat = cvMat( 1, 2, CV_32SC2, points );

            int RectX = (ROIRect.x + ROIRect.width) / 2;
            int RectY = (ROIRect.y + ROIRect.height) / 2;
            int CenterX = imgCut.size().width / 2 - 1;
            int CenterY = imgCut.size().height / 2 - 1;
            points[0].x = RectX;
            points[0].y = RectY;
            points[1].x = CenterX;
            points[1].y = CenterY;
//                twoPoint.push_back(p1);
//                twoPoint.push_back(p2);

            cvFitLine(&pointMat,    // 存于Mat的点集
                      CV_DIST_L2,   // 距离评估方法
                      0,            // 距离类型参数
                      0.001,        // 半径精度
                      0.001,        // 角度精度
                      line          // 输出直线，浮点型指针
                      );

            // 计算角度值
            // /偏为负，\偏为正，丨为零
            double tan_angle = double(line[0] / line[1]);
            angle = atan(tan_angle);
            angle = angle * 180 / CV_PI;
            angle = -angle;

            free(points);
        }
        // 被当做线
        else
        {
            lineOrpoint = 2;
            BinaryFitLine(imgCut, line, angle, noLine, 3);

        }
    }
}

/*===================================================================
 * 函数名：BinaryROIFitLine
 *
 * 功能：ROI内，二值化图像生成拟合直线，并返回一个角度值
 *
 * 说明：
 *  1、检查输入是否为单通道图像，非单通道图像时输出错误信息；
 *  2、ROI区域遍历，并存储Point向量；
 *  3、根据存储的Point向量拟合直线；
 *  4、计算拟合直线的角度信息；
 *  5、如果识别为点、线，则计算line的相关数值；并根据计算出line的参数，计算出直线的角度；并规定：
 *      /偏：斜率为负；
 *      \偏：斜率为正；
 *      丨 ：斜率为零；
 *
 * 参数：
 *  Mat src:                输入图像
 *  Rect rect:              ROI区域
 *  float *line:            拟合曲线信息
 *  double &angle:          拟合曲线角度值
=====================================================================
 */
void BinaryROIFitLine(Mat src, Rect rect, float *line, double &angle, int res)
{
    if(src.type() != CV_8U)
    {
        cout<<"ROI Fit Line Error: Pixel Format: "<< src.type()<<" is Wrong!"<<endl;
        return ;
    }

    Mat srcCopy;
    src.copyTo(srcCopy);
    int count = 0;
    res = res - 1;
    int resCount = 0;
    vector<int> PointXVec;
    vector<int> PointYVec;

    for(int i = rect.x; i <= (rect.x + rect.width); i++)
    {
        for(int j = rect.y; j <= (rect.y + rect.height); j++)
        {
            if(srcCopy.at<uchar>(j, i) == 255 && resCount == res)
            {
                PointXVec.push_back(i);
                PointYVec.push_back(j);

                ++count;
            }
            else if(srcCopy.at<uchar>(j, i) == 255 && resCount != res)
                resCount++;
        }
    }

    // 将已经遍历过的点的值赋给CvPoint数组，数目为count
    CvPoint* points = (CvPoint*)malloc( count * sizeof(points[0]));
    // 点集, 存储count个随机点points
    CvMat pointMat = cvMat(1, count, CV_32SC2, points);
//    CvMat *pointMat = cvCreateMat(1, count, CV_32SC2);

    for(int i = 0; i < count; i++)
    {
        points[i].x = PointXVec[i];
        points[i].y = PointYVec[i];
    }

    // 如果筛选出这个点，则做直线拟合
    if (count > 0)
    {
        // 曲线拟合
        cvFitLine(&pointMat,    // 存于Mat的点集
                  CV_DIST_L2,   // 距离评估方法
                  0,            // 距离类型参数
                  0.001,        // 半径精度
                  0.001,        // 角度精度
                  line          // 输出直线，浮点型指针
                  );

        // 计算角度值
        // /偏为负，\偏为正，丨为零
        double tan_angle = double(line[0] / line[1]);
        angle = atan(tan_angle);
        angle = angle * 180 / CV_PI;
    }
//    cvReleaseMat(&pointMat);
    free(points);
}

/*===================================================================
 * 函数名：BinaryCenterScreenLine
 *
 * 功能：ROI区域过小时，连接ROI中心与屏幕中心的直线，并返回一个角度值；
 *
 * 说明：
 *  1、检查输入是否为单通道图像，非单通道图像时输出错误信息；
 *  2、计算ROI区域的中点、屏幕中点坐标；
 *  3、两点坐标存于CvMat中，拟合通过屏幕中点的直线；
 *  4、计算拟合直线的角度信息；
 *  5、如果识别为点、线，则计算line的相关数值；并根据计算出line的参数，计算出直线的角度；并规定：
 *      /偏：斜率为负；
 *      \偏：斜率为正；
 *      丨 ：斜率为零；
 * 参数：
 *  Mat src:                输入图像
 *  Rect rect:              ROI区域
 *  float *line:            拟合曲线信息
 *  double &angle:          拟合曲线角度值
=====================================================================
 */
void BinaryCenterScreenLine(Mat src, Rect rect, float *line, double &angle)
{
    if(src.type() != CV_8U)
    {
        cout<<"Center Screen Line Error: Pixel Format: "<< src.type()<<" is Wrong!"<<endl;
        return ;
    }


    CvPoint* points = (CvPoint*)malloc( 2 * sizeof(points[0]));
    // 点集, 存储count个随机点points
    CvMat pointMat = cvMat( 1, 2, CV_32SC2, points );

    int RectX = (2 * rect.x + rect.width) / 2;
    int RectY = (2 * rect.y + rect.height) / 2;
    int CenterX = src.size().width / 2 - 1;
    int CenterY = src.size().height / 2 - 1;

    points[0].x = RectX;
    points[0].y = RectY;
    points[1].x = CenterX;
    points[1].y = CenterY;

    cvFitLine(&pointMat,    // 存于Mat的点集
              CV_DIST_L2,   // 距离评估方法
              0,            // 距离类型参数
              0.001,        // 半径精度
              0.001,        // 角度精度
              line          // 输出直线，浮点型指针
              );

    // 计算角度值
    // /偏为负，\偏为正，丨为零
    double tan_angle = double(line[0] / line[1]);
    angle = atan(tan_angle);
    angle = angle * 180 / CV_PI;
    free(points);
}

/*===================================================================
 * 函数名：CalcuLMR
 *
 * 功能：（初版本）计算直线在图像中的位置（左/中/右）
 *
 * 说明：
 *  1、计算图像尺寸信息：宽度、高度、X轴中心坐标；
 *  2、计算角度值
 *  3、根据输入的百分数，生成判定位置的基准线；
 *  4、判断位置信息：
 *  (1) 直线足够垂直，且位于图像中间部分：       0
 *  (2) 直线足够垂直，且位于图像左侧部分：      -1
 *  (3) 直线足够垂直，且位于图像右侧部分：       1
 *  (4) 直线不够垂直：                        2
 *  (5) 其他情况：                           3
 *
 * 参数：
 *  Mat &img:                 输入图像，生成图像尺寸信息，用于后面的计算
 *  float *line:              输出拟合直线的信息
 *  double percent:           判断为中间部分的图像宽度百分比；
 *      例：该参数输入0.1，则640*480图像的X方向上中心部分的20%被设置为中心部分；
 *  bool DrawCenterLimitLine: 决定是否绘制中间部分边界线；
 *      为0时绘制，为1时不绘制；
=====================================================================
 */
int CalcuLMR(Mat &img, float *line, double percent, bool DrawCenterLimitLine)
{
//    cout<<"LMR Begin."<<endl;
    int width = img.size().width - 1;
    int height = img.size().height - 1;
    // +0.5与(int)配合，实现四舍五入
    int centerX = (int)((float)img.size().width / 2 + 0.5) - 1;

    // 计算角度值
    double tan_angle = double(line[0] / line[1]);
    cout<<"tan_angle: "<<tan_angle<<endl;
    double angle = atan(tan_angle);
    angle = angle * 180 / CV_PI;

    // 左右限
    int LeftLimit = centerX - int(percent * double(width));
    int RightLimit= centerX + int(percent * double(width));

    // 绘制左右边界线
    if (DrawCenterLimitLine)
    {
        // 两条垂直直线的两个端点
        CvPoint LeftLineUp, LeftLineDown, RightLineUp, RightLineDown;
        LeftLineUp.x = LeftLineDown.x = LeftLimit;
        RightLineUp.x = RightLineDown.x = RightLimit;
        LeftLineUp.y = RightLineUp.y = 0;
        LeftLineDown.y = RightLineDown.y = height;

        // 绘制左右边界线
        cv::line(img, LeftLineUp, LeftLineDown, CV_RGB(0, 0, 255), 2);
        cv::line(img, RightLineUp, RightLineDown, CV_RGB(0, 0, 255), 2);
    }

    // 直线与图像底边的交点的X坐标
    int endLineX = line[2] + tan_angle * (height - line[3]);
//    cout<<"endLineX: "<<endLineX<<endl;
\
    // 若判断并未垂直，则返回未垂直信息：2
    if (fabs(angle) > 5 )
        return 2;



    // 偏左
    if(endLineX < LeftLimit)
        return -1;
    // 偏右
    else if (endLineX > RightLimit)
        return 1;
    // 中间
    else if (endLineX > LeftLimit && endLineX < RightLimit)
        return 0;
    // 错误
    else
        return 3;
}

/*===================================================================
 * 函数名：CalcuLMR2
 *
 * 功能：（二代版本）计算直线在图像中的位置（左/中/右）
 *
 * 说明：
 *  1、计算图像尺寸信息：宽度、高度；
 *  2、根据输入的百分数，生成判定位置的基准线；
 *  3、计算直线延长到图像顶部与底部交点的X轴坐标，两数之和除以2，该值作为判断直线是否在
 *   中间部分的判决标准；
 *  4、判断位置信息：
 *  (1) 直线在中间部分：    0
 *  (2) 直线在左侧部分：   -1
 *  (3) 直线在右侧部分：    1
 *  (4) 其他情况：         2
 *
 * 参数：
 *  Mat &img:                 输入图像，生成图像尺寸信息，用于后面的计算
 *  float *line:              输出拟合直线的信息
 *  double percent:           判断为中间部分的图像宽度百分比；
 *      例：该参数输入0.1，则640*480图像的X方向上中心部分的20%被设置为中心部分；
 *  bool DrawCenterLimitLine: 决定是否绘制中间部分边界线；
 *      为0时绘制，为1时不绘制；
=====================================================================
 */
int CalcuLMR2(Mat &img, float *line, double percent, bool DrawCenterLimitLine)
{
    int width = img.size().width - 1;
    int height = img.size().height - 1;
    // +0.5与(int)配合，实现四舍五入
    int centerX = (int)((float)img.size().width / 2 + 0.5) - 1;

    // 计算角度值
    double tan_angle = double(line[0] / line[1]);
    cout<<"tan_angle: "<<tan_angle<<endl;
    double angle = atan(tan_angle);
    angle = angle * 180 / CV_PI;

    // 左右限
    int LeftLimit = centerX - int(percent * double(width));
    int RightLimit= centerX + int(percent * double(width));

    // 绘制左右边界线
    if (DrawCenterLimitLine)
    {
        // 两条垂直直线的两个端点
        CvPoint LeftLineUp, LeftLineDown, RightLineUp, RightLineDown;
        LeftLineUp.x = LeftLineDown.x = LeftLimit;
        RightLineUp.x = RightLineDown.x = RightLimit;
        LeftLineUp.y = RightLineUp.y = 0;
        LeftLineDown.y = RightLineDown.y = height;

        // 绘制左右边界线
        cv::line(img, LeftLineUp, LeftLineDown, CV_RGB(0, 0, 255), 2);
        cv::line(img, RightLineUp, RightLineDown, CV_RGB(0, 0, 255), 2);
    }

    // 直线与图像顶边的交点的X坐标
    int TopLineX = line[2] - tan_angle * (line[3]);
    // 直线与图像底边的交点的X坐标
    int BottomLineX = line[2] + tan_angle * (height - line[3]);

    // 判决直线整体在左、中、右
    int centerOfLine = (TopLineX + BottomLineX) / 2;

    // 偏左
    if(centerOfLine < LeftLimit)
        return -1;
    // 偏右
    else if (centerOfLine > RightLimit)
        return 1;
    // 中间
    else if (centerOfLine > LeftLimit && centerOfLine < RightLimit)
        return 0;
    else
        return 2;
}
