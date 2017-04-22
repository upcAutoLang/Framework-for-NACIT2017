#include <ncvision.h>


// 显示过门图像
void NCVision::DrawFirstDoor(int num)
{
    switch(num)
    {
    case 5:
    {
        Mat tmp;
        ImgPro.getFwdFramePrePro(tmp);

        //***********************************
        //         图片中输出信息
        //***********************************

        string str;
        Size StrSize;
        Point outPoint;
        int baseline;
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int thickness = 1;
        Scalar color = Scalar(255, 255, 255);


        // 输出Line ID State
        switch(VInfo.lineIDstate)
        {
        case DOOR_FOUNDLINE:
        {
            str = "Line In Door state: FOUNDLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case DOOR_NOLINE:
        {
            str = "Line In Door state: NOLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        default:
            break;
        }

        // 输出Line ID Far
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            switch(VInfo.lineIDFar)
            {
            case DOOR_FAR:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Line In Door Far: FAR";
    //            Size getTextSize(const string& text, int fontFace, double fontScale, int thickness, int* baseLine)
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_NOLINE:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Line In Door Far: NEAR";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            default:
                break;
            }
        }

        // 输出Door State
        if(VInfo.lineIDstate == DOOR_FOUNDLINE
                && VInfo.lineIDFar == DOOR_NEAR)
        {
            switch(VInfo.doorstate)
            {
            case DOOR_NODOOR:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: NODOOR";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_HALFLEFT:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: LEFT";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_HALFRIGHT:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: RIGHT";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_WHOLEDOOR:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: WHOLE";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_TOPONLY:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: TOP";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_OTHER:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Door State: OTHER";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            default:
                break;
            }
        }

        //***********************************
        //         轮廓绘制
        //***********************************

        // 导引路径轮廓绘制
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            Rect ROI;
            ROI = VInfo.LineIDROI;

            // 绘制前视摄像头中看到的导引路径，黄线绘制
            PaintRect(tmp, tmp, ROI, Scalar(0, 255, 255), 2);
        }

        // 门轮廓绘制
        if(VInfo.lineIDstate == DOOR_FOUNDLINE
                && VInfo.lineIDFar == DOOR_NEAR)
        {
            vector<RotatedRect> rects;
            RotatedRect DoorRect;
            rects = VInfo.DoorRects;
            DoorRect = VInfo.AverageRect;

            int size = rects.size();

            // 绘制外接轮廓，红色绘制
            PaintRotatedRect(tmp, tmp, DoorRect, Scalar(0, 0, 255), 2);

            // 绘制每个小轮廓，绿色绘制
            for(int i = 0; i < size; i++)
            {
                PaintRotatedRect(tmp, tmp, rects[i], Scalar(0, 255, 0), 2);
            }
        }

        imshow("First Door", tmp);
        break;
    }
    case 6:
    {
        Mat tmp;
        ImgPro.getFwdFramePrePro(tmp);

        //***********************************
        //         图片中输出信息
        //***********************************

        string str;
        Size StrSize;
        Point outPoint;
        int baseline;
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int thickness = 1;
        Scalar color = Scalar(255, 255, 255);

        // 输出Line ID State
        switch(VInfo.lineIDstate)
        {
        case DOOR_FOUNDLINE:
        {
            str = "Line In Door state: FOUNDLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case DOOR_NOLINE:
        {
            str = "Line In Door state: NOLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        default:
            break;
        }

        // 输出Line ID Far
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            switch(VInfo.lineIDFar)
            {
            case DOOR_FAR:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Line In Door Far: FAR";
    //            Size getTextSize(const string& text, int fontFace, double fontScale, int thickness, int* baseLine)
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            case DOOR_NOLINE:
            {
                StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
                baseline += 1;

                str = "Line In Door Far: NEAR";
                outPoint = outPoint + Point(0, StrSize.height + 2);
                putText(tmp, str, outPoint, fontFace,
                        fontScale, color, thickness);
                break;
            }
            default:
                break;
            }
        }

        // 输出Line ID OffsetX
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Line ID OffsetX: ";
            str += double2String(VInfo.LineIDOffsetX);

            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
        }

        //***********************************
        //         轮廓绘制
        //***********************************

        // 导引路径轮廓绘制
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            Rect ROI;
            ROI = VInfo.LineIDROI;

            // 绘制前视摄像头中看到的导引路径，黄线绘制
            PaintRect(tmp, tmp, ROI, Scalar(0, 255, 255), 2);
        }

        imshow("First Door", tmp);

        break;
    }
    default:
        cout<<"Draw First Door Error: Wrong Num!"<<endl;
        break;
    }

}

// 显示寻线图像
void NCVision::DrawGuideLine(int num)
{
    switch(num)
    {
    case 5:
    {
        Mat tmp;
        ImgPro.getDownFramePrePro(tmp);

        //***********************************
        //         图片中输出信息
        //***********************************

        string str;
        Size StrSize;
        Point outPoint;
        int baseline;
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int thickness = 1;
        Scalar fontcolor = Scalar(255, 255, 255);
        Scalar linecolor = Scalar(255, 255, 255);

        // 输出Line State
        switch(VInfo.linestate)
        {
        case GUIDE_NOLINE:
        {
            str = "Line State: NOLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        case GUIDE_POINT:
        {
            str = "Line State: POINT";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        case GUIDE_LINE:
        {
            str = "Line State: LINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        default:
            break;
        }

        // 输出Line Angle
        if(VInfo.linestate != GUIDE_NOLINE)
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Line Angle: ";
            str += double2String(VInfo.LineAngle);
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
        }

        //***********************************
        //         轮廓绘制
        //***********************************

        // 导引路径ROI绘制
        if(VInfo.linestate != GUIDE_NOLINE)
        {
            Rect ROI;
            float line[4];
            for(int i = 0; i < 4; i++)
                line[i] = VInfo.GuideLine[i];
            ROI = VInfo.LineROIArea;

            // 绘制导引路径ROI，蓝线绘制
            PaintRect(tmp, tmp, ROI, Scalar(255, 0, 0), 2);

            // 点状态，用红线绘制
            if(VInfo.linestate == GUIDE_POINT)
                linecolor = Scalar(0, 0, 255);
            // 线状态，用绿线绘制
            else if (VInfo.linestate == GUIDE_LINE)
                linecolor = Scalar(0, 255, 0);

            PaintFitLine(tmp, tmp, line,
                         linecolor, 2);
        }

        imshow("Find Line", tmp);
        break;
    }
    case 6:
    {
        Mat tmp;
        ImgPro.getDownFramePrePro(tmp);

        //***********************************
        //         图片中输出信息
        //***********************************

        string str;
        Size StrSize;
        Point outPoint;
        int baseline;
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        int thickness = 1;
        Scalar fontcolor = Scalar(255, 255, 255);
        Scalar linecolor = Scalar(255, 255, 255);

        //***********************************
        //         输出Line State
        //***********************************
        switch(VInfo.linestate)
        {
        case GUIDE_NOLINE:
        {
            str = "Line State: NOLINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        case GUIDE_POINT:
        {
            str = "Line State: POINT";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        case GUIDE_LINE:
        {
            str = "Line State: LINE";
            outPoint = Point(10, 20);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
            break;
        }
        default:
            cout<<"Draw Line Info Error: Line State Num Wrong!"<<endl;
            break;
        }

        //***********************************
        //        输出其余线的信息
        //***********************************
        if(VInfo.linestate != GUIDE_NOLINE)
        {
            //***********************************
            //        输出Line Angle
            //***********************************

            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Line Angle: ";
            str += double2String(VInfo.LineAngle);
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);

            //***********************************
            //        输出Line Offset
            //***********************************

            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Line Offset: (";
            str += double2String(VInfo.LineOffsetX);
            str += ", ";
            str += double2String(VInfo.LineOffsetY);
            str += ")";

            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);

            //***********************************
            //        输出IsAboveLine
            //***********************************

            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Above Line: ";

            if(VInfo.IsAboveLine)
                str += "YES";
            else
                str += "NO";

            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, fontcolor, thickness);
        }

        //***********************************
        //         轮廓绘制
        //***********************************

        // 导引路径ROI绘制
        if(VInfo.linestate != GUIDE_NOLINE)
        {
            Rect ROI;
            float line[4];
            for(int i = 0; i < 4; i++)
                line[i] = VInfo.GuideLine[i];
            ROI = VInfo.LineROIArea;

            // 绘制导引路径ROI，蓝线绘制
            PaintRect(tmp, tmp, ROI, Scalar(255, 0, 0), 2);

            // 点状态，用红线绘制
            if(VInfo.linestate == GUIDE_POINT)
                linecolor = Scalar(0, 0, 255);
            // 线状态，用绿线绘制
            else if (VInfo.linestate == GUIDE_LINE)
                linecolor = Scalar(0, 255, 0);

            PaintFitLine(tmp, tmp, line,
                         linecolor, 2);
        }

        imshow("Find Line", tmp);
        break;
    }
    default:
        cout<<"Draw Guide Line Error: Wrong Num!"<<endl;
        break;
    }
}

// 显示撞球图像
void NCVision::DrawBalls()
{
    Mat tmp;
    ImgPro.getFwdFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    Scalar color = Scalar(255, 255, 255);

    // 寻找的球的颜色
    switch(VInfo.BallColor)
    {
    case BALL_RED:
    {
        str = "Ball's Color: RED";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case BALL_YELLOW:
    {
        str = "Ball's Color: YELLOW";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case BALL_GREEN:
    {
        str = "Ball's Color: GREEN";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    default:
        break;
    }

    // 输出Attached Balls
    if(VInfo.RedAttached || VInfo.GreenAttached)
    {
        if(VInfo.RedAttached && VInfo.GreenAttached)
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Attached Balls: RED & GREEN";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
        }
        else if(VInfo.RedAttached && !VInfo.GreenAttached)
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Attached Balls: RED";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
        }
        else if(!VInfo.RedAttached && VInfo.GreenAttached)
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;

            str = "Attached Balls: GREEN";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
        }
    }

    // 输出ball state
    switch(VInfo.ballsstate)
    {
    case BALL_FAR:
    {
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        str = "Ball's State: FAR";
        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case BALL_NEAR:
    {
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        str = "Ball's State: NEAR";
        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case BALL_ATTACH:
    {
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        str = "Ball's State: ATTACH";
        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    default:
        break;
    }

    //***********************************
    //         轮廓绘制
    //***********************************

//    if(VInfo.ballsstate != BALL_FAR)
//    {
        if(VInfo.RedHave == BALL_HAVE)
        {
            RotatedRect rect;
            rect = VInfo.RedRect;
            // 红色绘制红球区域
            color = Scalar(0, 0, 255);
            PaintRotatedRect(tmp, tmp, rect,
                             color, thickness);
        }

        if(VInfo.YellowHave == BALL_HAVE)
        {
            RotatedRect rect;
            rect = VInfo.YellowRect;
            // 黄色绘制黄球区域
            color = Scalar(0, 255, 255);
            PaintRotatedRect(tmp, tmp, rect,
                             color, thickness);
        }

        if(VInfo.GreenHave == BALL_HAVE)
        {
            RotatedRect rect;
            rect = VInfo.GreenRect;
            // 绿色绘制绿球区域
            color = Scalar(0, 255, 0);
            PaintRotatedRect(tmp, tmp, rect,
                             color, thickness);
        }
//    }

    imshow("Balls", tmp);
}

// 显示风格过门图像(num = 2)
void NCVision::DrawStyleDoor()
{
    Mat tmp;
    ImgPro.getFwdFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    Scalar color = Scalar(255, 255, 255);

    // 输出风格门距离状态StyleDoorState
    switch(VInfo.StyleDoorState)
    {
    case STYLE_FAR:
    {
        str = "Style Door Dist State: FAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case STYLE_NEAR:
    {
        str = "Style Door Dist State: NEAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    default:
        break;
    }

    //***********************************
    //         轮廓绘制
    //***********************************

    // 风格过门轮廓绘制
    if(VInfo.StyleDoorState != STYLE_FAR)
    {
        vector<RotatedRect> rects;
        RotatedRect DoorRect;
        rects = VInfo.StyleDoorVecRects;
        DoorRect = VInfo.StyleDoorRect;

        int size = rects.size();

        // 绘制外接轮廓，红色绘制
        color = Scalar(0, 0, 255);
        PaintRotatedRect(tmp, tmp, DoorRect, color, thickness);

        // 绘制每个小轮廓，黄色绘制
        for(int i = 0; i < size; i++)
        {
            color = Scalar(0, 255, 255);
            PaintRotatedRect(tmp, tmp, rects[i], Scalar(0, 255, 0), 2);
        }
    }

    imshow("Style Door", tmp);
}

// 显示风格过门图像(num = 3)
void NCVision::DrawStyleSeenDoor()
{
    Mat tmp;
    ImgPro.getFwdFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int ExThickness = 2;
    Scalar color = Scalar(255, 255, 255);

    // 输出风格门距离状态StyleDoor State
    switch(VInfo.StyleDoorState)
    {
    case STYLE_FAR:
    {
        str = "Style Distance State: FAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case STYLE_NEAR:
    {
        str = "Style Distance State: NEAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);


        // 在Near状态下，输出风格门状态StyleDoorSeenState
        switch(VInfo.StyleDoorSeenState)
        {
        case STYLE_NODOOR:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: NODOOR";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case STYLE_HALFLEFT:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: LEFT";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case STYLE_HALFRIGHT:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: RIGHT";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case STYLE_WHOLEDOOR:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: WHOLEDOOR";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);

            //***********************************
            //   输出Style Door角度对准状态
            //***********************************

            str = "Style Door Angle State: ";
            switch(VInfo.StyleDoorAngleState)
            {
            case STYLE_CENTER:          {str += "CENTER";break;}
            case STYLE_LEFT:            {str += "LEFT";break;}
            case STYLE_RIGHT:           {str += "RIGHT";break;}
            case STYLE_UNKNOWNANGLE:    {str += "UNKNOWN";break;}
            default:                    {str += "HUH??";break;}
            }
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);

            break;
        }
        case STYLE_BOTTOMONLY:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: BOTTOM";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        case STYLE_OTHER:
        {
            StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
            baseline += 1;
            str = "Style Door State: OTHER";
            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
            break;
        }
        default:
            break;
        }

        break;
    }
    default:
        break;
    }

    //***********************************
    //         轮廓绘制
    //***********************************

    // 风格过门轮廓绘制，绘制每个小轮廓
    if(!(VInfo.StyleDoorSeenState == STYLE_NODOOR && VInfo.StyleDoorSeenState == STYLE_OTHER))
    {
        vector<RotatedRect> rects;
        RotatedRect DoorRect;

        rects = VInfo.StyleDoorVecRects;
        DoorRect = VInfo.StyleDoorRect;

        int size = rects.size();

        // 绘制每个小轮廓，黄色绘制
        for(int i = 0; i < size; i++)
        {
            color = Scalar(0, 255, 255);
            PaintRotatedRect(tmp, tmp, rects[i], Scalar(0, 255, 0), thickness);
        }

        if(VInfo.StyleDoorSeenState == STYLE_WHOLEDOOR)
        {
            // 绘制外接轮廓，红色绘制
            color = Scalar(0, 0, 255);
            PaintRotatedRect(tmp, tmp, DoorRect, color, ExThickness);
        }
    }

    imshow("Style Door", tmp);
}

// 显示鱼雷切板任务图像
void NCVision::DrawTorBoard()
{
    Mat tmp;
    ImgPro.getFwdFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int ExThickness = 2;
    Scalar color = Scalar(255, 255, 255);
    Scalar RedColor = Scalar(0, 0, 255);
    Scalar YellowColor = Scalar(0, 255, 255);
    Scalar GreenColor = Scalar(0, 255, 0);
    Scalar WhiteColor = Scalar(255, 255, 255, 0);

    // 输出鱼雷切板距离等级与状态
    switch(VInfo.TorBoardState)
    {
    case TORPEDO_NOBOARD:
    {
        str = "Torpedo State: NO BOARD";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_FAR:
    {
        str = "Torpedo State: FAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_NEAR:
    {
        str = "Torpedo State: NEAR";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_NOHOLE:
    {
        str = "Torpedo State: ATTACH - NO HOLE";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_ONE:
    {
        str = "Torpedo State: ATTACH - ONE HOLE";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_CROSS:
    {
        str = "Torpedo State: ATTACH - CROSS";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_LEFT:
    {
        str = "Torpedo State: ATTACH - LEFT";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_RIGHT:
    {
        str = "Torpedo State: ATTACH - RIGHT";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    case TORPEDO_ATTACH_TOP:
    {
        str = "Torpedo State: ATTACH - TOP";
        outPoint = Point(10, 20);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
        break;
    }
    default:
        break;
    }

    //***********************************
    //    依据不同状态输出相关信息
    //***********************************

    switch(VInfo.TorBoardState)
    {
    case TORPEDO_NOBOARD:
    {
        break;
    }
    case TORPEDO_FAR:
    {
        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        break;
    }
    case TORPEDO_NEAR:
    {
        outPoint = Point(10, 20);
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        str = "Is Facing: ";
        if(VInfo.IsFacing)
            str += "TRUE";
        else
            str += "FALSE";

        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);

        // 正对鱼雷切板时，输出识别到的孔洞信息
        if(VInfo.IsFacing)
        {
            str = "Found Holes: ";
            if(VInfo.ULHole.HoleHave)
                str += "UpLeft ";
            if(VInfo.URHole.HoleHave)
                str += "UpRight ";
            if(VInfo.DLHole.HoleHave)
                str += "DownLeft ";
            if(VInfo.DRHole.HoleHave)
                str += "DownRight";

            outPoint = outPoint + Point(0, StrSize.height + 2);
            putText(tmp, str, outPoint, fontFace,
                    fontScale, color, thickness);
        }

        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        // 正对鱼雷切板时，绘制轮廓信息
        if(VInfo.IsFacing)
        {

            if(VInfo.ULHole.HoleHave)
            {
                PaintRotatedRect(tmp, tmp, VInfo.ULHole.HoleRect,
                                 RedColor, 1);
                circle(tmp, VInfo.ULHole.Center, 2, RedColor, 3);
            }
            if(VInfo.URHole.HoleHave)
            {
                PaintRotatedRect(tmp, tmp, VInfo.URHole.HoleRect,
                                 RedColor, 1);
                circle(tmp, VInfo.URHole.Center, 2, RedColor, 3);
            }

            if(VInfo.DLHole.HoleHave)
            {
                PaintRotatedRect(tmp, tmp, VInfo.DLHole.HoleRect,
                                 RedColor, 1);
                circle(tmp, VInfo.DLHole.Center, 2, RedColor, 3);
            }
            if(VInfo.DRHole.HoleHave)
            {
                PaintRotatedRect(tmp, tmp, VInfo.DRHole.HoleRect,
                                 RedColor, 1);
                circle(tmp, VInfo.DRHole.Center, 2, RedColor, 3);
            }
        }

        break;
    }
    case TORPEDO_ATTACH_NOHOLE:
    {
        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        break;
    }
    case TORPEDO_ATTACH_ONE:
    {
        outPoint = Point(10, 20);
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        str = "Is Facing: ";
        if(VInfo.IsFacing)
            str += "TRUE";
        else
            str += "FALSE";

        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);

        // 输出当前孔洞的目标状态
        str = "Current Hole Target State: ";
        switch(VInfo.CurrentHole.TargetState)
        {
        case TARGET_NOTARGET:
        {
            str += "NO TARGET";
            break;
        }
        case TARGET_LEFT:
        {
            str += "LEFT";
            break;
        }
        case TARGET_RIGHT:
        {
            str += "RIGHT";
            break;
        }
        case TARGET_CENTER:
        {
            str += "CENTER";
            break;
        }
        default:
            str += "huh??";
            break;
        }
        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);

        // 绘制三个目标范围
        PaintRect(tmp, tmp, VInfo.CurrentHole.LeftTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.RightTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.CenterTarget.rect,
                  GreenColor, 2);


        // 绘制轮廓信息

        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        if(VInfo.CurrentHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.CurrentHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.CurrentHole.Center, 2, RedColor, 3);
        }

        // 绘制目标孔洞信息
        PaintRotatedRect(tmp, tmp, VInfo.CurrentHole.HoleRect,
                         WhiteColor, 2);
        circle(tmp, VInfo.CurrentHole.Center, 2, WhiteColor, 3);

        str = "AIM";
        putText(tmp, str, VInfo.CurrentHole.Center, fontFace,
                fontScale, color, thickness);

        break;
    }
    case TORPEDO_ATTACH_CROSS:
    {
        outPoint = Point(10, 20);
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        // 绘制三个目标范围
        PaintRect(tmp, tmp, VInfo.CurrentHole.LeftTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.RightTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.CenterTarget.rect,
                  GreenColor, 2);

        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        // 绘制孔洞轮廓信息
        if(VInfo.ULHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.ULHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.ULHole.Center, 2, RedColor, 3);
        }
        if(VInfo.URHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.URHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.URHole.Center, 2, RedColor, 3);
        }

        if(VInfo.DLHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.DLHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.DLHole.Center, 2, RedColor, 3);
        }
        if(VInfo.DRHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.DRHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.DRHole.Center, 2, RedColor, 3);
        }

        break;
    }
    case TORPEDO_ATTACH_LEFT:
    case TORPEDO_ATTACH_RIGHT:
    case TORPEDO_ATTACH_TOP:
    {
        outPoint = Point(10, 20);
        StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
        baseline += 1;

        // 绘制三个目标范围
        PaintRect(tmp, tmp, VInfo.CurrentHole.LeftTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.RightTarget.rect,
                  GreenColor, 2);
        PaintRect(tmp, tmp, VInfo.CurrentHole.CenterTarget.rect,
                  GreenColor, 2);

        // 鱼雷切板外界矩形绘制
        RotatedRect TorBoardRect;
        TorBoardRect = VInfo.TorBoardRect;
        PaintRotatedRect(tmp, tmp, TorBoardRect, YellowColor, 2);

        // 绘制孔洞轮廓信息
        if(VInfo.ULHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.ULHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.ULHole.Center, 2, RedColor, 3);
        }
        if(VInfo.URHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.URHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.URHole.Center, 2, RedColor, 3);
        }

        if(VInfo.DLHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.DLHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.DLHole.Center, 2, RedColor, 3);
        }
        if(VInfo.DRHole.HoleHave)
        {
            PaintRotatedRect(tmp, tmp, VInfo.DRHole.HoleRect,
                             RedColor, 1);
            circle(tmp, VInfo.DRHole.Center, 2, RedColor, 3);
        }

        // 绘制目标孔洞信息
        PaintRotatedRect(tmp, tmp, VInfo.CurrentHole.HoleRect,
                         WhiteColor, 2);
        circle(tmp, VInfo.CurrentHole.Center, 2, WhiteColor, 3);

        str = "AIM";
        putText(tmp, str, VInfo.CurrentHole.Center, fontFace,
                fontScale, color, thickness);

        break;
    }
//    case TORPEDO_ATTACH_RIGHT:
//    {
//        break;
//    }
//    case TORPEDO_ATTACH_TOP:
//    {
//        break;
//    }
    default:
        break;
    }


    imshow("Torpedo Board", tmp);
}

// 显示投放标志物任务图像
void NCVision::DrawAnchor()
{
    Mat tmp;
    ImgPro.getDownFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int ExThickness = 2;
    Scalar color = Scalar(255, 255, 255);
    Scalar RedColor = Scalar(0, 0, 255);
    Scalar YellowColor = Scalar(0, 255, 255);
    Scalar GreenColor = Scalar(0, 255, 0);
    Scalar WhiteColor = Scalar(255, 255, 255, 0);
    Scalar BlueColor = Scalar(255, 255, 0);

    //***********************************
    //    输出AnchorDistState
    //***********************************

    switch(VInfo.AnchorDistState)
    {
    case ANCHOR_FAR:
    {
        str = "Anchor Distance State: FAR";
        break;
    }
    case ANCHOR_NEAR:
    {
        str = "Anchor Distance State: NEAR";
        break;
    }
    default:
        str = "Anchor Distance State: HUH??";
        break;
    }
    outPoint = Point(10, 20);
    StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
    baseline += 1;
    putText(tmp, str, outPoint, fontFace,
            fontScale, color, thickness);

    //***********************************
    //    输出AnchorState
    //***********************************

    str = "Anchor State: ";
    switch(VInfo.AnchorState)
    {
    case ANCHOR_NOANCHOR:
    {
        str += "NO ANCHOR";
        break;
    }
    case ANCHOR_COVERONLY:
    {
        str += "COVER";
        break;
    }
    case ANCHOR_AIMBOX:
    {
        str += "AIM";
        break;
    }
    default:
        str += "HUH??";
        break;
    }

    outPoint = outPoint + Point(0, StrSize.height + 2);
    putText(tmp, str, outPoint, fontFace,
            fontScale, color, thickness);


    //***********************************
    // 距离NEAR, 状态AIMBOX时，输出对准目标信息
    //***********************************
    if(VInfo.AnchorDistState == ANCHOR_NEAR
            && VInfo.AnchorState == ANCHOR_AIMBOX)
    {
        str = "Aim's Target State: ";
        switch(VInfo.AimHole.TargetState)
        {
        case TARGET_NOTARGET:
        {
            str += "NO TARGET";
            break;
        }
        case TARGET_LEFT:
        {
            str += "LEFT";
            break;
        }
        case TARGET_RIGHT:
        {
            str += "RIGHT";
            break;
        }
        case TARGET_CENTER:
        {
            str += "CENTER";
            break;
        }
        default:
            str += "HUH??";
            break;
        }

        outPoint = outPoint + Point(0, StrSize.height + 2);
        putText(tmp, str, outPoint, fontFace,
                fontScale, color, thickness);
    }


    //*************************
    //     绘制图像部分
    //*************************

    switch(VInfo.AnchorState)
    {
    case ANCHOR_NOANCHOR:
    {
        break;
    }
    case ANCHOR_COVERONLY:
    {
        RotatedRect BoxRect, CoverRect;
        BoxRect = VInfo.BoxRect;
        CoverRect = VInfo.CoverHole.HoleRect;

        // 绘制投放标志物箱子外围
        PaintRotatedRect(tmp, tmp, BoxRect, WhiteColor, 2);
        // 绘制盖子外接矩形
        PaintRotatedRect(tmp, tmp, CoverRect, BlueColor, 1);

        break;
    }
    case ANCHOR_AIMBOX:
    {
        RotatedRect BoxRect, CoverRect, AimRect, PatternRect;
        BoxRect = VInfo.BoxRect;
        AimRect = VInfo.AimHole.HoleRect;

        if(VInfo.AnchorDistState == ANCHOR_NEAR)
        {
            // 绘制三个目标范围
            PaintRect(tmp, tmp, VInfo.AimHole.LeftTarget.rect,
                      GreenColor, 2);
            PaintRect(tmp, tmp, VInfo.AimHole.RightTarget.rect,
                      GreenColor, 2);
            PaintRect(tmp, tmp, VInfo.AimHole.CenterTarget.rect,
                      GreenColor, 2);
        }
        // 绘制投放标志物箱子外围
        PaintRotatedRect(tmp, tmp, BoxRect, WhiteColor, 2);
        // 绘制目标箱子外接矩形
        PaintRotatedRect(tmp, tmp, AimRect, RedColor, 2);
        circle(tmp, VInfo.AimHole.Center, 2, RedColor, 3);

        if(VInfo.CoverHole.HoleHave)
        {
            CoverRect = VInfo.CoverHole.HoleRect;
            // 绘制盖子外接矩形
            PaintRotatedRect(tmp, tmp, CoverRect, BlueColor, 1);
        }
        if(VInfo.PatternHole.HoleHave)
        {
            PatternRect = VInfo.PatternHole.HoleRect;
            // 绘制黄色图案外接矩形
            PaintRotatedRect(tmp, tmp, PatternRect, YellowColor, 1);
        }
        break;
    }
    default:
        str += "HUH??";
        break;
    }

    imshow("Anchor", tmp);
}

void NCVision::DrawOctCoins()
{
    Mat tmp;
    ImgPro.getDownFramePrePro(tmp);

    //***********************************
    //         图片中输出信息
    //***********************************

    string str;
    Size StrSize;
    Point outPoint;
    int baseline;
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int ExThickness = 2;
    Scalar color = Scalar(255, 255, 255);
    Scalar RedColor = Scalar(0, 0, 255);
    Scalar YellowColor = Scalar(0, 255, 255);
    Scalar GreenColor = Scalar(0, 255, 0);
    Scalar WhiteColor = Scalar(255, 255, 255, 0);
    Scalar BlueColor = Scalar(255, 255, 0);

    if(VInfo.RedCoinHave)
    {
        str = "Red Coin Have : YES";
    }
    else
        str = "Red Coin Have : NO";

    outPoint = Point(10, 20);
    StrSize = getTextSize(str, fontFace, fontScale, 1, &baseline);
    baseline += 1;
    putText(tmp, str, outPoint, fontFace,
            fontScale, color, thickness);

    if(VInfo.RedCoinHave)
    {
        PaintRotatedRect(tmp, tmp, VInfo.RedCoinRect,
                         RedColor, 2);
    }

    imshow("Red Coin", tmp);
}
