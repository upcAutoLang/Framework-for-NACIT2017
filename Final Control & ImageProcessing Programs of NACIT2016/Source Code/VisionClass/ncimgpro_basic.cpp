#include "ncimgpro.h"

/*===================================================================
 * 构造函数：NCImgPro()
 *
 * 说明：
 *  对图像处理集合初始化；
/*===================================================================
 */
NCImgPro::NCImgPro()
{
    cutwidth = 640;
    cutheight = 480;

    InitCameraUndist();
    InitHSVRGBInfo();
}



/*===================================================================
 * 函数：setDownFrame(Mat src)
 *
 * 说明：
 *  为俯视摄像头原图像赋值
 *
 * 参数：
 *  Mat src:        俯视摄像头原图像；
/*===================================================================
 */
void NCImgPro::setDownFrame(Mat src)
{
    // 检测原图像
    if(src.size().width <= 0 || src.size().height <= 0)
    {
        cout<<endl<<"Down Frame Get Failed!"<<endl;
        cout<<"Have No source Image!"<<endl;
        return ;
    }
    src.copyTo(DownFrame);
}


/*===================================================================
 * 函数：setFwdFrame(Mat src)
 *
 * 说明：
 *  为前视摄像头原图像赋值
 *
 * 参数：
 *  Mat src:        前视摄像头原图像；
/*===================================================================
 */
void NCImgPro::setFwdFrame(Mat src)
{
    // 检测原图像
    if(src.size().width <= 0 || src.size().height <= 0)
    {
        cout<<endl<<"Forward Frame Get Failed!"<<endl;
        cout<<"Have No source Image!"<<endl;
        return ;
    }
    src.copyTo(FwdFrame);
}

/*===================================================================
 * 函数：getDownFrame(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头原图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为俯视摄像头原图像；
/*===================================================================
 */
void NCImgPro::getDownFrame(Mat &dst)
{
    DownFrame.copyTo(dst);
}

/*===================================================================
 * 函数：getFwdFrame(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头原图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为前视摄像头原图像；
/*===================================================================
 */
void NCImgPro::getFwdFrame(Mat &dst)
{
    FwdFrame.copyTo(dst);
}

/*===================================================================
 * 函数：getDownFrameUndist(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头消除畸变后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为俯视摄像头消除畸变后的图像；
/*===================================================================
 */
void NCImgPro::getDownFrameUndist(Mat &dst)
{
    DownFrameUdt.copyTo(dst);
}

/*===================================================================
 * 函数：getFwdFrameUndist(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头消除畸变后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为前视摄像头消除畸变后的图像；
/*===================================================================
 */
void NCImgPro::getFwdFrameUndist(Mat &dst)
{
    FwdFrameUdt.copyTo(dst);
}

/*===================================================================
 * 函数：getDownFrameStrench(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头拉伸后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为俯视摄像头拉伸后的图像；
/*===================================================================
 */
void NCImgPro::getDownFrameStrench(Mat &dst)
{
    DownStrench.copyTo(dst);
}

/*===================================================================
 * 函数：getFwdFrameStrench(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头拉伸后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为前视摄像头拉伸后的图像；
/*===================================================================
 */
void NCImgPro::getFwdFrameStrench(Mat &dst)
{
    FwdStrench.copyTo(dst);
}

/*===================================================================
 * 函数：getDownFramePrePro(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头预处理后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为俯视摄像头预处理后的图像；
/*===================================================================
 */
void NCImgPro::getDownFramePrePro(Mat &dst)
{
    DownImg.copyTo(dst);
}

/*===================================================================
 * 函数：getDownFramePrePro(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头预处理后的图像；
 *
 * 参数：
 *  Mat &dst:       输出Mat，Mat内容为前视摄像头预处理后的图像；
/*===================================================================
 */
void NCImgPro::getFwdFramePrePro(Mat &dst)
{
    FwdImg.copyTo(dst);
}

/*===================================================================
 * 函数：DownFrameCalib()
 *
 * 说明：
 *  俯视摄像头标定重绘；
 *
 * 1、检查内参数矩阵与畸变矩阵是否存在；
 * 2、俯视摄像头标定重绘；
/*===================================================================
 */
void NCImgPro::DownFrameCalib()
{
    // 检查内参数矩阵与畸变矩阵是否存在
    if(DownIntrinMat.size().height <= 0
            || DownIntrinMat.size().width <= 0
            || DownDistortMat.size().height <= 0
            || DownDistortMat.size().width <= 0 )
    {
        cout<<endl<<"Down Frame Calibration Failed! "<<endl;
        cout<<"Didn't Found DownCamera's IntrinMat or DistortMat!"<<endl;
        return ;
    }
    UndistortCameraFrame(DownFrame, DownFrameUdt, FrameSize, DownIntrinMat, DownDistortMat);
}

/*===================================================================
 * 函数：FwdFrameCalib()
 *
 * 说明：
 *  前视摄像头标定重绘；
 *
 * 1、检查内参数矩阵与畸变矩阵是否存在；
 * 2、前视摄像头标定重绘；
/*===================================================================
 */
void NCImgPro::FwdFrameCalib()
{
    // 检查内参数矩阵与畸变矩阵是否存在
    if(FwdIntrinMat.size().height <= 0
            || FwdIntrinMat.size().width <= 0
            || FwdDistortMat.size().height <= 0
            || FwdDistortMat.size().width <= 0 )
    {
        cout<<endl<<"Forward Frame Calibration Failed! "<<endl;
        cout<<"Didn't Found Forward Camera's IntrinMat or DistortMat!"<<endl;
        return ;
    }
    UndistortCameraFrame(FwdFrame, FwdFrameUdt, FrameSize, FwdIntrinMat, FwdDistortMat);
}

/*===================================================================
 * 函数：DownFrameCut()
 *
 * 说明：
 *  俯视摄像头裁剪；
 *
 * 1、检查消除畸变后的俯视摄像头图像是否存在；
 * 2、俯视摄像头裁剪；
/*===================================================================
 */
void NCImgPro::DownFrameCut()
{
    // 检查消除畸变后的俯视摄像头图像是否存在
    if(DownStrench.size().width <= 0 || DownStrench.size().height <= 0)
    {
        cout<<endl<<"Down Strench Frame is None so Can't Cut! "<<endl;
        return ;
    }
    CutCenterImage(DownStrench, DownImg, cutwidth, cutheight);
}

/*===================================================================
 * 函数：FwdFrameCut()
 *
 * 说明：
 *  前视摄像头裁剪；
 *
 * 1、检查消除畸变后的前视摄像头图像是否存在；
 * 2、前视摄像头裁剪；
 *
 * 注：
 *  对29号前视摄像头，需要将摄像头采集图像稍微向右偏移一些；
/*===================================================================
 */
void NCImgPro::FwdFrameCut()
{
    // 检查消除畸变且拉伸后的前视摄像头图像是否存在
    if(FwdStrench.size().width <= 0 || FwdStrench.size().height <= 0)
    {
        cout<<endl<<"Forward Strench Frame is None so Can't Cut! "<<endl;
        return ;
    }
    // 裁剪图像，对29号前视摄像头，需要将摄像头采集图像稍微向右偏移一些；
    CutCenterImage(FwdStrench, FwdImg, cutwidth, cutheight, 0);
}

/*===================================================================
 * 函数：DownFrameStrench()
 *
 * 说明：
 *  俯视摄像头拉伸；
 *
 * 1、检查消除畸变后的前视摄像头图像是否存在；
 * 2、俯视摄像头拉伸；
/*===================================================================
 */
void NCImgPro::DownFrameStrench()
{
    // 检查消除畸变后的前视摄像头图像是否存在
    if(DownFrameUdt.size().width <= 0 || DownFrameUdt.size().height <= 0)
    {
        cout<<endl<<"Down Undistort Frame is NULL so Can't Strench! "<<endl;
        return ;
    }
    StrenchImage(DownFrameUdt, DownStrench, 1.2, 1.2);
}

/*===================================================================
 * 函数：FwdFrameStrench()
 *
 * 说明：
 *  前视摄像头拉伸；
 *
 * 1、检查消除畸变后的前视摄像头图像是否存在；
 * 2、前视摄像头拉伸；
/*===================================================================
 */
void NCImgPro::FwdFrameStrench()
{
    // 检查消除畸变后的前视摄像头图像是否存在
    if(FwdFrameUdt.size().width <= 0 || FwdFrameUdt.size().height <= 0)
    {
        cout<<endl<<"Fwd Undistort Frame is NULL so Can't Strench! "<<endl;
        return ;
    }
    StrenchImage(FwdFrameUdt, FwdStrench, 1, 1.2);
}


/*===================================================================
 * 函数：DownFramePrePro()
 *
 * 说明：
 *  俯视摄像头图像预处理；
 *
 * 1、俯视摄像头标定；
 * 2、俯视摄像头拉伸；
 * 3、俯视摄像头裁剪；
/*===================================================================
 */
void NCImgPro::DownFramePrePro()
{
    DownFrameCalib();
    DownFrameStrench();
    DownFrameCut();
}

/*===================================================================
 * 函数：FwdFramePrePro()
 *
 * 说明：
 *  前视摄像头图像预处理；
 *
 * 1、前视摄像头标定；
 * 2、前视摄像头拉伸；
 * 3、前视摄像头裁剪；
/*===================================================================
 */
void NCImgPro::FwdFramePrePro()
{
    FwdFrameCalib();
    FwdFrameStrench();
    FwdFrameCut();
}

/*===================================================================
 * 函数：getimgBinDoor(Mat &dst)
 *
 * 说明：
 *  获取门的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinDoor(Mat &dst)
{
    imgBinDoor.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinLine(Mat &dst)
 *
 * 说明：
 *  获取线的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinLine(Mat &dst)
{
    imgBinLine.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinLineID(Mat &dst)
 *
 * 说明：
 *  获取过门任务前视摄像头中，导引路径的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinLineID(Mat &dst)
{
    imgBinLineID.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinBallsFar(Mat &dst)
 *
 * 说明：
 *  获取撞球任务中，远距离FAR状态下，三个球的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinBallsFar(Mat &dst)
{
    imgBinBallsFar.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinRedBall(Mat &dst)
 *
 * 说明：
 *  获取撞球任务中，近距离NEAR、即将碰撞ATTACH状态下，红球的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinRedBall(Mat &dst)
{
    imgBinRed.copyTo(dst);
}

void NCImgPro::getimgBinYellowBall(Mat &dst)
{
    imgBinYellow.copyTo(dst);
}

void NCImgPro::getimgBinGreenBall(Mat &dst)
{
    imgBinGreen.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinYelGreBalls(Mat &dst)
 *
 * 说明：
 *  获取撞球任务中，近距离NEAR、即将碰撞ATTACH状态下，黄绿两球的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinYelGreBalls(Mat &dst)
{
    imgBinYelGre.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinStyleDoor(Mat &dst)
 *
 * 说明：
 *  获取风格过门任务的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinStyleDoor(Mat &dst)
{
    imgBinStyleDoor.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinStyleDoor(Mat &dst)
 *
 * 说明：
 *  获取鱼雷切板的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinTorBoard(Mat &dst)
{
    imgBinTorBoard.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinStyleDoor(Mat &dst)
 *
 * 说明：
 *  获取鱼雷切板红色边缘的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinTorRedEdge(Mat &dst)
{
    imgBinTorRedEdge.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinAnchorBox(Mat &dst)
 *
 * 说明：
 *  获取投放标志物箱子外围的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinAnchorBox(Mat &dst)
{
    imgBinAnchorBox.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinAnchorCover(Mat &dst)
 *
 * 说明：
 *  获取投放标志物任务中盖子的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinAnchorCover(Mat &dst)
{
    imgBinAnchorCover.copyTo(dst);
}

/*===================================================================
 * 函数：getimgBinAnchorPattern(Mat &dst)
 *
 * 说明：
 *  获取鱼雷切板黄色图案的二值化图像；
/*===================================================================
 */
void NCImgPro::getimgBinAnchorPattern(Mat &dst)
{
    imgBinAnchorPattern.copyTo(dst);
}

void NCImgPro::getimgBinOctRedCoins(Mat &dst)
{
    imgBinRedCoins.copyTo(dst);
}
