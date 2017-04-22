#include <nccamera.h>
#include <ncmodel.h>
#include <ncheader.h>
#include <ncfwdmodel.h>
#include <ncdownmodel.h>
#include <ncimgpro.h>
#include <ncvision.h>

//**************************************
//          NCCamera
//**************************************

/*===================================================================
 * 函数：InitCamera(int CamNum)
 *
 * 说明：摄像头初始化
 *  1、初始化FireWire库；
 *  2、得到已经连接的节点数量；
 *  3、针对第二个打开的摄像头，得到节点数量的步骤中会返回错误1001（设备已打开），此处手动忽略；
 *  4、连接节点，若失败则输出错误信息；
 *
 * 参数：
 *  int CamNum:         摄像头编号；
/*===================================================================
 */
void NCCamera::InitCamera(int CamNum)
{
    if(CamNum != 0)
    {
        // 初始化FireWire库
        Result = FGInitModule(NULL);

        // 得到已经连接的节点数量
        if(Result == FCE_NOERROR)
        {
            Result = FGGetNodeList(NodeInfo, 3, &NodeCnt);
//            cout<<"GetNodeList Done!"<<endl;
        }

        // 针对第二个打开的摄像头，得到节点数量的步骤中会返回错误1001（设备已打开），此处手动忽略；
        if(Result == 1001)
            Result = 0;

        // 连接节点
        if(Result==FCE_NOERROR)
        {
            Result = Camera.Connect(&CurGuid);
//            cout<<"CurGuid Low & High: "<<CurGuid.Low<<" "<<CurGuid.High<<endl;
        }
        else
            cout<<"Camera "<<CamNum<<" Connect Fail!"<<endl;
    }
}

//**************************************
//             NCModel
//**************************************

/*===================================================================
 * 函数：InitBaseModel()
 *
 * 说明：
 *  初始化参数，函数内容同于基类NCModel的构造函数NCModel()，主要给派生类的
 * 构造函数传值；
/*===================================================================
 */
void NCModel::InitBaseModel()
{
    // 池底到水面深度
    H = 10;

    // 深度传感器测值，此处为我们设定的深度
    // 编写该处时随便写了0.50
    h_sen = 0.50;

    // 深度传感器与俯视摄像头垂直方向距离
    h_sen_down = 0.100;
//    // 摄像头距池底距离(cb: camera, bottom)
//    h_cb = H - h_sen - h_sd;

    // 航行器实际长、宽、高
    LEN_vr = 1.100;
    WID_vr = 0.550;
    H_vr = 0.450;
    // 视野角度
    ViewAngle = 140;
    // 摄像机焦距
    f = 0.0013;


    // 深度传感器与前视摄像头前后方向距离
    l_sen_fwd = 0.150;
}

//**************************************
//           NCDownModel
//**************************************

/*===================================================================
 * 函数：InitDownModel()
 *
 * 说明：
 *  初始化参数，用给派生类NCDownModel的构造函数；
/*===================================================================
 */
void NCDownModel::InitDownModel()
{
    //********************************************
    //  04.23更新，将前视摄像头暂时性赋给俯视摄像头
    //********************************************
    C = 235.4;


    //********************************************
    //             比赛官方数据
    //********************************************
    LEN_line = 1.200;
    WID_line = 0.150;
    h_line_bot = 0.450;


    //********************************************
    //             计算得来的部分
    //********************************************

    // 计算俯视摄像头深度；
    CalcuDownCamDepth();

    // 计算在图像坐标系中航行器长、宽、高的映射大小
    CalcuDimenInCam();
    CalcuLenLineInCam();
}

//**************************************
//           NCDownModel
//**************************************

/*===================================================================
 * 函数：InitFwdModel()
 *
 * 说明：
 *  初始化参数，用给派生类NCFwdModel的构造函数；
/*===================================================================
 */
void NCFwdModel::InitFwdModel()
{
    //********************************************
    //        04.23更新前视摄像头C值
    //********************************************
    C = 235.4;

    //********************************************
    //        调试阶段使用的值
    //********************************************
    WID_door_vr = 1.200;
    LEN_door_lr = 3.300;
    WID_torp_big_r = 0.305;
    WID_torp_sma_r = 0.178;
    WID_torp_who_r = 1.219;
}

//**************************************
//             NCImgPro
//**************************************

/*===================================================================
 * 函数：InitCameraUndist()
 *
 * 说明：
 *  初始化摄像头消除畸变的信息；
 *
 *  1、标定信息xml文件位置的获取；
 *  2、获取标定信息；
/*===================================================================
 */
void NCImgPro::InitCameraUndist()
{
    string XMLDownCalib_address = "../XML Save Path/Camera_Calibration/UnderWater/Camera28_UnderWater_780_580.xml";
    string XMLFwdCalib_address = "../XML Save Path/Camera_Calibration/UnderWater/Camera29_UnderWater_780_580.xml";

    GetCameraCalibInfoFromXML(XMLDownCalib_address, FrameSize, DownIntrinMat, DownDistortMat);
    GetCameraCalibInfoFromXML(XMLFwdCalib_address, FrameSize, FwdIntrinMat, FwdDistortMat);
}

/*===================================================================
 * 函数：InitHSVRGBInfo()
 *
 * 说明：
 *  初始化各颜色阈值信息；
 *
 *  1、颜色阈值信息xml文件位置的获取；
 *  2、获取颜色阈值信息；
/*===================================================================
 */
void NCImgPro::InitHSVRGBInfo()
{
    // 04.17视频XML文件地址
//    string XML_FilterDoor = "E:/Program/XML Save Path/2016.04/FilterThresh/First_filter_test.xml";
//    string XML_FilterLine = "E:/Program/XML Save Path/2016.04/FilterThresh/Line_filter_test.xml";

//    // 05.20视频XML文件地址
//    string XML_FilterDoor = "E:/Program/XML Save Path/2016.05/FilterThresh/0520_Door.xml";
//    string XML_FilterLine = "E:/Program/XML Save Path/2016.05/FilterThresh/0520_Line.xml";
//    string XML_FilterLineID = "E:/Program/XML Save Path/2016.05/FilterThresh/0520_LineInDoor.xml";

    // 05.20视频XML文件地址
    string XML_FilterDoor = "../XML Save Path/FilterThresh/0520_Door.xml";
    string XML_FilterLine = "../XML Save Path/FilterThresh/0728_Line.xml";
    string XML_FilterLineID = "../XML Save Path/FilterThresh/0727_LineInDoor_D.xml";

    // 05.26视频XML文件地址
    string XML_BallsFar = "../XML Save Path/FilterThresh/0526_BallsFar.xml";
    string XML_RedNear = "../XML Save Path/FilterThresh/0526_RedNear.xml";
    string XML_YelGreNear = "../XML Save Path/FilterThresh/0526_YelGreNear.xml";
    string XML_RedAttach = "../XML Save Path/FilterThresh/0526_RedAttach.xml";
    string XML_YelGreAttach = "../XML Save Path/FilterThresh/0526_YelGreAttach.xml";

//    string XML_StyleDoorFar = "../XML Save Path/FilterThresh/0526_StyleDoorFar.xml";
//    string XML_StyleDoorNear = "../XML Save Path/FilterThresh/0526_StyleDoorNear.xml";

    string XML_StyleDoorFar = "../XML Save Path/FilterThresh/0614_TestStyleDoorFar.xml";
//    string XML_StyleDoorNear = "../XML Save Path/FilterThresh/0614_TestStyleDoorNear.xml";

    string XML_StyleDoorNear = "../XML Save Path/FilterThresh/0727_StyleDoor_D.xml";

    string XML_Red = "../XML Save Path/FilterThresh/0726_RedBall.xml";
    string XML_YelGre = "../XML Save Path/FilterThresh/0709Night_YelGreBalls.xml";

////    string XML_Red = "../XML Save Path/FilterThresh/0617_RedBall.xml";
//    string XML_YelGre = "../XML Save Path/FilterThresh/0617_YelGreBalls.xml";

//    string XML_Red = "../XML Save Path/FilterThresh/0708_RedBall.xml";
    string XML_Yellow = "../XML Save Path/FilterThresh/0726_YellowBall.xml";
    string XML_Green = "../XML Save Path/FilterThresh/0726_GreenBall.xml";


    string XML_TorBoard = "../XML Save Path/FilterThresh/0704_TorBoard.xml";
    string XML_TorRedEdge = "../XML Save Path/FilterThresh/0704_TorRedEdge.xml";

    string XML_AnchorCover = "../XML Save Path/FilterThresh/0731_AnchorCover.xml";
    string XML_AnchorBox = "../XML Save Path/FilterThresh/0731_AnchorBox.xml";
    string XML_AnchorPattern = "../XML Save Path/FilterThresh/0731_AnchorPattern.xml";

    string XML_OctRedCoin = "../XML Save Path/FilterThresh/0728_RedCoins.xml";

    GetJoinMinusInfoFromXML(XML_FilterDoor, DoorHSV1, DoorHSV2,
                            DoorRGB1, DoorRGB2);
    GetJoinMinusInfoFromXML(XML_FilterLine, LineHSV1, LineHSV2,
                            LineRGB1, LineRGB2);
    GetJoinMinusInfoFromXML(XML_FilterLineID, LineIDHSV1, LineIDHSV2,
                            LineIDRGB1, LineIDRGB2);

    GetJoinMinusInfoFromXML(XML_BallsFar, BallsFarHSV1, BallsFarHSV2,
                            BallsFarRGB1, BallsFarRGB2);
    GetJoinMinusInfoFromXML(XML_RedNear, RedNearHSV1, RedNearHSV2,
                            RedNearRGB1, RedNearRGB2);
    GetJoinMinusInfoFromXML(XML_YelGreNear, YelGreNearHSV1, YelGreNearHSV2,
                            YelGreNearRGB1, YelGreNearRGB2);
    GetJoinMinusInfoFromXML(XML_RedAttach, RedAttHSV1, RedAttHSV2,
                            RedAttRGB1, RedAttRGB2);
    GetJoinMinusInfoFromXML(XML_YelGreAttach, YelGreAttHSV1, YelGreAttHSV2,
                            YelGreAttRGB1, YelGreAttRGB2);
    GetJoinMinusInfoFromXML(XML_Red, RedHSV1, RedHSV2,
                            RedRGB1, RedRGB2);
    GetJoinMinusInfoFromXML(XML_YelGre, YelGreHSV1, YelGreHSV2,
                            YelGreRGB1, YelGreRGB2);
    GetJoinMinusInfoFromXML(XML_Yellow, YellowHSV1, YellowHSV2,
                            YellowRGB1, YellowRGB2);
    GetJoinMinusInfoFromXML(XML_Green, GreenHSV1, GreenHSV2,
                            GreenRGB1, GreenRGB2);

    GetJoinMinusInfoFromXML(XML_StyleDoorFar, StyleDoorFarHSV1, StyleDoorFarHSV2,
                            StyleDoorFarRGB1, StyleDoorFarRGB2);
    GetJoinMinusInfoFromXML(XML_StyleDoorNear, StyleDoorNearHSV1, StyleDoorNearHSV2,
                            StyleDoorNearRGB1, StyleDoorNearRGB2);

    GetJoinMinusInfoFromXML(XML_TorBoard, TorBoardHSV1, TorBoardHSV2,
                            TorBoardRGB1, TorBoardRGB2);
    GetJoinMinusInfoFromXML(XML_TorRedEdge, TorRedEdgeHSV1, TorRedEdgeHSV2,
                            TorRedEdgeRGB1, TorRedEdgeRGB2);

    GetJoinMinusInfoFromXML(XML_AnchorCover, AnchorCoverHSV1, AnchorCoverHSV2,
                            AnchorCoverRGB1, AnchorCoverRGB2);
    GetJoinMinusInfoFromXML(XML_AnchorBox, AnchorBoxHSV1, AnchorBoxHSV2,
                            AnchorBoxRGB1, AnchorBoxRGB2);
    GetJoinMinusInfoFromXML(XML_AnchorPattern, AnchorPatternHSV1, AnchorPatternHSV2,
                            AnchorPatternRGB1, AnchorPatternRGB2);

    GetJoinMinusInfoFromXML(XML_OctRedCoin, RedCoinHSV1, RedCoinHSV2,
                            RedCoinRGB1, RedCoinRGB2);
}

//**************************************
//             NCVision
//**************************************

/*===================================================================
 * 函数：InitVisionInfo()
 *
 * 说明：
 *  VInfo初始化；
/*===================================================================
 */
void NCVision::InitVisionInfo()
{
    // 寻线相关
    VInfo.linestate = 0;
    for(int i = 0; i < 4; i++)
        VInfo.GuideLine[i] = -1;
    VInfo.LineAngle = 0;
    VInfo.LineROIArea = Rect(-1, -1, -1, -1);

    // 过门相关
    VInfo.doorstate = 0;
    VInfo.lineIDstate = 0;
    VInfo.LineIDOffsetX = 0;
    VInfo.lineIDFar = 0;
    VInfo.LineIDROI = Rect(-1, -1, -1, -1);

    for(int i = 0; i < 4; i++)
        VInfo.doorline[i] = -1;
    VInfo.DoorRects.clear();
    VInfo.AverageRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
    VInfo.CenterOffsetX = 0;

    // 撞球相关
    VInfo.BallColor = 0;

    VInfo.ballsstate = 0;
    VInfo.RedHave = 0;
    VInfo.YellowHave = 0;
    VInfo.GreenHave = 0;

    VInfo.RedRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
    VInfo.YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
    VInfo.GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);

    VInfo.RedAttached = false;
    VInfo.GreenAttached = false;

    VInfo.RedOffsetX = 0;
    VInfo.RedOffsetY = 0;
    VInfo.YellowOffsetX = 0;
    VInfo.YellowOffsetY = 0;
    VInfo.GreenOffsetX = 0;
    VInfo.GreenOffsetY = 0;

    // 风格过门相关
    VInfo.StyleDoorState = 0;
    VInfo.StyleDoorVecRects.clear();
    VInfo.StyleDoorRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
    VInfo.StyleCenterOffsetX = 0;
    VInfo.StyleCenterOffsetY = 0;
}

/*===================================================================
 * 函数：InitVision()
 *
 * 说明：Vision类初始化
 *  1、28、29号摄像头GUID的设置；
 *  2、初始化28、29号摄像头；
/*===================================================================
 */
void NCVision::InitVision()
{
    DownCamera.setCameraGUID(28);
    FwdCamera.setCameraGUID(29);
    DownCamera.InitCamera(28);
    FwdCamera.InitCamera(29);
}

// （调试，实时采集）Vision类初始化
void NCVision::InitVisionDebug()
{
    DownCamera.setCameraGUID(30);
    FwdCamera.setCameraGUID(0);
    DownCamera.InitCamera(30);
    FwdCamera.InitCamera(0);
}

/*===================================================================
 * 函数：InitCamerasParam()
 *
 * 说明：
 *  前视、俯视摄像头参数初始化；
/*===================================================================
 */
void NCVision::InitCamerasParam()
{
    DownCamera.InitCameraParam();
    FwdCamera.InitCameraParam();
}

// （调试，实时采集）前视、俯视摄像头参数初始化；
void NCVision::InitCamerasParamDebug()
{
    DownCamera.InitCameraParam();
}

/*===================================================================
 * 函数：setGrabFramesVideosPaths()
 *
 * 说明：
 *  调试用函数，设置俯视、前视视频地址；
/*===================================================================
 */
void NCVision::setGrabFramesVideosPaths()
{
//    // 04.17视频文件地址
//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/28/04.02/28_0402_01.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/04.17/0417_video_01.avi");

//    // 05.20视频文件地址
//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/05.20/05.avi");
////    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/05.26/02 Part.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/05.20/01.avi");

//    // 05.26视频文件地址
//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/05.26/02 Part.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/05.26/02 Part.avi");

//    DownCamera.setVideoCapture("F:/Work Station BackUp/F-046C Images and Videos/29/05.20/05.avi");
//    FwdCamera.setVideoCapture("F:/Work Station BackUp/F-046C Images and Videos/29/05.20/04.avi");

    DownCamera.setVideoCapture("C:/Users/GRQ/Desktop/06.24 Vision Class/06.21/03.avi");
    FwdCamera.setVideoCapture("C:/Users/GRQ/Desktop/06.24 Vision Class/06.21/03.avi");

//    DownCamera.setVideoCapture("C:/Users/GRQ/Desktop/07.08/0708_Balls.avi");
//    FwdCamera.setVideoCapture("C:/Users/GRQ/Desktop/07.08/0708_Balls.avi");

////    // 06.21视频文件地址
//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/06.25/02.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/06.25/02.avi");

//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/06.21/03.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/06.21/03.avi");

//    // 06.14视频文件地址
//    DownCamera.setVideoCapture("D:/F-046C Images and Videos/29/Test Video/Style Door Test.avi");
//    FwdCamera.setVideoCapture("D:/F-046C Images and Videos/29/Test Video/Style Door Test.avi");
}
