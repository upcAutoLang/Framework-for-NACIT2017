#include <ncimgpro.h>

/*===================================================================
 * 函数：BinDoor
 *
 * 说明：
 *  寻找门，生成二值化图像；
 *
 * 1、一次并运算，两次减运算，滤出门的二值图；
 * 2、形态学闭运算；
/*===================================================================
 */
void NCImgPro::BinDoor(int num)
{
    switch(num)
    {
    case 4:
    {
        // 中值滤波
        medianBlur(FwdImg, FwdImg, 3);
    //    imshow("Median Blur Door", FwdImg);
        // 一次并运算，两次减运算，滤出门的二值图；
        BinMatDoubleJoinMinus(FwdImg, imgBinDoor, DoorHSV1, DoorHSV2, DoorRGB1, DoorRGB2);

        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        Mat element2 = getStructuringElement(MORPH_RECT, Size(5, 5));
    //        dilate(imgResult, imgResult, element);
        morphologyEx(imgBinDoor, imgBinDoor, MORPH_OPEN, element);
        dilate(imgBinDoor, imgBinDoor, element2);
        break;
    }
    case 5:
    {
        // 中值滤波
        medianBlur(FwdImg, FwdImg, 3);
    //    imshow("Median Blur Door", FwdImg);
        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        Mat element2 = getStructuringElement(MORPH_RECT, Size(5, 5));
//        dilate(imgResult, imgResult, element);

        //**************************
        //          滤门
        //**************************

        // 一次并运算，两次减运算，滤出门的二值图；
        BinMatDoubleJoinMinus(FwdImg, imgBinDoor, DoorHSV1, DoorHSV2, DoorRGB1, DoorRGB2);

        morphologyEx(imgBinDoor, imgBinDoor, MORPH_OPEN, element);
        dilate(imgBinDoor, imgBinDoor, element2);

        //**************************
        //          滤线
        //**************************

        BinMatDoubleJoinMinus(FwdImg, imgBinLineID, LineIDHSV1, LineIDHSV2,
                              LineIDRGB1, LineIDRGB2);
        morphologyEx(imgBinLineID, imgBinLineID, MORPH_OPEN, element);

//        imshow("Line In Door", imgBinLineID);

//        imshow("Door Pre Pro", FwdImg);
//        imshow("Door Binary", imgBinDoor);
        break;
    }
    default:
        cout<<"Wrong Binary Door Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：BinLine
 *
 * 说明：
 *  寻找导引路径，生成二值化图像；
 *
 * 1、一次并运算，两次减运算，滤出门的二值图；
 * 2、形态学闭运算；
/*===================================================================
 */
void NCImgPro::BinLine(int num)
{
    switch(num)
    {
    case 4:
    {
        // 中值滤波
        medianBlur(DownImg, DownImg, 3);
    //    imshow("Median Blur Line", DownImg);
        // 暂时性，为了调试程序使用
        HSVThresh(DownImg, imgBinLine, LineHSV1);
        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    //        dilate(imgResult, imgResult, element);
        morphologyEx(imgBinLine, imgBinLine, MORPH_CLOSE, element);
        erode(imgBinLine, imgBinLine, element);

//        imshow("Line Pre Pro", DownImg);
//        imshow("Line Binary", imgBinLine);
        break;
    }
    case 5:
    {
        break;
    }
    default:
        cout<<"Wrong Binary Door Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：BinBalls
 *
 * 说明：
 *  寻找三个球，生成二值化图像；
/*===================================================================
 */
void NCImgPro::BinBalls(int state, int num)
{
    switch(num)
    {
    case 5:
    {
        // state在没有初值的情况下的初始化；
        if(state != BALL_FAR && state != BALL_NEAR && state != BALL_ATTACH
                && state != BALL_ATTACHED_RED && state != BALL_ATTACHED_YELLOW && state != BALL_ATTACHED_GREEN)
            state = BALL_FAR;
        // 不同的球距离状态
        switch(state)
        {
        case BALL_FAR:
        {
            BinMatDoubleJoinMinus(FwdImg, imgBinBallsFar,
                                  BallsFarHSV1, BallsFarHSV2,
                                  BallsFarRGB1, BallsFarRGB2);
            // 定义核
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
            morphologyEx(imgBinBallsFar, imgBinBallsFar, MORPH_OPEN, element);
            dilate(imgBinBallsFar, imgBinBallsFar, element);
//            imshow("Far Balls", imgBinBallsFar);
            break;
        }
        case BALL_NEAR:
        {
            BinMatDoubleJoinMinus(FwdImg, imgBinRed,
                                  RedNearHSV1, RedNearHSV2,
                                  RedNearRGB1, RedNearRGB2);
            BinMatDoubleJoinMinus(FwdImg, imgBinYelGre,
                                  YelGreNearHSV1, YelGreNearHSV2,
                                  YelGreNearRGB1, YelGreNearRGB2);
            // 定义核
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
            morphologyEx(imgBinRed, imgBinRed, MORPH_OPEN, element);
            dilate(imgBinRed, imgBinRed, element);
            morphologyEx(imgBinYelGre, imgBinYelGre, MORPH_OPEN, element);
            dilate(imgBinYelGre, imgBinYelGre, element);

//            imshow("Red Ball", imgBinRed);
//            imshow("Yellow & Green Balls", imgBinYelGre);
            break;
        }
        case BALL_ATTACH:
        case BALL_ATTACHED_RED:
        case BALL_ATTACHED_GREEN:
        {
            BinMatDoubleJoinMinus(FwdImg, imgBinRed,
                                  RedAttHSV1, RedAttHSV2,
                                  RedAttRGB1, RedAttRGB2);
            BinMatDoubleJoinMinus(FwdImg, imgBinYelGre,
                                  YelGreAttHSV1, YelGreAttHSV2,
                                  YelGreAttRGB1, YelGreAttRGB2);
            // 定义核
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
            morphologyEx(imgBinRed, imgBinRed, MORPH_OPEN, element);
            morphologyEx(imgBinYelGre, imgBinYelGre, MORPH_OPEN, element);

//            imshow("Red Ball", imgBinRed);
//            imshow("Yellow & Green Balls", imgBinYelGre);

            break;
        }
        default:
            cout<<"Wrong Bin Balls State!"<<endl;
            break;
        }

        break;
    }
    case 6:
    {
        BinMatDoubleJoinMinus(FwdImg, imgBinRed,
                              RedHSV1, RedHSV2,
                              RedRGB1, RedRGB2);
        BinMatDoubleJoinMinus(FwdImg, imgBinYelGre,
                              YelGreHSV1, YelGreHSV2,
                              YelGreRGB1, YelGreRGB2);
        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(imgBinRed, imgBinRed, MORPH_OPEN, element);
        dilate(imgBinRed, imgBinRed, element);
        morphologyEx(imgBinYelGre, imgBinYelGre, MORPH_OPEN, element);
        dilate(imgBinYelGre, imgBinYelGre, element);

//        imshow("Red Ball", imgBinRed);
//        imshow("Yellow & Green Balls", imgBinYelGre);

        break;
    }
    case 7:
    {
        BinMatDoubleJoinMinus(FwdImg, imgBinRed,
                              RedHSV1, RedHSV2,
                              RedRGB1, RedRGB2);
        BinMatDoubleJoinMinus(FwdImg, imgBinYellow,
                              YellowHSV1, YellowHSV2,
                              YellowRGB1, YellowRGB2);
        BinMatDoubleJoinMinus(FwdImg, imgBinGreen,
                              GreenHSV1, GreenHSV2,
                              GreenRGB1, GreenRGB2);

        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(imgBinRed, imgBinRed, MORPH_OPEN, element);
        dilate(imgBinRed, imgBinRed, element);
        morphologyEx(imgBinYellow, imgBinYellow, MORPH_OPEN, element);
        dilate(imgBinYellow, imgBinYellow, element);
        morphologyEx(imgBinGreen, imgBinGreen, MORPH_OPEN, element);
        dilate(imgBinGreen, imgBinGreen, element);

//        imshow("Red Ball", imgBinRed);
//        imshow("Yellow Ball", imgBinYellow);
//        imshow("Green Ball", imgBinGreen);

        break;
    }
    default:
        cout<<"Wrong Binary Balls Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：BinStyleDoor
 *
 * 说明：
 *  寻找风格过门，生成二值化图像；
 *
 * 参数：
 *  int state:      与风格过门的距离状态；
 *  int num:        二值化图像处理代码编写月份；
/*===================================================================
 */
void NCImgPro::BinStyleDoor(int state, int num)
{
    switch(num)
    {
    case 6:
    {
        switch(state)
        {
        case STYLE_FAR:
        {
            BinMatDoubleJoinMinus(FwdImg, imgBinStyleDoor,
                                  StyleDoorFarHSV1, StyleDoorFarHSV2,
                                  StyleDoorFarRGB1, StyleDoorFarRGB2);
            // 定义核
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//            morphologyEx(imgBinStyleDoor, imgBinStyleDoor, MORPH_OPEN, element);
            dilate(imgBinStyleDoor, imgBinStyleDoor, element);
//            imshow("Style Door", imgBinStyleDoor);
            break;
        }
        case STYLE_NEAR:
        {
            BinMatDoubleJoinMinus(FwdImg, imgBinStyleDoor,
                                  StyleDoorNearHSV1, StyleDoorNearHSV2,
                                  StyleDoorNearRGB1, StyleDoorNearRGB2);
            // 定义核
            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
            morphologyEx(imgBinStyleDoor, imgBinStyleDoor, MORPH_OPEN, element);
            dilate(imgBinStyleDoor, imgBinStyleDoor, element);
//            imshow("Style Door", imgBinStyleDoor);
            break;
        }
        default:
            cout<<"Wrong Bin Style Door State!"<<endl;
            break;
        }
        break;
    }
    case 7:
    {
        BinMatDoubleJoinMinus(FwdImg, imgBinStyleDoor,
                              StyleDoorNearHSV1, StyleDoorNearHSV2,
                              StyleDoorNearRGB1, StyleDoorNearRGB2);
        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//        morphologyEx(imgBinStyleDoor, imgBinStyleDoor, MORPH_OPEN, element);
        dilate(imgBinStyleDoor, imgBinStyleDoor, element);
//        imshow("Style Door Bin", imgBinStyleDoor);

        break;
    }
    default:
        cout<<"Wrong Binary Style Door Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：BinTorBoard
 *
 * 说明：
 *  寻找鱼雷切板，生成二值化图像；
 *
 * 参数：
 *  int num:        二值化图像处理代码编写月份；
/*===================================================================
 */
void NCImgPro::BinTorBoard(int num)
{
    switch(num)
    {
    case 7:
    {
        BinMatDoubleJoinMinus(FwdImg, imgBinTorBoard,
                              TorBoardHSV1, TorBoardHSV2,
                              TorBoardRGB1, TorBoardRGB2);
        BinMatDoubleJoinMinus(FwdImg, imgBinTorRedEdge,
                              TorRedEdgeHSV1, TorRedEdgeHSV2,
                              TorRedEdgeRGB1, TorRedEdgeRGB2);
        // 定义核
        Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
//        morphologyEx(imgBinStyleDoor, imgBinStyleDoor, MORPH_OPEN, element);
//        dilate(imgBinTorBoard, imgBinTorBoard, element);
        morphologyEx(imgBinTorBoard, imgBinTorBoard, MORPH_OPEN, element);
        morphologyEx(imgBinTorRedEdge, imgBinTorRedEdge, MORPH_OPEN, element);
//        dilate(imgBinTorRedEdge, imgBinTorRedEdge, element);

//        imshow("Torpedo Board", imgBinTorBoard);
//        imshow("Red Edge", imgBinTorRedEdge);
        break;
    }
    default:
        cout<<"Wrong Binary Torpedo Board Number!"<<endl;
        break;
    }
}

// 寻找标志物箱子，生成二值化图像；
void NCImgPro::BinAnchors(int num)
{
    switch(num)
    {
    case 7:
    {
        BinMatDoubleJoinMinus(DownImg, imgBinAnchorCover,
                              AnchorCoverHSV1, AnchorCoverHSV2,
                              AnchorCoverRGB1, AnchorCoverRGB2);
        BinMatDoubleJoinMinus(DownImg, imgBinAnchorBox,
                              AnchorBoxHSV1, AnchorBoxHSV2,
                              AnchorBoxRGB1, AnchorBoxRGB2);
        BinMatDoubleJoinMinus(DownImg, imgBinAnchorPattern,
                              AnchorPatternHSV1, AnchorPatternHSV2,
                              AnchorPatternRGB1, AnchorPatternRGB2);

        // 定义核
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(imgBinAnchorCover, imgBinAnchorCover, MORPH_OPEN, element);
        morphologyEx(imgBinAnchorBox, imgBinAnchorBox, MORPH_OPEN, element);
        morphologyEx(imgBinAnchorPattern, imgBinAnchorPattern, MORPH_OPEN, element);

//        imshow("Bin Anchor Cover", imgBinAnchorCover);
//        imshow("Bin Anchor Box", imgBinAnchorBox);
//        imshow("Bin Anchor Pattern", imgBinAnchorPattern);
        break;
    }
    default:
        cout<<"Wrong Binary Weigh Anchor Number!"<<endl;
        break;
    }
}

void NCImgPro::BinOctCoins()
{
    BinMatDoubleJoinMinus(DownImg, imgBinRedCoins,
                          RedCoinHSV1, RedCoinHSV2,
                          RedCoinRGB1, RedCoinRGB2);

    // 定义核
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(imgBinRedCoins, imgBinRedCoins, MORPH_OPEN, element);
}
