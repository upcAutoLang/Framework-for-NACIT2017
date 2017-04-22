#include "ncvision.h"

NCVision::NCVision()
{
//    InitVision();
    // VInfo初始化；
    InitVisionInfo();
}


//**********************************
//          各种初始化
//**********************************

/*===================================================================
 * 函数：StartCamerasGrab()
 *
 * 说明：
 *  俯视、前视摄像头开始捕获视频；
/*===================================================================
 */
void NCVision::StartCamerasGrab()
{
    DownCamera.StartGrab();
    FwdCamera.StartGrab();
}

// （调试，实时采集）俯视、前视摄像头开始捕获视频
void NCVision::StartCamerasGrabDebug()
{
    DownCamera.StartGrab();
    FwdCamera.StartGrab();
}

//**********************************
//          图像捕获
//**********************************

/*===================================================================
 * 函数：GrabFrames()
 *
 * 说明：
 *  实际操作中使用：实时获取俯视、前视原图像；
/*===================================================================
 */
void NCVision::GrabFrames()
{
    DownCamera.CatchOneFrame();
    FwdCamera.CatchOneFrame();
}

// （调试，实时采集）获取俯视、前视原图像
void NCVision::GrabFramesDebug()
{
    DownCamera.CatchOneFrame();
    FwdCamera.CatchOneFrameFromUSB();
}

/*===================================================================
 * 函数：GrabFramesFromVideos()
 *
 * 说明：
 *  调试过程中使用：从视频中获取俯视、前视原图像；
/*===================================================================
 */
void NCVision::GrabFramesFromVideos()
{
    DownCamera.CatchOneFrameFromVideo();
    FwdCamera.CatchOneFrameFromVideo();
}

//**********************************
//        数据传递函数
//**********************************

/*===================================================================
 * 函数：sendFrames()
 *
 * 说明：
 *  将俯视、前视原图像传给ImgPro；
/*===================================================================
 */
void NCVision::sendFrames()
{
    sendDownFrame();
    sendFwdFrame();
}

/*===================================================================
 * 函数：sendDownFrame()
 *
 * 说明：
 *  将俯视原图像传给ImgPro；
/*===================================================================
 */
void NCVision::sendDownFrame()
{
    Mat tmp;
    DownCamera.getFrame(tmp);
    ImgPro.setDownFrame(tmp);
}

/*===================================================================
 * 函数：sendFwdFrame()
 *
 * 说明：
 *  将前视原图像传给ImgPro；
/*===================================================================
 */
void NCVision::sendFwdFrame()
{
    Mat tmp;
    FwdCamera.getFrame(tmp);
    ImgPro.setFwdFrame(tmp);
}



//**********************************
//        set, get函数
//**********************************

/*===================================================================
 * 函数：setSensorHeight(double src)
 *
 * 说明：
 *  设置传感器工作深度值（不同的任务有不同的深度值）；
 *
 * 参数：
 *  double src:         传感器工作深度值；
/*===================================================================
 */
void NCVision::setSensorHeight(double src)
{
    DownModel.setSensorHeight(src);
}

/*===================================================================
 * 函数：getLineWidthInCam(double &dst)
 *
 * 说明：
 *  获取图像坐标系中导引路径应有的宽度；
 *
 * 参数：
 *  double &dst:         图像坐标系中导引路径应有的宽度；
/*===================================================================
 */
void NCVision::getLineWidthInCam(double &dst)
{
    DownModel.getLineWidthInCam(dst);
}

/*===================================================================
 * 函数：getLineLengthInCam(double &dst)
 *
 * 说明：
 *  获取图像坐标系中导引路径应有的长度；
 *
 * 参数：
 *  double &dst:         图像坐标系中导引路径应有的长度；
/*===================================================================
 */
void NCVision::getLineLengthInCam(double &dst)
{
    DownModel.getLineLengthInCam(dst);
}

/*===================================================================
 * 函数：getDownFrame(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头原图像；
 *
 * 参数：
 *  Mat &dst:         俯视摄像头原图像；
/*===================================================================
 */
void NCVision::getDownFrame(Mat &dst)
{
    DownCamera.getFrame(dst);
}

/*===================================================================
 * 函数：getFwdFrame(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头原图像；
 *
 * 参数：
 *  Mat &dst:         前视摄像头原图像；
/*===================================================================
 */
void NCVision::getFwdFrame(Mat &dst)
{
    FwdCamera.getFrame(dst);
}

/*===================================================================
 * 函数：getDownPrePro(Mat &dst)
 *
 * 说明：
 *  获取俯视摄像头预处理图像；
 *
 * 参数：
 *  Mat &dst:         俯视摄像头预处理图像；
/*===================================================================
 */
void NCVision::getDownPrePro(Mat &dst)
{
    ImgPro.getDownFramePrePro(dst);
}

/*===================================================================
 * 函数：getFwdPrePro(Mat &dst)
 *
 * 说明：
 *  获取前视摄像头预处理图像；
 *
 * 参数：
 *  Mat &dst:         前视摄像头预处理图像；
/*===================================================================
 */
void NCVision::getFwdPrePro(Mat &dst)
{
    ImgPro.getFwdFramePrePro(dst);
}

/*===================================================================
 * 函数：getBinLine(Mat &dst)
 *
 * 说明：
 *  获取寻线二值图；
 *
 * 参数：
 *  Mat &dst:         寻线二值图；
/*===================================================================
 */
void NCVision::getBinLine(Mat &dst)
{
    ImgPro.getimgBinLine(dst);
}

/*===================================================================
 * 函数：getBinDoor(Mat &dst)
 *
 * 说明：
 *  获取过门二值图；
 *
 * 参数：
 *  Mat &dst:         过门二值图；
/*===================================================================
 */
void NCVision::getBinDoor(Mat &dst)
{
    ImgPro.getimgBinDoor(dst);
}

/*===================================================================
 * 函数：getBinBallsFar(Mat &dst)
 *
 * 说明：
 *  获取撞球远状态图像；
 *
 * 参数：
 *  Mat &dst:         撞球远状态二值图；
/*===================================================================
 */
void NCVision::getBinBallsFar(Mat &dst)
{
    ImgPro.getimgBinBallsFar(dst);
}

/*===================================================================
 * 函数：getBinRedBall(Mat &dst)
 *
 * 说明：
 *  获取撞球NEAR, ATTACH状态下红球二值化图像；
 *
 * 参数：
 *  Mat &dst:         撞球NEAR, ATTACH状态下红球二值化图像；
/*===================================================================
 */
void NCVision::getBinRedBall(Mat &dst)
{
    ImgPro.getimgBinRedBall(dst);
}

/*===================================================================
 * 函数：getBinYelGreBalls(Mat &dst)
 *
 * 说明：
 *  获取撞球NEAR, ATTACH状态下黄绿球二值化图像；
 *
 * 参数：
 *  Mat &dst:         撞球NEAR, ATTACH状态下黄绿球二值化图像；
/*===================================================================
 */
void NCVision::getBinYelGreBalls(Mat &dst)
{
    ImgPro.getimgBinYelGreBalls(dst);
}



/*===================================================================
 * 函数：getVisionInfo(VisionInfo &dst)
 *
 * 说明：
 *  获得图像信息；
 *
 * 参数：
 *  VisionInfo &dst:         图像信息；
/*===================================================================
 */
void NCVision::getVisionInfo(VisionInfo &dst)
{
    dst = VInfo;
}


//***********************************
//          图像处理函数
//***********************************

/*===================================================================
 * 函数：FramesPrePro()
 *
 * 说明：
 *  俯视、前视图像预处理；
/*===================================================================
 */
void NCVision::FramesPrePro()
{
    ImgPro.DownFramePrePro();
    ImgPro.FwdFramePrePro();
}

/*===================================================================
 * 函数：BinLinePro()
 *
 * 说明：
 *  寻线任务，二值化处理；
/*===================================================================
 */
void NCVision::BinLinePro()
{
    ImgPro.BinLine();
}

/*===================================================================
 * 函数：BinDoorPro()
 *
 * 说明：
 *  过门任务，二值化处理；
/*===================================================================
 */
void NCVision::BinDoorPro()
{
    ImgPro.BinDoor();
}

/*===================================================================
 * 函数：BinBallsPro()
 *
 * 说明：
 *  撞球任务，二值化处理；
 *
 * 参数：
 *  int state:      当前与球之间距离的状态
/*===================================================================
 */
void NCVision::BinBallsPro(int state)
{
    ImgPro.BinBalls(state);
}

/*===================================================================
 * 函数：BinBallsPro()
 *
 * 说明：
 *  风格过门任务，二值化处理；
/*===================================================================
 */
void NCVision::BinStyleDoor(int state)
{
    ImgPro.BinStyleDoor(state);
}

/*===================================================================
 * 函数：BinBallsPro()
 *
 * 说明：
 *  鱼雷切板任务，二值化处理；
/*===================================================================
 */
void NCVision::BinTorBoard()
{
    ImgPro.BinTorBoard();
}

/*===================================================================
 * 函数：BinAnchor()
 *
 * 说明：
 *  投放标志物任务，二值化处理；
/*===================================================================
 */
void NCVision::BinAnchor()
{
    ImgPro.BinAnchors();
}

void NCVision::BinOctCoins()
{
    ImgPro.BinOctCoins();
}
