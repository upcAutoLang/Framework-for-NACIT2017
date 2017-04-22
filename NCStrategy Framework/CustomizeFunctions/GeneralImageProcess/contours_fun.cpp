#include <GeneralImageProcess/contours_fun.h>


/*===================================================================
 * 函数名：PaintRectContours
 *
 * 功能：矩形拟合，并绘制输入的轮廓
 *
 * 参数：
 *  Mat &src:                           输入图像
 *  Mat &dst:                           输出图像
 *  vector<vector<Point> > &contours:    输入轮廓向量
=====================================================================
 */
void PaintRectContours(Mat &src, Mat &dst,
                       vector<vector<Point> > &contours)
{
    src.copyTo(dst);
    // 遍历轮廓序列contours
    for(size_t i = 0; i < contours.size(); i++)
    {
        size_t count = contours[i].size();
        // 轮廓个数过少，小于6时，跳出此次循环
        if( count < 6 )
            continue;

        Mat pointsf;
        // 第i个轮廓中，格式转为32位Float，存于Mat类的pointsf中
        // contours[i]为向量，转换为1*n的Mat
        Mat(contours[i]).convertTo(pointsf, CV_32F);

        // 矩形拟合，返回到旋转矩形RotateRect中
        RotatedRect box = minAreaRect(pointsf);

        // 如果box宽度与高度的最大值，大于宽度与高度最小值的三十倍，则跳出此次循环
        if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*30 )
            continue;

        // 绘制轮廓
        drawContours(dst,                   // 输入图像
                     contours,              // 被绘制的轮廓
                     (int)i,                // 轮廓编号
                     Scalar::all(255),      // 颜色，此处为白色
                     1,                     // 绘制轮廓厚度
                     8                      // 线型
                     );

        // 返回外接矩形的四个顶点
        Point2f vtx[4];
        box.points(vtx);

        // 绘制外接矩形，绿色
        for( int j = 0; j < 4; j++ )
            line(dst, vtx[j], vtx[(j+1)%4], Scalar(0,255,0), 2, CV_AA);
    }
}


/*===================================================================
 * 函数名：ContoursAreaDownSort
 *
 * 功能：轮廓根据面积进行冒泡法降序排序
 *
 * 说明：
 *  1、将所有轮廓的面积一一对应的赋值到double类型的vector中；
 *  2、降序冒泡法，比较轮廓的大小，若第j个轮廓面积小于第j+1个轮廓面积，则两个轮廓
 * 与面积vector相应的元素交换位置；
 *
 * 参数：
 *  vector<vector<Point> > &contours: 输入轮廓向量
=====================================================================
 */
void ContoursAreaDownSort(vector<vector<Point> > &contours)
{
    vector<double> ContoursAreas;
    int length = contours.size();

    if(length <= 0)
    {
//        cout<<"Down Sort: No Found Contours!"<<endl;
        return ;
    }

    for (int i = 0; i < length; i++)
    {
        double Area;
        Area = contourArea(contours[i]);
        ContoursAreas.push_back(Area);
//        cout<<"ContoursAreas["<<i<<"]: "<<ContoursAreas[i]<<endl;
    }

    // 降序冒泡法
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - 1; j++)
            if(ContoursAreas[j] < ContoursAreas[j+1])
            {
                std::swap(contours[j], contours[j+1]);
                std::swap(ContoursAreas[j], ContoursAreas[j+1]);
            }
    }
}

/*===================================================================
 * 函数名：ContoursLengthDownSort
 *
 * 功能：轮廓根据拟合矩形尺寸进行冒泡法降序排序
 *
 * 说明：
 *  1、将所有轮廓的面积一一对应的赋值到double类型的vector中；
 *  2、降序冒泡法，比较轮廓拟合矩形长宽最大值的大小，若第j个轮廓面积小于第j+1个
 * 轮廓面积，则两个轮廓与面积vector相应的元素交换位置；
 *
 * 参数：
 *  vector<vector<Point> > &contours: 输入轮廓向量
=====================================================================
 */
void ContoursLengthDownSort(vector<vector<Point> > &contours)
{
    vector<double> ContoursLengths;
    int length = contours.size();

    if(length <= 0)
    {
//        cout<<"Down Sort: No Found Contours!"<<endl;
        return ;
    }

    for (int i = 0; i < length; i++)
    {
//        double Area;
//        Area = contourArea(contours[i]);
        RotatedRect rect;
        ContourFitRotatedRect(contours[i], rect);
        // 以length作长边
        float len, width;
        len = rect.size.height;
        width = rect.size.width;
        if(len <= width)
            std::swap(len, width);
        ContoursLengths.push_back(len);
//        cout<<"ContoursAreas["<<i<<"]: "<<ContoursAreas[i]<<endl;
    }

    // 降序冒泡法
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - 1; j++)
            if(ContoursLengths[j] < ContoursLengths[j+1])
            {
                std::swap(contours[j], contours[j+1]);
                std::swap(ContoursLengths[j], ContoursLengths[j+1]);
            }
    }
}

/*===================================================================
 * 函数名：ContoursLeftDownSort
 *
 * 功能：轮廓从左向右排序
 *
 * 说明：
 *  1、将所有轮廓的面积一一对应的赋值到float类型的vector中；
 *  2、降序冒泡法，比较轮廓拟合矩形中心点x值的大小，若第j个轮廓中心X值小于第j+1个
 * 中心X值，则两个轮廓与面积vector相应的元素交换位置；
 *
 * 参数：
 *  vector<vector<Point> > &contours: 输入轮廓向量
=====================================================================
 */
void ContoursLeftDownSort(vector<vector<Point> > &contours)
{
    vector<float> ContoursX;
    int length = contours.size();

    if(length <= 0)
    {
//        cout<<"Down Sort: No Found Contours!"<<endl;
        return ;
    }

    for (int i = 0; i < length; i++)
    {
        RotatedRect rect;
        ContourFitRotatedRect(contours[i], rect);

        float X;
        X = rect.center.x;
        ContoursX.push_back(X);
    }

    // 降序冒泡法
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - 1; j++)
            if(ContoursX[j] > ContoursX[j+1])
            {
                std::swap(contours[j], contours[j+1]);
                std::swap(ContoursX[j], ContoursX[j+1]);
            }
    }
}

/*===================================================================
 * 函数名：ContoursUpWardDownSort
 *
 * 功能：轮廓从下向上排序
 *
 * 说明：
 *  1、将所有轮廓的面积一一对应的赋值到float类型的vector中；
 *  2、降序冒泡法，比较轮廓拟合矩形中心点Y值的大小，若第j个轮廓中心Y值小于第j+1个
 * 中心Y值，则两个轮廓与面积vector相应的元素交换位置；
 *
 * 参数：
 *  vector<vector<Point> > &contours: 输入轮廓向量
=====================================================================
 */
void ContoursUpWardDownSort(vector<vector<Point> > &contours)
{
    vector<float> ContoursY;
    int length = contours.size();

    if(length <= 0)
    {
//        cout<<"Down Sort: No Found Contours!"<<endl;
        return ;
    }

    for (int i = 0; i < length; i++)
    {
        RotatedRect rect;
        ContourFitRotatedRect(contours[i], rect);

        float Y;
        Y = rect.center.y;
        ContoursY.push_back(Y);
    }
    // 降序冒泡法
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - 1; j++)
            if(ContoursY[j] < ContoursY[j+1])
            {
                std::swap(contours[j], contours[j+1]);
                std::swap(ContoursY[j], ContoursY[j+1]);
            }
    }
}

/*===================================================================
 * 函数名：DeleteContoursByRank
 *
 * 功能：根据排序后的名次删除冗余轮廓
 *
 * 说明：
 *  输入轮廓向量（输入的轮廓向量是已经排序过的），以及轮廓面积由大到小排序情况下需
 * 要留下的轮廓个数；经过该函数后，除了最大的几个轮廓，其他冗余轮廓全部被删除；
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量
 *  int savenum:                        需要留下的轮廓个数
=====================================================================
 */
void DeleteContoursByRank(vector<vector<Point> > &contours, int savenum)
{
    int length = contours.size();

    if(length > savenum)
        for (int i = length; i > savenum; --i)
        {
            contours.erase(contours.end() -1 );
        }
}

/*===================================================================
 * 函数名：PrintContoursDistInfo
 *
 * 功能：打印轮廓与图像中心点距离信息
 *
 * 说明：
 *  将输入的轮廓向量相关信息在控制台中打印出来；
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量；
 *  vector<int> &XDistance:             需要存储到各轮廓向量中心与屏幕中心的X方向距离；
 *  vector<int> &YDistance:             需要存储到各轮廓向量中心与屏幕中心的Y方向距离；
=====================================================================
 */
void PrintContoursDistInfo(vector<vector<Point> > &contours,
                       vector<int> &XDistance,
                       vector<int> &YDistance)
{
    vector<double> ContoursAreas;
    int length = contours.size();
    for (int i = 0; i < length; i++)
    {
        double Area;
        Area = contourArea(contours[i]);
        ContoursAreas.push_back(Area);
//        cout<<"ContoursAreas["<<i<<"]: "<<ContoursAreas[i]<<endl;
    }

    for (int i = 0; i < length; i++)
    {
        cout<<"Contour ["<<i<<"] : "<<endl;
        cout<<"  Areas : "<<ContoursAreas[i]<<endl;
        cout<<"  X Distance : "<<XDistance[i]<<endl;
        cout<<"  Y Distance : "<<YDistance[i]<<endl;
    }
    cout<<endl;
}



/*===================================================================
 * 函数名：CalcuContoursDistance
 *
 * 功能：计算轮廓与屏幕中心X、Y方向上的距离
 *
 * 说明：
 *  输入Mat，以其尺寸作为基准，计算轮廓向量contours的拟合椭圆中心点与图像中心的
 * X、Y方向距离；
 *
 * 参数：
 *  Mat img:                            输入图像，用于得到图像中心点的X、Y值
 *  vector<vector<Point> > &contours:   输入轮廓向量
 *  vector<int> &XDistance:             各轮廓的中心与屏幕中心的X方向距离
 *  vector<int> &YDistance:             各轮廓的中心与屏幕中心的Y方向距离
=====================================================================
 */
void CalcuContoursDistance(Mat img,
                           vector<vector<Point> > &contours,
                           vector<int> &XDistance,
                           vector<int> &YDistance)
{
    int length = contours.size();
    XDistance.clear();
    YDistance.clear();
    // +0.5与(int)配合，实现四舍五入
    int centerX = (int)((float)img.size().width / 2 + 0.5) - 1;
    int centerY = (int)((float)img.size().height/ 2 + 0.5) - 1;
//    cout<<"centerX: "<<centerX<<endl;
//    cout<<"centerY: "<<centerY<<endl;

    int x_distance = 0;
    int y_distance = 0;

    for(int i = 0; i < length; i++)
    {
        size_t count = contours[i].size();
        // 轮廓个数过少，小于6时，跳出此次循环
        if( count < 6 )
            continue;

        Mat pointsf;
        // 第i个轮廓中，格式转为32位Float，存于Mat类的pointsf中
        // contours[i]为向量，转换为1*n的Mat
        Mat(contours[i]).convertTo(pointsf, CV_32F);

        // 椭圆拟合，返回到旋转矩形RotateRect中
        RotatedRect box = fitEllipse(pointsf);

        x_distance = (int)box.center.x - centerX;
        y_distance = (int)box.center.y - centerY;

        XDistance.push_back(x_distance);
        YDistance.push_back(y_distance);
    }
}
/*===================================================================
 * 函数名：CalcuContoursCenterPoint
 *
 * 功能：计算轮廓中心位置
 *
 * 说明：
 *  输入Mat，以其尺寸作为基准，计算轮廓向量contours的拟合矩形中心点在图像中的坐标
 *
 * 参数：
 *  vector<vector<Point> > &contours:   输入轮廓向量
 *  vector<Point> &CenterPoints:        各轮廓的拟合矩形的中心点坐标
=====================================================================
 */
void CalcuContoursCenterPoint(vector<vector<Point> > &contours,
                              vector<Point> &CenterPoints)
{
    int length = contours.size();
    CenterPoints.clear();

    for(int i = 0; i < length; i++)
    {
        size_t count = contours[i].size();
        // 轮廓个数过少，小于8时，跳出此次循环
        if( count < 8 )
            continue;

        Mat pointsf;
        // 第i个轮廓中，格式转为32位Float，存于Mat类的pointsf中
        // contours[i]为向量，转换为1*n的Mat
        Mat(contours[i]).convertTo(pointsf, CV_32F);

        // 矩形拟合，返回到旋转矩形RotateRect中
        RotatedRect box = minAreaRect(pointsf);
        CenterPoints.push_back(box.center);
    }
}

/*===================================================================
 * 函数名：PrintContoursCenterPointInfo
 *
 * 功能：打印轮廓与图像中心点距离信息
 *
 * 说明：
 *  将输入的轮廓向量相关信息在控制台中打印出来；
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量；
 *  vector<Point> &CenterPoints:        轮廓向量中心点；
=====================================================================
 */
void PrintContoursCenterPointInfo(vector<vector<Point> > &contours,
                                  vector<Point> &CenterPoints)
{
    int length = contours.size();

    for (int i = 0; i < length; i++)
    {
        cout<<"Contour ["<<i<<"] : "<<endl;
        cout<<"  Center Point : ("<<CenterPoints[i].x<<", "<<CenterPoints[i].y<<")"<<endl;
    }
    cout<<endl;
}


/*===================================================================
 * 函数名：ContoursMinAreaRect
 *
 * 功能：寻找轮廓的最小外包矩形
 *
 * 说明：
 *  求出输入轮廓的最小外包轮廓；
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量；
 *  RotatedRect &MinAreaBox:            多个轮廓的最小外接矩形；
=====================================================================
 */
void ContoursMinAreaRect(vector<vector<Point> > &contours,
                         RotatedRect &MinAreaBox)
{
    size_t size = contours.size();

    vector<Point2f> points;

    // 图中存在轮廓时，才会开始拟合轮廓的最小外接矩形；
    if(size > 0)
    {
        for(size_t i = 0; i < size; i++)
        {
            Mat pointsf;
            // 第i个轮廓中，格式转为32位Float，存于Mat类的pointsf中
            // contours[i]为向量，转换为1*n的Mat
            Mat(contours[i]).convertTo(pointsf, CV_32F);

            // 矩形拟合，返回到旋转矩形RotateRect中
            RotatedRect box = minAreaRect(pointsf);
            // 返回外接矩形的四个顶点
            Point2f vtx[4];
            box.points(vtx);

            // 四个顶点push_back到Point2f向量points中；
            for(int j = 0; j < 4; j++)
            {
                points.push_back(vtx[j]);
            }
        }

        // 向量points中存储了所有轮廓拟合矩形的四个顶点，根据这些点拟合出所有轮廓的最小外接矩形；
        MinAreaBox = minAreaRect(points);
    }
}

/*===================================================================
 * 函数名：RectsMinAreaRect
 *
 * 功能：寻找RotatedRect向量的最小外包矩形
 *
 * 说明：
 *  求出输入轮廓的最小外包轮廓；
 *
 * 参数：
 *  vector<RotatedRect> Rects:          输入旋转矩形向量；
 *  RotatedRect &MinAreaBox:            多个轮廓的最小外接矩形；
=====================================================================
 */
void RectsMinAreaRect(vector<RotatedRect> Rects,
                      RotatedRect &MinAreaBox)
{
    size_t size = Rects.size();

    vector<Point2f> points;

    if(size > 0)
    {
        for(size_t i = 0; i < size; i++)
        {
            RotatedRect tmpRect;
            tmpRect = Rects[i];
            Point2f vtx[4];
            tmpRect.points(vtx);

            // 四个顶点push_back到Point2f向量points中；
            for(int j = 0; j < 4; j++)
            {
                points.push_back(vtx[j]);
            }
        }

        // 向量points中存储了所有轮廓拟合矩形的四个顶点，根据这些点拟合出所有轮廓的最小外接矩形；
        MinAreaBox = minAreaRect(points);
    }
}

/*===================================================================
 * 函数名：ContoursMinAreaRectCenter
 *
 * 功能：寻找轮廓的最小外包矩形的中点
 *
 * 说明：
 *  求出输入轮廓的最小外包轮廓的中点；（函数本质上与ContoursMinAreaRect相同）
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量；
 *  Point2f &Center:                    多个轮廓的最小外接矩形的中点；
=====================================================================
 */
void ContoursMinAreaRectCenter(vector<vector<Point> > &contours,
                               Point2f &Center)
{
    RotatedRect MinAreaBox;
    size_t size = contours.size();
    if(size > 0)
    {
        ContoursMinAreaRect(contours, MinAreaBox);

        Center = MinAreaBox.center;
    }
}

/*===================================================================
 * 函数名：ContoursMinAreaRectMidLine
 *
 * 功能：寻找轮廓的最小外包矩形的中线
 *
 * 说明：
 *  求出输入轮廓的最小外包轮廓的中线；（函数本质上与ContoursMinAreaRect相同）
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓向量；
 *  Point2f &UpMidPoint:                多个轮廓的最小外接矩形的顶边中点；
 *  Point2f &DownMidPoint:              多个轮廓的最小外接矩形的底边中点；
=====================================================================
 */
void ContoursMinAreaRectMidLine(vector<vector<Point> > &contours,
                                Point2f &UpMidPoint, Point2f &DownMidPoint)
{
    RotatedRect MinAreaBox;
    size_t size = contours.size();
    if(size > 0)
    {
        ContoursMinAreaRect(contours, MinAreaBox);

        // 返回外接矩形的四个顶点
        Point2f vtx[4];
    //    box.points(vtx);

        MinAreaBox.points(vtx);


        // 保证为竖直方向的中线
        float RectAngle1, RectAngle2;
        RectAngle1 = atan(fabs((vtx[3].y - vtx[0].y) / (vtx[3].x - vtx[0].x)));
        RectAngle1 = RectAngle1 * 180 / CV_PI;
        RectAngle2 = atan(fabs((vtx[0].y - vtx[1].y) / (vtx[0].x - vtx[1].x)));
        RectAngle2 = RectAngle2 * 180 / CV_PI;

        if(RectAngle1 > 90)     RectAngle1 = 90;
        if(RectAngle1 < 0)      RectAngle1 = 0;
        if(RectAngle2 > 90)     RectAngle2 = 90;
        if(RectAngle2 < 0)      RectAngle2 = 0;
    //    cout<<RectAngle1<<" "<<RectAngle2<<endl;

        if(RectAngle1 < 45 && RectAngle2 > 45)
        {
            UpMidPoint.x = (vtx[0].x + vtx[3].x) / 2;
            UpMidPoint.y = (vtx[0].y + vtx[3].y) / 2;
            DownMidPoint.x = (vtx[1].x + vtx[2].x) / 2;
            DownMidPoint.y = (vtx[1].y + vtx[2].y) / 2;
        }
        else
        {
            UpMidPoint.x = (vtx[0].x + vtx[1].x) / 2;
            UpMidPoint.y = (vtx[0].y + vtx[1].y) / 2;
            DownMidPoint.x = (vtx[2].x + vtx[3].x) / 2;
            DownMidPoint.y = (vtx[2].y + vtx[3].y) / 2;
        }
    }
}

/*===================================================================
 * 函数名：AverageRect
 *
 * 功能：求三个缓存矩形的均值
 *
 * 说明：
 *  三个缓存矩形，对应的各顶点x、y值求平均值，得到目标矩形的顶点；最后拟合得到目标矩形；
 *
 * 参数：
 *  RotatedRect &dst:       目标矩形
 *  RotatedRect src1:       缓存矩形1
 *  RotatedRect src2:       缓存矩形2
 *  RotatedRect src3:       缓存矩形3
=====================================================================
 */
void AverageRect(RotatedRect &dst, RotatedRect src1, RotatedRect src2, RotatedRect src3)
{
    Point2f src1point[4], src2point[4], src3point[4];
    for(int i = 0; i < 4; i++)
    {
        src1.points(src1point);
        src2.points(src2point);
        src3.points(src3point);
    }
    Point2f dstpoint[4];

    for(int i = 0; i < 4; i++)
    {
        dstpoint[i].x = (src1point[i].x + src2point[i].x + src3point[i].x) / 3;
        dstpoint[i].y = (src1point[i].y + src2point[i].y + src3point[i].y) / 3;
    }

    vector<Point2f> points;
    for(int i = 0; i < 4; i++)
    {
        points.push_back(dstpoint[i]);
    }
    dst = minAreaRect(points);
}

/*===================================================================
 * 函数名：ContourFitRotatedRect
 *
 * 功能：根据一个轮廓，拟合出一个旋转矩形；
 *
 * 参数：
 *  vector<Point> contour:      输入轮廓
 *  RotatedRect &Rect:          拟合出的旋转矩形
/*=====================================================================
 */
void ContourFitRotatedRect(vector<Point> contour, RotatedRect &rect)
{
    Mat pointsf;
    Mat(contour).convertTo(pointsf, CV_32F);
    rect = minAreaRect(pointsf);
}

// 根据一个轮廓，拟合出一个ROI矩形；
void ContourFitROIRect(vector<Point> contour, Rect &rect)
{
    Mat pointsf;
    Mat(contour).convertTo(pointsf, CV_32F);
    rect = boundingRect(pointsf);
}

/*===================================================================
 * 函数名：ContourRotatedRectLength
 *
 * 功能：根据一个轮廓，算出拟合旋转矩形的长边；
 *
 * 参数：
 *  vector<Point> contour:      输入轮廓；
 *  float &length:              拟合矩形的长边；
/*=====================================================================
 */
void ContourRotatedRectLength(vector<Point> contour, float &length)
{
    RotatedRect tmpRect;
    ContourFitRotatedRect(contour, tmpRect);
    float len, wid;
    len = tmpRect.size.height;
    wid = tmpRect.size.width;
    if(len <= wid)
        std::swap(len, wid);
    length = len;
}

// Rect转RotatedRect
RotatedRect RotatedRect2Rect(Rect rect)
{
    Point2f center;
    Size2f size;
    center = Point2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
    size = Size2f(rect.width, rect.height);
    RotatedRect tmp = RotatedRect(center, size, 0);
    return tmp;
}

/*===================================================================
 * 函数名：DeleteTopLightVoice
 *
 * 功能：剔除自然光分量函数；
 *
 * 说明：
 *   将图像中较靠上的部分当作自然光分量，拟合轮廓的矩形，在这条上限之上的轮廓
 * 将被剔除；
 *
 * 参数：
 *  Mat src:                            输入图像；
 *  vector<vector<Point> > &contours:    输入轮廓；
 *  double upratio:                     顶部与图像高度之比；
/*=====================================================================
 */
void DeleteTopLightVoice(Mat src, vector<vector<Point> > &contours,
                         double upratio)
{
    vector<RotatedRect> box;

    int size = contours.size();

    int height = src.size().height;
    int UpLimit = height / upratio;

    // 填充RotatedRect向量
    for(int i = 0; i < size; i++)
    {
        RotatedRect tmpRect;
        ContourFitRotatedRect(contours[i], tmpRect);
        box.push_back(tmpRect);
    }

    // 检查剩余几个轮廓的外接矩形，若外接矩形的中心在图像上限之上，则被视为自然光分量，剔除之；
    for(int i = size - 1; i >= 0; --i)
    {
        if(box[i].center.y < UpLimit)
            contours.erase(contours.begin() + i);
    }
}

// 剔除摄像头底部光分量函数
void DeleteBottomLightVoice(Mat src, vector<vector<Point> > &contours,
                            double DownRatio)
{
    vector<RotatedRect> box;

    int size = contours.size();

    int height = src.size().height;
    int DownLimit = height - height / DownRatio;

    // 填充RotatedRect向量
    for(int i = 0; i < size; i++)
    {
        RotatedRect tmpRect;
        ContourFitRotatedRect(contours[i], tmpRect);
        box.push_back(tmpRect);
    }

    // 检查剩余几个轮廓的外接矩形，若外接矩形的中心在图像上限之上，则被视为自然光分量，剔除之；
    for(int i = size - 1; i >= 0; --i)
    {
        if(box[i].center.y > DownLimit)
            contours.erase(contours.begin() + i);
    }
}

/*===================================================================
 * 函数名：DeleteSmallContours
 *
 * 功能：按照轮廓尺寸，剔除尺寸过小的轮廓
 *
 * 参数：
 *  vector<vector<Point> > &contours:    输入轮廓；
 *  int limit:                          轮廓尺寸下限；
/*=====================================================================
 */
void DeleteSmallContours(vector<vector<Point> > &contours, int limit)
{
    int size = contours.size();

    // 遍历轮廓
    for(int i = size - 1; i >= 0; --i)
    {
        float tmplength;
        ContourRotatedRectLength(contours[i], tmplength);

        // 若轮廓长边小于某值，则剔除该轮廓
        if(tmplength <= limit)
            contours.erase(contours.begin() + i);
    }
}

/*===================================================================
 * 函数名：calcLineDegree
 *
 * 功能：计算两点连线的角度
 *
 * 参数：
 *  const Point2f& firstPt:     第一个点的坐标
 *  const Point2f& secondPt:    第二个点的坐标
/*=====================================================================
 */
double calcLineDegree(Point2f& firstPt, Point2f& secondPt)
{
    double curLineAngle = 0.0f;
    if (secondPt.x - firstPt.x != 0)
    {
        curLineAngle = atan(static_cast<double>(firstPt.y - secondPt.y) / static_cast<double>(secondPt.x - firstPt.x));
        if (curLineAngle < 0)
        {
            curLineAngle += CV_PI;
        }
    }
    else
    {
        curLineAngle = CV_PI / 2.0f; //90度
    }
    return curLineAngle*180.0f/CV_PI;
}

/*===================================================================
 * 函数名：getRcDegree
 *
 * 功能：计算RotatedRect长边的angle
 *
 * 参数：
 *  const RotatedRect box:      需要求的RotatedRect；
/*=====================================================================
 */
double getRcDegree(RotatedRect box)
{
    double degree = 0.0f;
    Point2f vertVect[4];
    box.points(vertVect);
    //line 1
    const double firstLineLen = (vertVect[1].x - vertVect[0].x)*(vertVect[1].x - vertVect[0].x) +
        (vertVect[1].y - vertVect[0].y)*(vertVect[1].y - vertVect[0].y);
    //line 2
    const double secondLineLen = (vertVect[2].x - vertVect[1].x)*(vertVect[2].x - vertVect[1].x) +
        (vertVect[2].y - vertVect[1].y)*(vertVect[2].y - vertVect[1].y);
    if (firstLineLen > secondLineLen)
        degree = calcLineDegree(vertVect[0], vertVect[1]);
    else
        degree = calcLineDegree(vertVect[2], vertVect[1]);
    return degree;
}










