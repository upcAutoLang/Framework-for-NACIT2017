
#include <imageprocessing_fun.h>

/*===================================================================
 * 函数名：HSVThresh
 *
 * 功能：使用HSV阈值，将输入图像处理成为二值化图像
 *
 * 说明：HSV处理方法
 *  关于Method:
 *    1：普通方法，遍历；
 *    2：迭代器方法;
 *    3：inrange函数，默认；
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  int HMin = 0:       色相Hue最小值
 *  int HMax = 180:     色相Hue最大值
 *  int SMin = 0:       饱和度Saturation最小值
 *  int SMax = 255:     饱和度Saturation最大值
 *  int VMin = 0:       亮度Value最小值
 *  int VMax = 255:     亮度Value最大值
 *  int Method = 3:     不同的处理方法，默认为3，即inrange方法
=====================================================================
*/
void HSVThresh(Mat &src, Mat &dst, int HMin, int HMax, int SMin, int SMax, int VMin, int VMax, int Method)
{
    Mat imgHSV;
    switch(Method)
    {
    // 方法1：普通方法，遍历
    case 1:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);
        cvtColor(src, imgHSV, CV_BGR2HSV);
        for (int i = 0; i < imgHSV.rows; ++i)
        {
            for(int j = 0; j < imgHSV.cols; ++j)
            {
                if(imgHSV.at<cv::Vec3b>(i,j)[0] >= HMin
                        && imgHSV.at<cv::Vec3b>(i,j)[0] <= HMax
                        && imgHSV.at<cv::Vec3b>(i,j)[1] >= SMin
                        && imgHSV.at<cv::Vec3b>(i,j)[1] <= SMax
                        && imgHSV.at<cv::Vec3b>(i,j)[2] >= VMin
                        && imgHSV.at<cv::Vec3b>(i,j)[2] <= VMax)
                {
                    dst.at<uchar>(i,j) = 255;
                }
                else
                {
                    dst.at<uchar>(i,j) = 0;
                }
            }
        }
        break;
    }

    // 方法2：迭代器方法
    case 2:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);
        cvtColor(src, imgHSV, CV_BGR2HSV);
        MatIterator_<Vec3b> HSVit, HSVitend;
        MatIterator_<uchar> Binit, Binitend;
        for(HSVit = imgHSV.begin<Vec3b>(), HSVitend = imgHSV.end<Vec3b>(),
            Binit = dst.begin<uchar>(), Binitend = dst.end<uchar>();
            HSVit != HSVitend, Binit != Binitend;
            ++HSVit, ++Binit)
        {
            if((*HSVit)[0] >= HMin && (*HSVit)[0] <= HMax
                    && (*HSVit)[1] >= SMin && (*HSVit)[1] <= SMax
                    && (*HSVit)[2] >= VMin && (*HSVit)[2] <= VMax)
            {
                *Binit = 255;
            }
            else
            {
                *Binit = 0;
            }
        }
        break;
    }

    // 方法3：inrange函数
    case 3:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);
        cvtColor(src, imgHSV, CV_BGR2HSV);
        cv::Scalar MinThresh = cv::Scalar(HMin, SMin, VMin);
        cv::Scalar MaxThresh = cv::Scalar(HMax, SMax, VMax);
        inRange(imgHSV, MinThresh, MaxThresh, dst);
        break;
    }

    default:
        cout<<"No This HSV Thresh Method!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数名：HSVThresh
 *
 * 功能：使用HSV阈值，将输入图像处理成为二值化图像
 *
 * 说明：HSV处理方法
 *  关于Method:
 *    1：普通方法，遍历；
 *    2：迭代器方法;
 *    3：inrange函数，默认；
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  HSVInfo Info:       HSV阈值上下限信息
 *  int Method = 3:     不同的处理方法，默认为3，即inrange方法
=====================================================================
*/
void HSVThresh(Mat &src, Mat &dst,
               HSVInfo Info, int Method)
{
    Mat imgHSV;
    int HMin, HMax, SMin, SMax, VMin, VMax;
    switch(Method)
    {
    // 方法1：普通方法，遍历
    case 1:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        HMin = Info.HMin;
        HMax = Info.HMax;
        SMin = Info.SMin;
        SMax = Info.SMax;
        VMin = Info.VMin;
        VMax = Info.VMax;

        cvtColor(src, imgHSV, CV_BGR2HSV);
        for (int i = 0; i < imgHSV.rows; ++i)
        {
            for(int j = 0; j < imgHSV.cols; ++j)
            {
                if(imgHSV.at<cv::Vec3b>(i,j)[0] >= HMin
                        && imgHSV.at<cv::Vec3b>(i,j)[0] <= HMax
                        && imgHSV.at<cv::Vec3b>(i,j)[1] >= SMin
                        && imgHSV.at<cv::Vec3b>(i,j)[1] <= SMax
                        && imgHSV.at<cv::Vec3b>(i,j)[2] >= VMin
                        && imgHSV.at<cv::Vec3b>(i,j)[2] <= VMax)
                {
                    dst.at<uchar>(i,j) = 255;
                }
                else
                {
                    dst.at<uchar>(i,j) = 0;
                }
            }
        }
        break;
    }

    // 方法2：迭代器方法
    case 2:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        HMin = Info.HMin;
        HMax = Info.HMax;
        SMin = Info.SMin;
        SMax = Info.SMax;
        VMin = Info.VMin;
        VMax = Info.VMax;

        cvtColor(src, imgHSV, CV_BGR2HSV);
        MatIterator_<Vec3b> HSVit, HSVitend;
        MatIterator_<uchar> Binit, Binitend;
        for(HSVit = imgHSV.begin<Vec3b>(), HSVitend = imgHSV.end<Vec3b>(),
            Binit = dst.begin<uchar>(), Binitend = dst.end<uchar>();
            HSVit != HSVitend, Binit != Binitend;
            ++HSVit, ++Binit)
        {
            if((*HSVit)[0] >= HMin && (*HSVit)[0] <= HMax
                    && (*HSVit)[1] >= SMin && (*HSVit)[1] <= SMax
                    && (*HSVit)[2] >= VMin && (*HSVit)[2] <= VMax)
            {
                *Binit = 255;
            }
            else
            {
                *Binit = 0;
            }
        }
        break;
    }

    // 方法3：inrange函数
    case 3:
    {
        imgHSV.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        HMin = Info.HMin;
        HMax = Info.HMax;
        SMin = Info.SMin;
        SMax = Info.SMax;
        VMin = Info.VMin;
        VMax = Info.VMax;

        cvtColor(src, imgHSV, CV_BGR2HSV);
        cv::Scalar MinThresh = cv::Scalar(HMin, SMin, VMin);
        cv::Scalar MaxThresh = cv::Scalar(HMax, SMax, VMax);
        inRange(imgHSV, MinThresh, MaxThresh, dst);
        break;
    }

    default:
        cout<<"No This HSV Thresh Method!"<<endl;
        break;
    }
}


/*===================================================================
 * 函数名：RGBThresh
 *
 * 功能：使用RGB阈值，将输入图像处理成为二值化图像
 *
 * 说明：RGB处理方法
 *  关于Method:
 *    1：普通方法，遍历；
 *    2：迭代器方法;
 *    3：inrange函数，默认；
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  int RMin = 0:       红色通道Red最小值
 *  int RMax = 255:     红色通道Red最大值
 *  int GMin = 0:       绿色通道Green最小值
 *  int GMax = 255:     绿色通道Green最大值
 *  int BMin = 0:       蓝色通道Blue最小值
 *  int BMax = 255:     蓝色通道Blue最大值
 *  int Method = 3:     不同的处理方法，默认为3，即inrange方法
=====================================================================
*/
void RGBThresh(Mat &src, Mat &dst,
               int RMin, int RMax, int GMin, int GMax, int BMin, int BMax,
               int Method)
{
    Mat imgRGB;
    switch(Method)
    {
    // 方法1：普通方法，遍历
    case 1:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);
        cvtColor(src, imgRGB, CV_BGR2RGB);
        for (int i = 0; i < imgRGB.rows; ++i)
        {
            for(int j = 0; j < imgRGB.cols; ++j)
            {
                if(imgRGB.at<cv::Vec3b>(i,j)[0] >= RMin
                        && imgRGB.at<cv::Vec3b>(i,j)[0] <= RMax
                        && imgRGB.at<cv::Vec3b>(i,j)[1] >= GMin
                        && imgRGB.at<cv::Vec3b>(i,j)[1] <= GMax
                        && imgRGB.at<cv::Vec3b>(i,j)[2] >= BMin
                        && imgRGB.at<cv::Vec3b>(i,j)[2] <= BMax)
                {
                    dst.at<uchar>(i,j) = 255;
                }
                else
                {
                    dst.at<uchar>(i,j) = 0;
                }
            }
        }
        break;
    }

    // 方法2：迭代器方法
    case 2:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);
        cvtColor(src, imgRGB, CV_BGR2RGB);
        MatIterator_<Vec3b> RGBit, RGBitend;
        MatIterator_<uchar> Binit, Binitend;
        for(RGBit = imgRGB.begin<Vec3b>(), RGBitend = imgRGB.end<Vec3b>(),
            Binit = dst.begin<uchar>(), Binitend = dst.end<uchar>();
            RGBit != RGBitend, Binit != Binitend;
            ++RGBit, ++Binit)
        {
            if( (*RGBit)[0] >= RMin && (*RGBit)[0] <= RMax
                    && (*RGBit)[1] >= GMin && (*RGBit)[1] <= GMax
                    && (*RGBit)[2] >= BMin && (*RGBit)[2] <= BMax)
                *Binit = 255;
            else
                *Binit = 0;
        }
        break;
    }

    // 方法3：inrange函数
    case 3:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        cvtColor(src, imgRGB, CV_BGR2RGB);
        cv::Scalar MinThresh = cv::Scalar(RMin, GMin, BMin);
        cv::Scalar MaxThresh = cv::Scalar(RMax, GMax, BMax);
        inRange(imgRGB, MinThresh, MaxThresh, dst);
        break;
    }
    default:
        cout<<"No This RGB Thresh Method!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数名：RGBThresh
 *
 * 功能：使用RGB阈值，将输入图像处理成为二值化图像
 *
 * 说明：RGB处理方法
 *  关于Method:
 *    1：普通方法，遍历；
 *    2：迭代器方法;
 *    3：inrange函数，默认；
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  RGBInfo Info:       RGB阈值上下限信息
 *  int Method = 3:     不同的处理方法，默认为3，即inrange方法
=====================================================================
*/
void RGBThresh(Mat &src, Mat &dst,
               RGBInfo Info, int Method)
{
    Mat imgRGB;
    int RMin, RMax, GMin, GMax, BMin, BMax;
    switch(Method)
    {
    // 方法1：普通方法，遍历；
    case 1:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        RMin = Info.RMin;
        RMax = Info.RMax;
        GMin = Info.GMin;
        GMax = Info.GMax;
        BMin = Info.BMin;
        BMax = Info.BMax;

        cvtColor(src, imgRGB, CV_BGR2RGB);
        for (int i = 0; i < imgRGB.rows; ++i)
        {
            for(int j = 0; j < imgRGB.cols; ++j)
            {
                if(imgRGB.at<cv::Vec3b>(i,j)[0] >= RMin
                        && imgRGB.at<cv::Vec3b>(i,j)[0] <= RMax
                        && imgRGB.at<cv::Vec3b>(i,j)[1] >= GMin
                        && imgRGB.at<cv::Vec3b>(i,j)[1] <= GMax
                        && imgRGB.at<cv::Vec3b>(i,j)[2] >= BMin
                        && imgRGB.at<cv::Vec3b>(i,j)[2] <= BMax)
                {
                    dst.at<uchar>(i,j) = 255;
                }
                else
                {
                    dst.at<uchar>(i,j) = 0;
                }
            }
        }
        break;
    }
    // 方法2：迭代器方法
    case 2:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        RMin = Info.RMin;
        RMax = Info.RMax;
        GMin = Info.GMin;
        GMax = Info.GMax;
        BMin = Info.BMin;
        BMax = Info.BMax;

        cvtColor(src, imgRGB, CV_BGR2RGB);
        MatIterator_<Vec3b> RGBit, RGBitend;
        MatIterator_<uchar> Binit, Binitend;
        for(RGBit = imgRGB.begin<Vec3b>(), RGBitend = imgRGB.end<Vec3b>(),
            Binit = dst.begin<uchar>(), Binitend = dst.end<uchar>();
            RGBit != RGBitend, Binit != Binitend;
            ++RGBit, ++Binit)
        {
            if( (*RGBit)[0] >= RMin && (*RGBit)[0] <= RMax
                    && (*RGBit)[1] >= GMin && (*RGBit)[1] <= GMax
                    && (*RGBit)[2] >= BMin && (*RGBit)[2] <= BMax)
                *Binit = 255;
            else
                *Binit = 0;
        }
        break;
    }
    case 3:
    {
        imgRGB.create(src.size(), src.type());
        dst.create(src.size(), CV_8U);

        RMin = Info.RMin;
        RMax = Info.RMax;
        GMin = Info.GMin;
        GMax = Info.GMax;
        BMin = Info.BMin;
        BMax = Info.BMax;

        cvtColor(src, imgRGB, CV_BGR2RGB);
        cv::Scalar MinThresh = cv::Scalar(RMin, GMin, BMin);
        cv::Scalar MaxThresh = cv::Scalar(RMax, GMax, BMax);
        inRange(imgRGB, MinThresh, MaxThresh, dst);
        break;
    }
    default:
        cout<<"No This RGB Thresh Method!"<<endl;
        break;
    }
}


/*===================================================================
 * 函数名：UndistortCameraFrame
 *
 * 功能：根据输入原畸变图像、内参数矩阵、畸变矩阵，输出处理后的图像
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  Size FrameSize:     被处理的帧图像尺寸
 *  Mat IntrinMat:      内参数矩阵
 *  Mat DistortMat:     畸变矩阵
=====================================================================
*/
bool UndistortCameraFrame(Mat &src, Mat &dst,
                          Size FrameSize, Mat IntrinMat, Mat DistortMat)
{
    // 重绘所需要的矩阵
    Mat mapX, mapY;
    // 单位矩阵
    Mat IdentityMatrix = (Mat_<double>(3,3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);

    bool IsValidIntrinMat = (IntrinMat.size().width == 3) && (IntrinMat.size().height == 3);
    bool IsValidDistortMat=
               (DistortMat.size().width == 1)
            && (DistortMat.size().height == 4)
            || (DistortMat.size().height == 5)
            || (DistortMat.size().height == 8);

    // 若内参数矩阵与畸变矩阵尺寸不正确，则返回false
    if(!IsValidIntrinMat || !IsValidDistortMat)
    {
        return false;
    }

    // 计算消除和纠正畸变的map矩阵，输出的两个map矩阵用于remap函数
    initUndistortRectifyMap(IntrinMat, DistortMat, IdentityMatrix,
                            IntrinMat, FrameSize, CV_32FC1, mapX, mapY);

    // 将源图像复制到目标图像中
    src.copyTo(dst);

    // 重绘图像
    remap(src, dst, mapX, mapY, INTER_NEAREST);
    return true;
}

/*===================================================================
 * 函数名：CutCenterImage
 *
 * 功能：根据输入长宽，裁剪原图像的中心部分图像
 *
 * 参数：
 *  Mat &src:           输入图像
 *  Mat &dst:           输出图像
 *  int width:          裁剪宽度
 *  int height:         裁剪高度
 *  int offsetX:        X方向裁剪偏移像素点个数
 *  int offsetY:        Y方向裁剪偏移像素点个数
=====================================================================
*/
void CutCenterImage(Mat &src, Mat &dst, int width, int height, int offsetX, int offsetY)
{
    if(abs(offsetX) >= width / 2 || abs(offsetY) >= height / 2)
    {
        cout<<"Cut Center Image Error: Offset X or Y is larger than half of width or height!"<<endl;
        return ;
    }
    Mat img;
    int imgWidth, imgHeight;
    int imgStartX, imgStartY;
    imgWidth = src.size().width;
    imgHeight = src.size().height;

    if(imgWidth >= width && imgHeight >= height)
    {
        imgStartX = (imgWidth - width) / 2 + offsetX;
        imgStartY = (imgHeight - height) / 2 + offsetY;
        img = src(Range(imgStartY, imgStartY + height),
                  Range(imgStartX, imgStartX + width));
    }
    else
    {
        img = src;
        cout<<"Width or Height ERROR!"<<endl;
    }

    dst = img;
}

/*===================================================================
 * 函数名：BinMatJoin
 *
 * 功能：二值化图像并运算；
 *
 * 参数：
 *  Mat src1:           输入图像1；
 *  Mat src2:           输入图像2；
 *  Mat &dst:           目标输出图像；
=====================================================================
*/
void BinMatJoin(Mat src1, Mat src2, Mat &dst)
{
    // 检查是否为二值图像
    if(src1.type() != CV_8U || src2.type() != CV_8U)
    {
        cout<<"Binary Mat Join Error: Source Images Format Wrong! "<<endl;
        return ;
    }
    // 检查两图像尺寸是否相等
    if(src1.size != src2.size)
    {
        cout<<"Binary Mat Join Error: Source Images' Sizes Are Not The Same! "<<endl;
        return ;
    }

    Mat tmp;
    tmp.create(src1.size(), CV_8U);
    src1.copyTo(tmp);

    MatIterator_<uchar> it1, it1end;
    MatIterator_<uchar> it2, it2end;
    MatIterator_<uchar> tmpit, tmpend;

    for(it1 = src1.begin<uchar>(), it1end = src1.end<uchar>(),
        it2 = src2.begin<uchar>(), it2end = src2.end<uchar>(),
        tmpit = tmp.begin<uchar>(), tmpend = tmp.end<uchar>();

        it1 != it1end, it2 != it2end, tmpit != tmpend;
        ++it1, ++it2, ++tmpit)
    {
        if( *it1 > 0 || *it2 > 0 )
            *tmpit = 255;
        else
            *tmpit = 0;
    }

    tmp.copyTo(dst);
}

/*===================================================================
 * 函数名：BinMatMinus
 *
 * 功能：二值化图像减运算，将第二张图像中的白色像素从第一张图像中减去；
 *
 * 参数：
 *  Mat src1:           输入图像1；
 *  Mat src2:           输入图像2；
 *  Mat &dst:           目标输出图像；
=====================================================================
*/
void BinMatMinus(Mat src1, Mat src2, Mat &dst)
{
    // 检查是否为二值图像
    if(src1.type() != CV_8U || src2.type() != CV_8U)
    {
        cout<<"Source Images Format Wrong! "<<endl;
        return ;
    }
    // 检查两图像尺寸是否相等
    if(src1.size != src2.size)
    {
        cout<<"Source Images' Sizes Are Not The Same! "<<endl;
        return ;
    }

    Mat tmp;
    tmp.create(src1.size(), CV_8U);
    src1.copyTo(tmp);

    MatIterator_<uchar> it1, it1end;
    MatIterator_<uchar> it2, it2end;
    MatIterator_<uchar> tmpit, tmpend;

    for(it1 = src1.begin<uchar>(), it1end = src1.end<uchar>(),
        it2 = src2.begin<uchar>(), it2end = src2.end<uchar>(),
        tmpit = tmp.begin<uchar>(), tmpend = tmp.end<uchar>();

        it1 != it1end, it2 != it2end, tmpit != tmpend;
        ++it1, ++it2, ++tmpit)
    {
        if( *it2 > 0)
        {
            *tmpit = 0;
        }
    }
    tmp.copyTo(dst);
}

/*===================================================================
 * 函数名：BinMatDoubleJoinMinus
 *
 * 功能：两个HSV并运算，再进行两个RGB减运算；
 *
 * 参数：
 *  Mat src:                输入图像；
 *  Mat &dst:               输出图像；
 *  HSVInfo HSVInfo1:       捕获目标HSV信息1；
 *  HSVInfo HSVInfo2:       捕获目标HSV信息2；
 *  RGBInfo RGBVoice1:      滤除噪声RGB信息1；
 *  RGBInfo RGBVoice2:      滤除噪声RGB信息2；
=====================================================================
*/
void BinMatDoubleJoinMinus(Mat src, Mat &dst,
                           HSVInfo HSVInfo1, HSVInfo HSVInfo2,
                           RGBInfo RGBVoice1, RGBInfo RGBVoice2)
{
    // 检查是否为二值图像
    if(src.type() != CV_8UC3)
    {
        cout<<"Source Images Format Wrong! "<<endl;
        return ;
    }

    Mat tmp;
    tmp.create(src.size(), CV_8U);

    Mat imgHSV;
    imgHSV.create(src.size(), src.type());
    cvtColor(src, imgHSV, CV_BGR2HSV);
//    src.copyTo(tmp);

    int HMin1, HMax1, SMin1, SMax1, VMin1, VMax1;
    int HMin2, HMax2, SMin2, SMax2, VMin2, VMax2;
    int RMin1, RMax1, GMin1, GMax1, BMin1, BMax1;
    int RMin2, RMax2, GMin2, GMax2, BMin2, BMax2;

    HMin1 = HSVInfo1.HMin; HMax1 = HSVInfo1.HMax;
    SMin1 = HSVInfo1.SMin; SMax1 = HSVInfo1.SMax;
    VMin1 = HSVInfo1.VMin; VMax1 = HSVInfo1.VMax;

    HMin2 = HSVInfo2.HMin; HMax2 = HSVInfo2.HMax;
    SMin2 = HSVInfo2.SMin; SMax2 = HSVInfo2.SMax;
    VMin2 = HSVInfo2.VMin; VMax2 = HSVInfo2.VMax;

    RMin1 = RGBVoice1.RMin; RMax1 = RGBVoice1.RMax;
    GMin1 = RGBVoice1.GMin; GMax1 = RGBVoice1.RMax;
    BMin1 = RGBVoice1.BMin; BMax1 = RGBVoice1.BMax;

    RMin2 = RGBVoice2.RMin; RMax2 = RGBVoice2.RMax;
    GMin2 = RGBVoice2.GMin; GMax2 = RGBVoice2.RMax;
    BMin2 = RGBVoice2.BMin; BMax2 = RGBVoice2.BMax;

    MatIterator_<Vec3b> HSVit, HSVitend;
    MatIterator_<Vec3b> RGBit, RGBitend;
    MatIterator_<uchar> tmpit, tmpend;

    for(HSVit = imgHSV.begin<Vec3b>(), HSVitend = imgHSV.end<Vec3b>(),
        RGBit = src.begin<Vec3b>(), RGBitend = src.end<Vec3b>(),
        tmpit = tmp.begin<uchar>(), tmpend = tmp.end<uchar>();

        HSVit != HSVitend, RGBit != RGBitend, tmpit != tmpend;
        ++HSVit, ++RGBit, ++tmpit)
    {
        if(((*HSVit)[0] >= HMin1 && (*HSVit)[0] <= HMax1
                && (*HSVit)[1] >= SMin1 && (*HSVit)[1] <= SMax1
                && (*HSVit)[2] >= VMin1 && (*HSVit)[2] <= VMax1)

                || ((*HSVit)[0] >= HMin2 && (*HSVit)[0] <= HMax2
                && (*HSVit)[1] >= SMin2 && (*HSVit)[1] <= SMax2
                && (*HSVit)[2] >= VMin2 && (*HSVit)[2] <= VMax2))
        {
            if(((*RGBit)[2] >= RMin1 && (*RGBit)[2] <= RMax1
                    && (*RGBit)[1] >= GMin1 && (*RGBit)[1] <= GMax1
                    && (*RGBit)[0] >= BMin1 && (*RGBit)[0] <= BMax1)

                    || ((*RGBit)[2] >= RMin2 && (*RGBit)[2] <= RMax2
                    && (*RGBit)[1] >= GMin2 && (*RGBit)[1] <= GMax2
                    && (*RGBit)[0] >= BMin2 && (*RGBit)[0] <= BMax2))
                *tmpit = 0;
            else
                *tmpit = 255;
        }
        else
        {
            *tmpit = 0;
        }
    }
    tmp.copyTo(dst);
}

/*===================================================================
 * 函数名：StrenchImage
 *
 * 功能：图像纵横方向拉伸；
 *
 * 参数：
 *  Mat src:            输入图像；
 *  Mat &dst:           输出图像；
 *  double ratioX:      X方向拉伸比例；
 *  double ratioY:      Y方向拉伸比例；
=====================================================================
*/
void StrenchImage(Mat src, Mat &dst, double ratioX, double ratioY)
{
    // 检查是否存在图像
    if(src.size().width <= 0 || src.size().height <= 0)
    {
        cout<<"Strench Error: No such file! "<<endl;
        return ;
    }

    Mat tmp;
    Size dsize = Size(src.cols*ratioX, src.rows*ratioY);
    resize(src, tmp, dsize, 0, 0);
    tmp.copyTo(dst);
}


