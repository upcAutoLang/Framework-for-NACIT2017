#include "nccamera.h"

/*===================================================================
 * 构造函数：NCCamera()
 *
 * 说明：
 *  基本不用这个构造函数，要用另外一个构造函数；
/*===================================================================
 */
NCCamera::NCCamera()
{
    width = 780;
    height = 580;
}

/*===================================================================
 * 构造函数：NCCamera(int CamNum, int wid, int heig)
 *
 * 说明：
 *  1、设定捕捉图像大小（默认780*580）
 *  2、根据输入的摄像头编号，获得特定摄像头的GUID
 *  3、连接摄像头
 *
 * 注：
 *  错误1001：设备已打开；该错误在打开第一个之后的摄像头会出现。此处将该错误手动置0；
/*===================================================================
 */
NCCamera::NCCamera(int CamNum, int wid, int heig)
{
    width = wid;
    height = heig;

    setCameraGUID(CamNum);
    InitCamera(CamNum);
}

// 设置摄像头GUID
/*===================================================================
 * 函数：setCameraGUID
 *
 * 说明：
 *  给对应编号的摄像机赋GUID值；
 *
 * 参数：
 *  int CamNum:             摄像机编号，本项目中只接受28, 29, 30三个值；
/*===================================================================
 */
void NCCamera::setCameraGUID(int CamNum)
{
    switch(CamNum)
    {
    case 28:
        CurGuid.Low = 302722228;
        CurGuid.High = 673537;
        break;
    case 29:
        CurGuid.Low = 302722229;
        CurGuid.High = 673537;
        break;
    case 30:
        // 30号摄像头的GUID
        CurGuid.Low = 302722230;
        CurGuid.High = 673537;
        break;
    case 0:
        // USB摄像头
        CurGuid.Low = 0;
        CurGuid.High = 0;
        video = VideoCapture(0);
        break;
    default:
        cout<<"Camera Number Wrong!"<<endl;
        break;
    }
}


/*===================================================================
 * 析构函数：~NCCamera()
 *
 * 说明：
 *  退出FireWire模块；
/*===================================================================
 */
NCCamera::~NCCamera()
{
//    ReleaseCamera();
    // 退出该模块
    FGExitModule();
}

/*===================================================================
 * 函数：StartGrab()
 *
 * 说明：
 *  1、开启DMA逻辑序列；
 *  2、开启图像采集；
/*===================================================================
 */
void NCCamera::StartGrab()
{
    if(CurGuid.High != 0 && CurGuid.Low != 0)
    {
        // 开启DMA逻辑序列
        if(Result == FCE_NOERROR)
        {
            Result = Camera.OpenCapture();
    //        cout<<"OpenCapture Done!"<<endl;
        }

        // 开启图像采集
        if(Result == FCE_NOERROR)
        {
            Result = Camera.StartDevice();
    //        cout<<"StartDevice Done!"<<endl;
        }
    }
    else
        video >> Frame;
}

/*===================================================================
 * 函数：CatchOneFrame()
 *
 * 说明：
 *  1、获取FPFRAME格式的图像
 *  2、指针赋值，将FPFRAME格式的图像赋给Mat图像；
 *  3、颜色空间转换，RGB->BGR；
 *  4、StandBy序列中的缓存图像销毁(PutFrame)；
 *  5、将获得的图像传给Camera类中的Mat格式的Frame；
/*===================================================================
 */
void NCCamera::CatchOneFrame()
{
//    FGFRAME Frame;
    Mat FrameGrab(height, width, CV_8UC3);
    Mat FrameRGB2BGR;
    FrameRGB2BGR.create(FrameGrab.size(), FrameGrab.type());

//    UINT32 Result;
    Result = Camera.GetFrame(&FGFrame, 5000);
    FrameGrab.data = FGFrame.pData;
    cvtColor(FrameGrab, FrameRGB2BGR, CV_BGR2RGB);

    // Return Frame to module
    if(Result==FCE_NOERROR)
        Result = Camera.PutFrame(&FGFrame);
    FrameRGB2BGR.copyTo(Frame);
}

// 从USB摄像头采集图像，将数据读入Mat格式的Frame中；
void NCCamera::CatchOneFrameFromUSB()
{
    video >> Frame;
}

/*===================================================================
 * 函数：setVideoCapture(string path)
 *
 * 说明：
 *  调试用程序，对视频赋文件地址；
/*===================================================================
 */
void NCCamera::setVideoCapture(string path)
{
    cp = VideoCapture(path);
}

/*===================================================================
 * 函数：CatchOneFrameFromVideo()
 *
 * 说明：
 *  调试用程序，从视频中采集图像，将数据读入Mat格式的Frame中；
/*===================================================================
 */
void NCCamera::CatchOneFrameFromVideo()
{
    if(!cp.isOpened())
    {
        cout<<endl<<"Didn't Found This Video! "<<endl;
        return ;
    }

    Mat frame;
    cp>>frame;
    frame.copyTo(Frame);
}

/*===================================================================
 * 函数：ReleaseCamera()
 *
 * 说明：
 *  1、停止设备
 *  2、图像采集关闭
 *  3、退出模块之前断开连接
/*===================================================================
 */
void NCCamera::ReleaseCamera()
{ 
    Camera.StopDevice();
    Camera.CloseCapture();
    Camera.Disconnect();
}

/*===================================================================
 * 函数：getFrame(Mat &dst)
 *
 * 参数
 *  Mat &dst:       输出的Mat格式图像；
 *
 * 说明：
 *  将Camera中的Mat图像传出；
/*===================================================================
 */
void NCCamera::getFrame(Mat &dst)
{
    Frame.copyTo(dst);
}

/*===================================================================
 * 函数：InitCameraParam()
 *
 * 说明：
 *  1、设置图像格式为RGB8；
 *  2、设置X、Y方向分辨率；
 *  3、设置数据包尺寸（默认8192 / 16 = 512）；
 *  4、
/*===================================================================
 */
void NCCamera::InitCameraParam()
{
    FGPINFO    XSize, YSize, PacketSize;
    // 此处有n个摄像头，则除以n
    // PACKAGESIZE的合理大小可以使两个摄像头正常工作，不会出现一个摄像头捕获图像有延时，一个摄像头捕获图像无延时的情况；
    // 另外，PACKAGESIZE越小，图像捕获帧率越低
    UINT32     PACKAGESIZE = 8192 / 48;

    // 设置像素格式为RGB8
    Camera.SetParameter(FGP_IMAGEFORMAT,MAKEIMAGEFORMAT(RES_SCALABLE,CM_RGB8,FR_1_875));

    // Set XSize
    // 设置X方向分辨率
    do
    {
        XSize.IsValue = width;
        Result = Camera.SetParameter(FGP_XSIZE, XSize.IsValue);
//        cout<<"XSize Ok and Error Code: "<<Result<<endl;
        if(Result==FCE_ADJUSTED)
            Result=FCE_NOERROR;
    }while(Result!=FCE_NOERROR);

    // Set YSize
    // 设置Y方向分辨率
    do
    {
        YSize.IsValue = height;
        Result=Camera.SetParameter(FGP_YSIZE, YSize.IsValue);
//        cout<<"YSize Ok and Error Code: "<<Result<<endl;
        if(Result==FCE_ADJUSTED)
            Result=FCE_NOERROR;
    }while(Result!=FCE_NOERROR);

    // Adapt PacketSize
    // 设置合适的数据包尺寸
    do
    {
        // Get PacketSize range
        Result=Camera.GetParameterInfo(FGP_PACKETSIZE,&PacketSize);
        PacketSize.IsValue = PACKAGESIZE;
        // Set PacketSize
        Result=Camera.SetParameter(FGP_PACKETSIZE, PacketSize.IsValue);
//        cout<<"PacketSize Ok and Error Code: "<<Result<<endl;
        if(Result==FCE_ADJUSTED)
            Result=FCE_NOERROR;
    }while(Result!=FCE_NOERROR);

    // Get current AOI Settings
    Result=Camera.GetParameterInfo(FGP_PACKETSIZE,&PacketSize);
    Result=Camera.GetParameterInfo(FGP_XSIZE,&XSize);
    Result=Camera.GetParameterInfo(FGP_YSIZE,&YSize);

    // Get Parameters
    if(Result == FCE_NOERROR)
    {
//        cout<<"Geting Parameters..."<<Result<<endl;
        Camera.GetParameter(FGP_XSIZE, &width);
        Camera.GetParameter(FGP_YSIZE, &height);
//        cout<<width<<" "<<height<<endl;
//        cout<<"Get Parameters Done."<<endl;
    }

}

