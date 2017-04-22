#include <ncvision.h>

//***********************************
//          分析函数
//***********************************

/*===================================================================
 * 函数：AnalyseGuideLine()
 *
 * 说明：分析二值图，获得寻线信息；
 *  1、检查是否存在Mat格式直线的二值图；
 *  2、设置工作深度（此处值暂时是为了调试随便设的值）；
 *  3、寻找轮廓，计算轮廓数量；
 *  (1)轮廓数量小于等于0，则线状态设置为GUIDE_NOLINE；
 *  (2)轮廓数量大于0：
 *   A、面积降序排列轮廓向量，删除剩余一个最大的轮廓；
 *   B、算出线的宽度的像素点大小，用于作判断；
 *   （4.28更改：不需要用模型算出来，避免图像与下位机的耦合；此处只需要多做实验，将
 *  不同深度的图像像素点大小对比出来制成表格，在该函数段依据表格直接赋值即可；）
 *   C、剩余的一个轮廓，用于拟合出Rect, RotatedRect各一个；
 *      其中Rect区域作ROI，RotatedRect用于获取直线长宽信息；
 *   D、计算RotatedRect在图中的长宽；
 *   E、判断RotatedRect的尺寸，与模型估算尺寸相比较：
 *      a、RotatedRect的尺寸过小，视为无线GUIDE_NOLINE；
 *      b、RotatedRect的尺寸较小，线视为点GUIDE_POINT；
 *      c、RotatedRect的长或宽大于模型尺寸，拟合线GUIDE_LINE；
 *   F、直线信息传入VInfo；
 *
 * 参数：
 *  int num:        识别门策略程序撰写月份
/*===================================================================
 */
void NCVision::AnalyseGuideLine(int num)
{
    switch(num)
    {
    case 4:
    {

        double angle;
        float line[4];
        Mat imgBinLine, imgtmp;
        ImgPro.getimgBinLine(imgBinLine);

        // 检查是否存在Mat格式直线的二值图
        if(imgBinLine.size().width <= 0 || imgBinLine.size().height <= 0)
        {
            cout<<"Analyse Guide Line Error: Have No Binary Line!"<<endl;
            return ;
        }

        // 设置工作深度（此处值暂时是为了调试随便设的值）
        DownModel.setSensorHeight(8);

        vector<vector<Point>> contours;

        imgBinLine.copyTo(imgtmp);
        // 寻找轮廓
        findContours(imgtmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        // 轮廓数量
        int contoursize = contours.size();

        // 若轮廓树小于0
        if(contoursize <= 0)
            VInfo.linestate = GUIDE_NOLINE;
        else
        {
            // 降序排列
            ContoursAreaDownSort(contours);
            // 删除剩余一个最大值
            DeleteContoursByRank(contours, 1);

            // 算出该深度下线宽的像素点大小，用于作判断
            // 4.28更改：不需要用模型算出来，避免图像与下位机的耦合；
            // 此处只需要多做实验，将不同深度的图像像素点大小对比出来制成表格，在该函数段依据表格直接赋值即可；
            double width_in_cam;
    //        DownModel.getLineWidthInCam(width_in_cam);
            width_in_cam = 20;

            // 剩余一个轮廓，拟合Rect, RotatedRect
            // Rect区域作ROI，RotatedRect用于获取直线长宽信息
            Mat pointsf;
            Mat(contours[0]).convertTo(pointsf, CV_32F);

            Rect ROIRect;
            RotatedRect LineRotRect;
            ROIRect = boundingRect(pointsf);
            LineRotRect = minAreaRect(pointsf);

            // 将ROI区域赋给VisionInfo
            VInfo.LineROIArea = ROIRect;

            // 计算RotatedRect在图中的长宽
            double linelen, linewid;
            linelen = LineRotRect.size.height;
            linewid = LineRotRect.size.width;
            if(linelen < linewid)
                swap(linelen, linewid);

            // 判断RotatedRect的尺寸，与模型估算尺寸相比较

            // RotatedRect的尺寸过小，视为无线
            if(linelen <= width_in_cam/2 && linewid <= width_in_cam/2)
            {
                VInfo.linestate = GUIDE_NOLINE;
            }
            // RotatedRect的尺寸较小，线视为点
            else if(linelen <= width_in_cam*1.2 && linewid <= width_in_cam*1.2)
            {
                VInfo.linestate = GUIDE_POINT;

                BinaryCenterScreenLine(imgBinLine, ROIRect, line, angle);
    //            cout<<endl<<"ROI Rect: "<<ROIRect.x<<" "<<ROIRect.width<<" "<<ROIRect.y<<" "<<ROIRect.height<<endl;

            }
            // RotatedRect的长或宽大于模型尺寸，拟合线
            else
            {
                VInfo.linestate = GUIDE_LINE;

                BinaryROIFitLine(imgBinLine, ROIRect, line, angle, 10);
    //            cout<<endl<<"ROI Rect: "<<ROIRect.x<<" "<<ROIRect.width<<" "<<ROIRect.y<<" "<<ROIRect.height<<endl;
            }

            // 直线信息传入VInfo
            for(int i = 0; i < 4; i++)
            {
                VInfo.GuideLine[i] = line[i];
            }
            VInfo.LineAngle = angle;
        }
        break;
    }
    case 6:
    {
        int linestate;
        int OffsetX, OffsetY;
        bool isaboveline = false;
        double angle;
        float line[4];
        Mat imgBinLine, imgtmp;
        ImgPro.getimgBinLine(imgBinLine);

        // 检查是否存在Mat格式直线的二值图
        if(imgBinLine.size().width <= 0 || imgBinLine.size().height <= 0)
        {
            cout<<"Analyse Guide Line Error: Have No Binary Line!"<<endl;
            return ;
        }

        vector<vector<Point>> contours;

        imgBinLine.copyTo(imgtmp);
        // 寻找轮廓
        findContours(imgtmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        // 降序排列
        ContoursAreaDownSort(contours);

        // 剔除长宽比过小的轮廓
        for(int i = contours.size() - 1; i >= 0; i--)
        {
            RotatedRect tmpRect;
            ContourFitRotatedRect(contours[i], tmpRect);
            double ratio;
            ratio = tmpRect.size.height / tmpRect.size.width;
            if(ratio < 1)       ratio = 1 / ratio;

            if(ratio < 1.5)
                contours.erase(contours.begin() + i);
        }

        // 轮廓数量
        int contoursize = contours.size();

        // 若轮廓树小于0
        if(contoursize <= 0)
            VInfo.linestate = GUIDE_NOLINE;
        else
        {
            // 删除剩余一个最大值
            DeleteContoursByRank(contours, 1);

            Rect ROIRect;
            RotatedRect LineRect;

            ContourFitROIRect(contours[0], ROIRect);
            ContourFitRotatedRect(contours[0], LineRect);

            // 计算RotatedRect在图中的长宽
            double linelen, linewid;
            linelen = LineRect.size.height;
            linewid = LineRect.size.width;
            if(linelen < linewid)
                swap(linelen, linewid);

            //*****************************************
            //  判断RotatedRect的尺寸，与模型估算尺寸相比较
            //*****************************************

            // RotatedRect的尺寸过小，视为无线
            if(linelen <= LINE_SIZE_LIMIT / 2
                    && linewid <= LINE_SIZE_LIMIT / 2)
            {
                linestate = GUIDE_NOLINE;
                for(int i = 0; i < 4; i++)
                    line[i] = 0;
            }
            // RotatedRect的尺寸较小，线视为点
            else if(linelen <= LINE_SIZE_LIMIT * 1.0
                    && linewid <= LINE_SIZE_LIMIT * 1.0)
            {
                linestate = GUIDE_POINT;
                BinaryCenterScreenLine(imgBinLine, ROIRect, line, angle);
            }
            // RotatedRect的长或宽大于模型尺寸，拟合线
            else
            {
                linestate = GUIDE_LINE;
                BinaryROIFitLine(imgBinLine, ROIRect, line, angle, 10);
            }

            //*****************************************
            //    计算导引路径ROI的X方向偏移量值
            //*****************************************

            if(linestate == GUIDE_NOLINE)
            {
                OffsetX = OffsetY = 0;
                ROIRect = Rect(-1, -1, -1, -1);
                isaboveline = false;
            }
            else
            {
                Size size = imgBinLine.size();
                int CenterX, CenterY;
                CenterX = ROIRect.x + ROIRect.width / 2 ;
                CenterY = ROIRect.y + ROIRect.height / 2;

                OffsetX = CenterX - (size.width / 2 - 1);
                OffsetY = CenterY - (size.height / 2 - 1);

                if(CenterY >= size.height * 1/4)
                    isaboveline = true;
                else
                    isaboveline = false;
            }

            //*****************************************
            //      向VInfo中传递数值
            //*****************************************

            VInfo.linestate = linestate;
            // 直线信息传入VInfo
            for(int i = 0; i < 4; i++)
                VInfo.GuideLine[i] = line[i];
            VInfo.LineAngle = angle;
            VInfo.LineROIArea = ROIRect;
            VInfo.LineOffsetX = OffsetX;
            VInfo.LineOffsetY = OffsetY;
            VInfo.IsAboveLine = isaboveline;
        }
        break;
    }
    default:
        cout<<"Wrong Analyse Line Number!"<<endl;
        break;
    }

}

/*===================================================================
 * 函数：AnalyseFirstDoor()
 *
 * 说明：分析二值图，获得过门信息；
 *  1、检查是否存在Mat格式第一个门的二值图；
 *  2、寻找轮廓，降序排列；
 *  3、判断轮廓数量：若轮廓数<=0，返回信息：没有找到门DOOR_NODOOR；
 *   若轮廓数>0:
 *   (1)删除剩余5个轮廓；
 *   (2)剔除自然光分量;
 *   (3)填充RotatedRect向量;
 *   (4)检查剩余几个轮廓的外接矩形，若外接矩形的中心在图像上限之上，则被视为自然光分量，剔除之；
 *   (5)更新此时的轮廓数量，再次检查轮廓数量，若轮廓数>=3，比较第二、三轮廓的面积比值：
 *   若第二轮廓与第三轮廓面积比值大于5，则剔除第三轮廓，此时轮廓数剩余2；
 *   (6)再次更新此时的轮廓数量，判断当前帧的门状态：
 *     a、size = 0: 无门状态DOOR_NODOOR；
 *     b、size = 1: 计算拟合RotatedRect的长宽比、角度值(>=1)，
 *          若长宽比大于某值，且倾斜角度不超过20度，则认为只有横梁DOOR_TOPONLY；
 *          否则，门状态为DOOR_OTHER；
 *     c、size = 2: 比较两个轮廓，将其作为上、下轮廓，并作位置调节；
 *          根据两轮廓拟合矩形的中心点x,y值判断为|——, ——|状态；否则，为状态DOOR_OTHER；
 *     d、size = 3: 三个轮廓位置调整；
 *          三个轮廓拟合矩形的角度值符合条件，则判断为WHOLEDOOR状态，否则为DOOR_OTHER；
 *   (7)缓存更新：
 *     a、处理Door相关缓存向量；
 *     b、门状态与门拟合矩形信息赋值给VInfo；
 *   (8)计算整个门状态时的相关信息：
 *     a、门状态与偏移值的计算；
 *     b、轮廓向量赋给VInfo；
 *     c、门状态赋给VInfo；
 *     d、平均矩形赋给VInfo；
 *
 * 参数：
 *  int num:        识别门策略程序撰写月份
/*===================================================================
 */
void NCVision::AnalyseFirstDoor(int num)
{
    switch(num)
    {
    case 4:
    {
        // 门状态
        int doorstate;
        // 门总轮廓
        RotatedRect doorRect;
        Mat imgBinDoor, imgtmp;
        ImgPro.getimgBinDoor(imgBinDoor);

        //*****************************************
        //    检查是否存在Mat格式第一个门的二值图
        //*****************************************
        if(imgBinDoor.size().width <= 0 || imgBinDoor.size().height <= 0)
        {
            cout<<"Analyse First Door Error: Have No Binary Door!"<<endl;
            return ;
        }

        // 寻找轮廓，按长度降序排列
        vector<vector<Point>> contours;
        imgBinDoor.copyTo(imgtmp);
        findContours(imgtmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        ContoursLengthDownSort(contours);
    //    ContoursAreaDownSort(contours);



        //*****************************************
        //              判断轮廓数量
        //*****************************************
        // 轮廓数量
        int contoursize = contours.size();

        // 轮廓数<=0，返回信息：没有找到门
        if(contoursize <= 0)
            VInfo.doorstate = DOOR_NODOOR;
        else
        {
            //*****************************************
            //        滤除杂波，判断是否存在门；
            //*****************************************

            // 求最大长度
            RotatedRect MaxContour;
            ContourFitRotatedRect(contours[0], MaxContour);
            float len, wid;
            len = MaxContour.size.height;
            wid = MaxContour.size.width;
            if(len <= wid)
                std::swap(len, wid);
            float contourMaxlen = len;

            // 若最大长度仍小于某值，则视为所有轮廓都为杂波，此时为无门状态
            if(contourMaxlen <= 20)
                VInfo.doorstate = DOOR_NODOOR;
            else
            {
                vector<RotatedRect> box;

                // 轮廓数>=5，删除剩5个
                // （只需要3个，剩5个是为了留余量）
                if (contoursize >= 5)
                {
                    // 删除剩余5个轮廓
                    DeleteContoursByRank(contours, 5);
                }


                //*****************************************
                //          剔除自然光分量
                //*****************************************
                // 更新轮廓数量
                contoursize = contours.size();

                int height = imgBinDoor.size().height;
                // 为图像设限，在该条线之上出现的轮廓被视为自然光分量轮廓，被剔除；
                int UpLimit = height / 8;


                // 填充RotatedRect向量
                for(int i = 0; i < contoursize; i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(contours[i], tmpRect);
                    box.push_back(tmpRect);
                }

                // 检查剩余几个轮廓的外接矩形，若外接矩形的中心在图像上限之上，则被视为自然光分量，剔除之；
                for(int i = contoursize - 1; i >= 0; --i)
                {
                    if(box[i].center.y < UpLimit)
                    {
                        contours.erase(contours.begin() + i);
                    }
                }

                //*****************************************
                //          比较第二、三轮廓面积
                //*****************************************

                // 更新此时的轮廓数量；
                contoursize = contours.size();

                // 再次检查轮廓数量
                // 若轮廓数>=3，比较第二、三轮廓的面积比值；
                if(contoursize >= 3)
                {
                    DeleteContoursByRank(contours, 3);

    //                RotatedRect SecondRect, ThirdRect;
    //                // 轮廓拟合RotatedRect
    //                ContourFitRotatedRect(contours[1], SecondRect);
    //                ContourFitRotatedRect(contours[2], ThirdRect);

    //                double SecondRectArea, ThirdRectArea;
    //                SecondRectArea = SecondRect.size.width * SecondRect.size.height;
    //                ThirdRectArea = ThirdRect.size.width * ThirdRect.size.height;

    //                if(SecondRectArea / 2 > ThirdRectArea)
    //                    contours.erase(contours.end()-1);

                    float SecondLen, ThirdLen;
                    ContourRotatedRectLength(contours[1], SecondLen);
                    ContourRotatedRectLength(contours[2], ThirdLen);

                    if(SecondLen / 3 > ThirdLen)
                        contours.erase(contours.end()-1);

                }

                // 再次更新此时的轮廓数量；
                contoursize = contours.size();

                //*****************************************
                //           判断当前帧的门状态
                //*****************************************

                if(contoursize == 0)
                {
                    doorstate = DOOR_NODOOR;
                }
                else if(contoursize == 1)
                {
                    RotatedRect rect;
                    ContourFitRotatedRect(contours[0], rect);

                    // 计算拟合RotatedRect的长宽比、角度值(>=1)
                    double ratio;
                    double angle;
                    ratio = rect.size.width / rect.size.height;
                    angle = rect.angle;

                    angle = fabs(angle);
                    if(ratio < 1)
                        ratio = 1 / ratio;

                    // 长宽比大于某值，且倾斜角度不超过20度，则认为只有横梁；
                    if(ratio >= 3 && angle <= 20)
                        doorstate = DOOR_TOPONLY;
                    else
                        doorstate = DOOR_OTHER;
                }
                else if(contoursize == 2)
                {
                    RotatedRect UpRect, DownRect;
                    ContourFitRotatedRect(contours[0], UpRect);
                    ContourFitRotatedRect(contours[1], DownRect);

                    // 比较上下轮廓，并作位置调节
                    if(UpRect.center.y > DownRect.center.y)
                        swap(UpRect, DownRect);

                    // |--  形状
                    if(UpRect.center.x > DownRect.center.x
                            && UpRect.center.y < DownRect.center.y)
                        doorstate = DOOR_HALFLEFT;

                    // --|  形状
                    else if(UpRect.center.x < DownRect.center.x
                            && UpRect.center.y < DownRect.center.y)
                        doorstate = DOOR_HALFRIGHT;
                    else
                        doorstate = DOOR_OTHER;
                }
                else if(contoursize == 3)
                {
                    RotatedRect TopRect, SideRect1, SideRect2;
                    ContourFitRotatedRect(contours[0], TopRect);
                    ContourFitRotatedRect(contours[1], SideRect1);
                    ContourFitRotatedRect(contours[2], SideRect2);

                    // 三个轮廓位置调整；
                    if(TopRect.center.y > SideRect1.center.y)
                        swap(TopRect, SideRect1);
                    if(TopRect.center.y > SideRect2.center.y)
                        swap(TopRect, SideRect2);
                    if(SideRect1.center.x > SideRect2.center.x)
                        swap(SideRect1, SideRect2);

        //            cout<<"Top: "<<TopRect.center.x<<","<<TopRect.center.y<<endl;
        //            cout<<"Side1: "<<SideRect1.center.x<<","<<SideRect1.center.y<<endl;
        //            cout<<"Side2: "<<SideRect2.center.x<<","<<SideRect2.center.y<<endl;

                    double UpAngle;
                    double SideAngle1, SideAngle2;

                    UpAngle = TopRect.angle;
                    SideAngle1 = fabs(SideRect1.angle);
                    SideAngle2 = fabs(SideRect2.angle);

                    // 三个轮廓拟合矩形的角度值符合条件，则判断为WHOLEDOOR状态，否则为DOOR_OTHER；

                    // 轮廓角度适合
                    if(fabs(UpAngle) < 30 && fabs(90 - SideAngle1) > 60 && fabs(90 - SideAngle2) > 60
                            // 三个轮廓的x相对位置正确
                            && TopRect.center.x > SideRect1.center.x && TopRect.center.x < SideRect2.center.x
                            && TopRect.center.y < SideRect1.center.y && TopRect.center.y < SideRect2.center.y)
                        doorstate = DOOR_WHOLEDOOR;
                    else
                        doorstate = DOOR_OTHER;
                }
            }
        }

        //*****************************************
        //            缓存更新
        //*****************************************

        ContoursMinAreaRect(contours, doorRect);
        // 处理Door相关缓存向量；
        UpdateTmpDoorState(doorstate);
        UpdateTmpDoorRect(doorstate, doorRect);
        // 门状态与门拟合矩形信息赋值给VInfo；
        ProcessTmpDoor(doorstate, doorRect);


        //*****************************************
        //       计算整个门状态时的相关信息
        //*****************************************

        //====================
        // 门状态与偏移值的计算：
        //====================

        // 识别到整个门的时候，计算与门拟合矩形与屏幕中心的X方向偏移值；
        // 负值为左偏，正值为右偏；
        if(doorstate == 3)
        {
            ContoursMinAreaRect(contours, doorRect);

            Point2f doorcenter;
            doorcenter = doorRect.center;

            int ScreenCenter = imgtmp.size().width / 2 - 1;

            VInfo.CenterOffsetX = doorcenter.x - ScreenCenter;
        }
        else
        {
            // 其他状态，则偏移值回归一个定值（此处暂定为0）
            VInfo.CenterOffsetX = 0;
        }

        //====================
        //  轮廓向量赋给VInfo
        //====================
        contoursize = contours.size();
        VInfo.DoorRects.clear();
        for(int i = 0; i < contoursize; i++)
        {
            RotatedRect tmpRect;
            ContourFitRotatedRect(contours[i], tmpRect);
            VInfo.DoorRects.push_back(tmpRect);
        }

        //====================
        //  门状态赋给VInfo
        //====================
        VInfo.doorstate = doorstate;

        //====================
        //  平均矩形赋给VInfo
        //====================
        VInfo.AverageRect = doorRect;
        break;
    }
    case 5:
    {
        //************************
        //      分析线位置
        //************************

        Mat imgBinLineID;
        Mat imgtmp;
        int lineIDState;
        Rect LineIDROI;
        ImgPro.getimgBinLineID(imgBinLineID);

        //*****************************************
        //    检查是否存在Mat格式第一个门的二值图
        //*****************************************
        if(imgBinLineID.size().width <= 0 || imgBinLineID.size().height <= 0)
        {
            cout<<"Analyse First Door Error: Have No Binary Line!"<<endl;
            return ;
        }

        // 寻找轮廓，按面积降序排列
        vector<vector<Point>> LineContours;
        imgBinLineID.copyTo(imgtmp);
        findContours(imgtmp, LineContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        ContoursAreaDownSort(LineContours);

        //*****************************************
        //              判断轮廓数量
        //*****************************************

        // 轮廓数量
        int lineIDsize = LineContours.size();

        //
        if(lineIDsize <= 0)
            lineIDState = DOOR_NOLINE;
        else
        {
            vector<RotatedRect> box;

            // 轮廓数>=3，删除剩3个
            // （只需要1个，剩3个是为了留余量）
            if(lineIDsize > 3)
            {
                DeleteContoursByRank(LineContours, 3);
            }

            // 剔除自然光分量
            DeleteTopLightVoice(imgBinLineID, LineContours, 3);

            // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
            DeleteSmallContours(LineContours, DOOR_LINE_SIZELIMIT);

            //*****************************************
            //  再查轮廓数量，面积降序排序，最大者为线的ROI
            //*****************************************

            // 更新轮廓数量
            lineIDsize = LineContours.size();

            if(lineIDsize <= 0)
                lineIDState = DOOR_NOLINE;
            else
            {
                ContoursAreaDownSort(LineContours);
                // 剩余一个轮廓，拟合Rect, RotatedRect
                // Rect区域作ROI，RotatedRect用于获取直线长宽信息
                Mat pointsf;
                Mat(LineContours[0]).convertTo(pointsf, CV_32F);

                LineIDROI = boundingRect(pointsf);

                VInfo.LineIDROI = LineIDROI;
                lineIDState = DOOR_FOUNDLINE;
            }
        }

        // 向VisionInfo中赋值
        VInfo.lineIDstate = lineIDState;

        //************************
        //      分析门状态
        //************************

        // 门状态
        int doorstate;
        // 门总轮廓
        RotatedRect doorRect;
        Mat imgBinDoor;
        ImgPro.getimgBinDoor(imgBinDoor);

        //*****************************************
        //    检查是否存在Mat格式第一个门的二值图
        //*****************************************
        if(imgBinDoor.size().width <= 0 || imgBinDoor.size().height <= 0)
        {
            cout<<"Analyse First Door Error: Have No Binary Door!"<<endl;
            return ;
        }

        // 寻找轮廓，按长度降序排列
        vector<vector<Point>> contours;
        imgBinDoor.copyTo(imgtmp);
        findContours(imgtmp, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        ContoursLengthDownSort(contours);
    //    ContoursAreaDownSort(contours);

        //*****************************************
        //              判断轮廓数量
        //*****************************************
        // 轮廓数量
        int contoursize = contours.size();

        // 轮廓数<=0，返回信息：没有找到门
        if(contoursize <= 0)
            doorstate = DOOR_NODOOR;
        else
        {
            //*****************************************
            //        滤除杂波，判断是否存在门；
            //*****************************************

            // 求最大长度
            RotatedRect MaxContour;
            ContourFitRotatedRect(contours[0], MaxContour);
            float len, wid;
            len = MaxContour.size.height;
            wid = MaxContour.size.width;
            if(len <= wid)
                std::swap(len, wid);
            float contourMaxlen = len;

            // 若最大长度仍小于某值，则视为所有轮廓都为杂波，此时为无门状态
            if(contourMaxlen <= DOOR_CONTOURMAXLIMIT)
//                VInfo.doorstate = DOOR_NODOOR;
                doorstate = DOOR_NODOOR;
            else
            {
                vector<RotatedRect> box;

                // 填充RotatedRect向量
                for(int i = contoursize - 1; i >= 0; --i)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(contours[i], tmpRect);
                    if(tmpRect.center.x > LineIDROI.x
                            && tmpRect.center.x < LineIDROI.x + LineIDROI.width
                            && tmpRect.center.y > LineIDROI.y
                            && tmpRect.center.y < LineIDROI.y + LineIDROI.height)
                    {
                        contours.erase(contours.begin() + i);
                    }
                }

                // 轮廓数>=6，删除剩6个
                // （只需要3个，剩6个是为了留余量）
                if (contoursize >= 6)
                {
                    // 删除剩余5个轮廓
                    DeleteContoursByRank(contours, 6);
                }

                //*****************************************
                //          剔除自然光分量
                //*****************************************

                DeleteTopLightVoice(imgBinDoor, contours, 8);

                //*****************************************
                //          判断LineID状态
                //*****************************************

                // 前视摄像头中若没有看到线，则必定看不到门
                if(lineIDState == 0)
                    doorstate = DOOR_NODOOR;
                else if(lineIDState)
                {
                    //*****************************************
                    //    根据线ROI，判断距离是否在2m之内
                    //*****************************************

                    int ROIwidth, ROIheight, ROIMax;
                    ROIwidth = LineIDROI.width;
                    ROIheight = LineIDROI.height;

                    if(ROIwidth > ROIheight)    ROIMax = ROIwidth;
                    else    ROIMax = ROIheight;

                    // 判断ROI大小，根据大小的判断结果判断距离的远近
                    if(ROIMax <= DOOR_FAR_LIMIT)
                    {
                        doorstate = DOOR_NODOOR;
                        VInfo.lineIDFar = DOOR_FAR;
                    }
                    else
                    {
                        VInfo.lineIDFar = DOOR_NEAR;

                        // 更新轮廓数量
                        contoursize = contours.size();

                        //*****************************************
                        //          比较第二、三轮廓面积
                        //*****************************************

                        // 更新此时的轮廓数量；
                        contoursize = contours.size();

                        // 再次检查轮廓数量
                        // 若轮廓数>=3，比较第二、三轮廓的面积比值；
                        if(contoursize >= 3)
                        {
                            DeleteContoursByRank(contours, 3);

            //                RotatedRect SecondRect, ThirdRect;
            //                // 轮廓拟合RotatedRect
            //                ContourFitRotatedRect(contours[1], SecondRect);
            //                ContourFitRotatedRect(contours[2], ThirdRect);

            //                double SecondRectArea, ThirdRectArea;
            //                SecondRectArea = SecondRect.size.width * SecondRect.size.height;
            //                ThirdRectArea = ThirdRect.size.width * ThirdRect.size.height;

            //                if(SecondRectArea / 2 > ThirdRectArea)
            //                    contours.erase(contours.end()-1);

                            float SecondLen, ThirdLen;
                            ContourRotatedRectLength(contours[1], SecondLen);
                            ContourRotatedRectLength(contours[2], ThirdLen);

                            if(SecondLen / DOOR_LENGTHRATIO_2_3 > ThirdLen)
                                contours.erase(contours.end()-1);

                        }

                        // 再次更新此时的轮廓数量；
                        contoursize = contours.size();

                        //*****************************************
                        //           判断当前帧的门状态
                        //*****************************************

                        cout<<"Second Third Done."<<endl;
                        cout<<"Contour Size: "<<contoursize<<endl;

                        if(contoursize == 0)
                        {
                            doorstate = DOOR_NODOOR;
                        }
                        else if(contoursize == 1)
                        {
                            RotatedRect rect;
                            ContourFitRotatedRect(contours[0], rect);

                            // 计算拟合RotatedRect的长宽比、角度值(>=1)
                            double ratio;
                            double angle;
                            ratio = rect.size.width / rect.size.height;
                            angle = rect.angle;

                            angle = fabs(angle);
                            if(ratio < 1)
                                ratio = 1 / ratio;

                            // 长宽比大于某值，且倾斜角度不超过20度，则认为只有横梁；
                            if(ratio >= 3 && angle <= 20)
                                doorstate = DOOR_TOPONLY;
                            else
                                doorstate = DOOR_OTHER;
                        }
                        else if(contoursize == 2)
                        {
                            RotatedRect UpRect, DownRect;
                            ContourFitRotatedRect(contours[0], UpRect);
                            ContourFitRotatedRect(contours[1], DownRect);

                            // 比较上下轮廓，并作位置调节
                            if(UpRect.center.y > DownRect.center.y)
                                swap(UpRect, DownRect);

                            // |--  形状
                            if(UpRect.center.x > DownRect.center.x
                                    && UpRect.center.y < DownRect.center.y)
                                doorstate = DOOR_HALFLEFT;

                            // --|  形状
                            else if(UpRect.center.x < DownRect.center.x
                                    && UpRect.center.y < DownRect.center.y)
                                doorstate = DOOR_HALFRIGHT;
                            else
                                doorstate = DOOR_OTHER;
                        }
                        else if(contoursize == 3)
                        {
                            RotatedRect TopRect, SideRect1, SideRect2;
                            ContourFitRotatedRect(contours[0], TopRect);
                            ContourFitRotatedRect(contours[1], SideRect1);
                            ContourFitRotatedRect(contours[2], SideRect2);

                            // 三个轮廓位置调整；
                            if(TopRect.center.y > SideRect1.center.y)
                                swap(TopRect, SideRect1);
                            if(TopRect.center.y > SideRect2.center.y)
                                swap(TopRect, SideRect2);
                            if(SideRect1.center.x > SideRect2.center.x)
                                swap(SideRect1, SideRect2);

                //            cout<<"Top: "<<TopRect.center.x<<","<<TopRect.center.y<<endl;
                //            cout<<"Side1: "<<SideRect1.center.x<<","<<SideRect1.center.y<<endl;
                //            cout<<"Side2: "<<SideRect2.center.x<<","<<SideRect2.center.y<<endl;

                            double UpAngle;
                            double SideAngle1, SideAngle2;

                            UpAngle = TopRect.angle;
                            SideAngle1 = fabs(SideRect1.angle);
                            SideAngle2 = fabs(SideRect2.angle);

                            // 三个轮廓拟合矩形的角度值符合条件，则判断为WHOLEDOOR状态，否则为DOOR_OTHER；

                            // 轮廓角度适合
                            if(fabs(UpAngle) > 60 && fabs(90 - SideAngle1) > 60 && fabs(90 - SideAngle2) > 60
                                    // 三个轮廓的x相对位置正确
                                    && TopRect.center.x > SideRect1.center.x && TopRect.center.x < SideRect2.center.x
                                    && TopRect.center.y < SideRect1.center.y && TopRect.center.y < SideRect2.center.y)
                                doorstate = DOOR_WHOLEDOOR;
                            else
                                doorstate = DOOR_OTHER;
                        }
                    }
                }
            }
        }

        cout<<"Door State tmp: "<<doorstate<<endl;

//        Mat img;
//        img.create(imgBinDoor.size(), CV_8UC3);
//        drawContours(img, contours, -1, Scalar(255), 2);
//        imshow("Contours", img);

        //*****************************************
        //            缓存更新
        //*****************************************

        // 处理LineID相关缓存向量；
        UpdateTmpLineIDState(lineIDState);
        UpdateTmpLineIDROI(lineIDState, LineIDROI);

        // LineID状态与LineID拟合矩形信息赋值给VInfo；
        ProcessTmpLineID(lineIDState, LineIDROI);


        ContoursMinAreaRect(contours, doorRect);
        // 处理Door相关缓存向量；
        UpdateTmpDoorState(doorstate);
        UpdateTmpDoorRect(doorstate, doorRect);
        // 门状态与门拟合矩形信息赋值给函数参数；
        ProcessTmpDoor(doorstate, doorRect);


        //*****************************************
        //       计算整个门状态时的相关信息
        //*****************************************

        //===========================
        // 导引路径ROI的X方向偏移量计算：
        //===========================

        // 识别到线的时候，计算导引路径ROI区域与屏幕中心X方向的偏移值；
        // 负值为左偏，正值为右偏；
        if(lineIDState == DOOR_FOUNDLINE)
        {
            int LineROICenter = LineIDROI.x + LineIDROI.width / 2;
            int ScreenCenter= imgtmp.size().width / 2 - 1;

            VInfo.LineIDOffsetX = LineROICenter - ScreenCenter;
        }
        else
        {
            // 其他状态，则偏移值回归一个定值（此处暂定为0）
            VInfo.LineIDOffsetX = 0;
        }

        //===========================
        //    门偏移值的计算：
        //===========================

        // 识别到整个门的时候，计算与门拟合矩形与屏幕中心的X方向偏移值；
        // 负值为左偏，正值为右偏；
        if(doorstate == DOOR_WHOLEDOOR)
        {
            ContoursMinAreaRect(contours, doorRect);

            Point2f doorcenter;
            doorcenter = doorRect.center;

            int ScreenCenter = imgtmp.size().width / 2 - 1;

            VInfo.CenterOffsetX = doorcenter.x - ScreenCenter;
        }
        else
        {
            // 其他状态，则偏移值回归一个定值（此处暂定为0）
            VInfo.CenterOffsetX = 0;
        }

        //====================
        //  轮廓向量赋给VInfo
        //====================
        contoursize = contours.size();
        VInfo.DoorRects.clear();
        for(int i = 0; i < contoursize; i++)
        {
            RotatedRect tmpRect;
            ContourFitRotatedRect(contours[i], tmpRect);
            VInfo.DoorRects.push_back(tmpRect);
        }

        //====================
        //  平均ROI赋给VInfo
        //====================
        VInfo.LineIDROI = LineIDROI;

        //====================
        //  门状态赋给VInfo
        //====================
        VInfo.doorstate = doorstate;

        //====================
        //  平均矩形赋给VInfo
        //====================
        VInfo.AverageRect = doorRect;


        break;
    }
    case 6:
    {
        //************************
        //      分析线位置
        //************************

        Mat imgBinLineID;
        Mat imgtmp;
        int lineIDState;
        int lineIDFar;
        Rect LineIDROI;
        ImgPro.getimgBinLineID(imgBinLineID);

        //*****************************************
        //    检查是否存在Mat格式第一个门的二值图
        //*****************************************
        if(imgBinLineID.size().width <= 0 || imgBinLineID.size().height <= 0)
        {
            cout<<"Analyse First Door Error: Have No Binary Line!"<<endl;
            return ;
        }

        // 寻找轮廓，按面积降序排列
        vector<vector<Point>> LineContours;
        imgBinLineID.copyTo(imgtmp);
        findContours(imgtmp, LineContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        ContoursAreaDownSort(LineContours);

        //*****************************************
        //              判断轮廓数量
        //*****************************************

        // 轮廓数量
        int lineIDsize = LineContours.size();

        if(lineIDsize <= 0)
            lineIDState = DOOR_NOLINE;
        else
        {
            // 轮廓数>=3，删除剩3个
            // （只需要1个，剩3个是为了留余量）
            if(lineIDsize > 3)
            {
                DeleteContoursByRank(LineContours, 3);
            }

            // 剔除自然光分量
            DeleteTopLightVoice(imgBinLineID, LineContours, 3);

            // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
            DeleteSmallContours(LineContours, DOOR_LINE_SIZELIMIT);

            //*****************************************
            //  再查轮廓数量，面积降序排序，最大者为线的ROI
            //*****************************************

            // 更新轮廓数量
            lineIDsize = LineContours.size();

            if(lineIDsize <= 0)
                lineIDState = DOOR_NOLINE;
            else
            {
                ContoursAreaDownSort(LineContours);

                // 拟合ROI区域
                ContourFitROIRect(LineContours[0], LineIDROI);

                lineIDState = DOOR_FOUNDLINE;

                //*****************************************
                //    根据线ROI，判断距离是否在2m之内
                //*****************************************

                int ROIwidth, ROIheight, ROIMax;
                ROIwidth = LineIDROI.width;
                ROIheight = LineIDROI.height;

                if(ROIwidth > ROIheight)    ROIMax = ROIwidth;
                else    ROIMax = ROIheight;

                // 判断ROI大小，根据大小的判断结果判断距离的远近
                if(ROIMax <= DOOR_FAR_LIMIT)
                    lineIDFar = DOOR_FAR;
                else
                    lineIDFar = DOOR_NEAR;
            }
        }

        //*****************************************
        //            缓存更新
        //*****************************************

        // 处理LineID相关缓存向量；
        UpdateTmpLineIDState(lineIDState);
        UpdateTmpLineIDROI(lineIDState, LineIDROI);

        // LineID状态与LineID拟合矩形信息赋值给VInfo；
        ProcessTmpLineID(lineIDState, LineIDROI);

        //*****************************************
        //       计算整个门状态时的相关信息
        //*****************************************

        //====================
        //  LineIDState赋给VInfo
        //====================
        VInfo.lineIDstate = lineIDState;

        //====================
        //  LineIDFar赋给VInfo
        //====================
        if(lineIDState == DOOR_NOLINE)
            lineIDFar = DOOR_FAR;
        VInfo.lineIDFar = lineIDFar;

        //====================
        //  LineIDROI赋给VInfo
        //====================
        if(lineIDState == DOOR_NOLINE)
            VInfo.LineIDROI = Rect(-1, -1, -1, -1);
        else
            VInfo.LineIDROI = LineIDROI;

        //===========================
        // 导引路径ROI的X方向偏移量计算：
        //===========================

        // 识别到线的时候，计算导引路径ROI区域与屏幕中心X方向的偏移值；
        // 负值为左偏，正值为右偏；
        if(lineIDState == DOOR_FOUNDLINE)
        {
            int LineROICenter = LineIDROI.x + LineIDROI.width / 2;
            int ScreenCenter= imgtmp.size().width / 2 - 1;

            VInfo.LineIDOffsetX = LineROICenter - ScreenCenter;
        }
        else
        {
            // 其他状态，则偏移值回归一个定值（此处暂定为0）
            VInfo.LineIDOffsetX = 0;
        }

        break;
    }
    case 7:
    {
        //************************
        //      分析线位置
        //************************

        Mat imgBinLineID;
        Mat imgtmp;
        int lineIDState;
        int lineIDFar;
        Rect LineIDROI;
        ImgPro.getimgBinLineID(imgBinLineID);

        //*****************************************
        //    检查是否存在Mat格式第一个门的二值图
        //*****************************************
        if(imgBinLineID.size().width <= 0 || imgBinLineID.size().height <= 0)
        {
            cout<<"Analyse First Door Error: Have No Binary Line!"<<endl;
            return ;
        }

        // 寻找轮廓，按面积降序排列
        vector<vector<Point>> LineContours;
        imgBinLineID.copyTo(imgtmp);
        findContours(imgtmp, LineContours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        ContoursAreaDownSort(LineContours);


        //*****************************************
        //              判断轮廓数量
        //*****************************************

        // 轮廓数量
        int lineIDsize = LineContours.size();

        if(lineIDsize <= 0)
            lineIDState = DOOR_NOLINE;
        else
        {
            // 剔除自然光分量
            DeleteTopLightVoice(imgBinLineID, LineContours, 1.5);

            // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
            DeleteSmallContours(LineContours, DOOR_LINE_SIZELIMIT);

            // 更新轮廓数量
            lineIDsize = LineContours.size();
            if(lineIDsize > 3)
                DeleteContoursByRank(LineContours, 3);

            lineIDsize = LineContours.size();

            if(lineIDsize <= 0)
                lineIDState = DOOR_NOLINE;
            else
            {
                ContoursUpWardDownSort(LineContours);

                // 拟合ROI区域
                ContourFitROIRect(LineContours[0], LineIDROI);

                lineIDState = DOOR_FOUNDLINE;

                //*****************************************
                //    根据线ROI，判断距离是否在2m之内
                //*****************************************

                int ROIwidth, ROIheight, ROIMax;
                ROIwidth = LineIDROI.width;
                ROIheight = LineIDROI.height;

                if(ROIwidth > ROIheight)    ROIMax = ROIwidth;
                else    ROIMax = ROIheight;

                // 判断ROI大小，根据大小的判断结果判断距离的远近
                if(ROIMax <= DOOR_FAR_LIMIT)
                    lineIDFar = DOOR_FAR;
                else
                    lineIDFar = DOOR_NEAR;
            }
        }

        //*****************************************
        //            缓存更新
        //*****************************************

//        // 处理LineID相关缓存向量；
//        UpdateTmpLineIDState(lineIDState);
//        UpdateTmpLineIDROI(lineIDState, LineIDROI);

//        // LineID状态与LineID拟合矩形信息赋值给VInfo；
//        ProcessTmpLineID(lineIDState, LineIDROI);

        //*****************************************
        //       计算整个门状态时的相关信息
        //*****************************************

        //====================
        //  LineIDState赋给VInfo
        //====================
        VInfo.lineIDstate = lineIDState;

        //====================
        //  LineIDFar赋给VInfo
        //====================
        if(lineIDState == DOOR_NOLINE)
            lineIDFar = DOOR_FAR;
        VInfo.lineIDFar = lineIDFar;

        //====================
        //  LineIDROI赋给VInfo
        //====================
        if(lineIDState == DOOR_NOLINE)
            VInfo.LineIDROI = Rect(-1, -1, -1, -1);
        else
            VInfo.LineIDROI = LineIDROI;

        //===========================
        // 导引路径ROI的X方向偏移量计算：
        //===========================

        // 识别到线的时候，计算导引路径ROI区域与屏幕中心X方向的偏移值；
        // 负值为左偏，正值为右偏；
        if(lineIDState == DOOR_FOUNDLINE)
        {
            int LineROICenter = LineIDROI.x + LineIDROI.width / 2;
            int ScreenCenter= imgtmp.size().width / 2 - 1;

            VInfo.LineIDOffsetX = LineROICenter - ScreenCenter;
        }
        else
        {
            // 其他状态，则偏移值回归一个定值（此处暂定为0）
            VInfo.LineIDOffsetX = 0;
        }

        break;
    }
    default:
        cout<<"Wrong Anaylyse First Door Number!"<<endl;
        break;
    }
}

// 分析二值图，获得撞球信息；
void NCVision::AnalyseBalls(int ColorNum, int num)
{
    switch(num)
    {
    case 5:
    {
        // 该时刻球距离状态，作为缓存
        int ballstate;
        ballstate = VInfo.ballsstate;

        // 视野中红黄绿球存在状况
        int RedHave = 0;
        int YellowHave = 0;
        int GreenHave = 0;
        // 视野中红黄绿球距离状况
        int RedState = 0;
        int YellowState = 0;
        int GreenState = 0;
        // 视野中红黄绿球的位置（对应的Have状态不为0时起作用）
        RotatedRect RedRect;
        RotatedRect YellowRect;
        RotatedRect GreenRect;

        switch(ballstate)
        {

        //************************
        //      球在远距离状态
        //************************
        case BALL_FAR:
        {
            // 远距离状态下，默认不存在红黄绿三球；
            RedHave = 0;
            YellowHave = 0;
            GreenHave = 0;

            Mat imgBallsFar;
            Mat imgtmp;
            ImgPro.getimgBinBallsFar(imgBallsFar);

            //*****************************************
            //    检查是否存在Mat格式远状态球的二值图
            //*****************************************

            if(imgBallsFar.empty())
            {
                cout<<"Analyse Balls Error: Have No Binary Far Balls Mat!"<<endl;
                return ;
            }

            // 寻找轮廓，按面积降序排列
            vector<vector<Point>> BallsFarContours;
            imgBallsFar.copyTo(imgtmp);
            findContours(imgtmp, BallsFarContours,
                         CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            ContoursAreaDownSort(BallsFarContours);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int BallsFarsize = BallsFarContours.size();

            if(BallsFarsize <= 0)
            {
                ballstate = BALL_FAR;
            }
            else
            {
                if(BallsFarsize >= 5)
                    DeleteContoursByRank(BallsFarContours, 5);

                // 剔除自然光分量
                DeleteTopLightVoice(imgBallsFar, BallsFarContours, 5);

                // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                DeleteSmallContours(BallsFarContours, BALL_NOBALL_LIMIT);

                //*****************************************
                //           判断最大轮廓
                //*****************************************

                // 更新轮廓数量
                BallsFarsize = BallsFarContours.size();

                // 此时少于两个轮廓，则视为无球；
                if(BallsFarsize <= 1)
                {
                    ballstate = BALL_FAR;
                }
                else
                {
                    ContoursLengthDownSort(BallsFarContours);

                    float MaxLength;
                    ContourRotatedRectLength(BallsFarContours[0], MaxLength);

                    if(MaxLength >= 0 && MaxLength < BALL_NEAR_LIMIT)
                        ballstate = BALL_FAR;
                    else if(MaxLength >= BALL_NEAR_LIMIT)
                        ballstate = BALL_NEAR;
                }
            }

//            cout<<"Ball State: "<<ballstate<<endl<<endl;

            break;
        }
        //************************
        //      球在近距离状态
        //************************
        case BALL_NEAR:
        {
            Mat imgRed, imgYelGre;
            Mat imgtmp1, imgtmp2;
            ImgPro.getimgBinRedBall(imgRed);
            ImgPro.getimgBinYelGreBalls(imgYelGre);

            //*****************************************
            //    检查是否存在Mat格式近状态球的二值图
            //*****************************************

            if(imgRed.empty() || imgYelGre.empty())
            {
                cout<<"Analyse Balls Error: Have No Binary Yellow or Green Mats!"<<endl;
                return ;
            }

            // 寻找轮廓，按面积降序排列
            vector<vector<Point>> RedBallContours;
            vector<vector<Point>> YelGreBallsContours;
            imgRed.copyTo(imgtmp1);
            imgYelGre.copyTo(imgtmp2);
            findContours(imgtmp1, RedBallContours,
                         CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
            findContours(imgtmp2, YelGreBallsContours,
                         CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
            ContoursAreaDownSort(RedBallContours);
            ContoursAreaDownSort(YelGreBallsContours);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int RedSize = RedBallContours.size();
            int YelGreSize = YelGreBallsContours.size();


            if(RedSize + YelGreSize <= 0)
                ballstate = BALL_FAR;
            else
            {
                if(RedSize >= 4)
                    DeleteContoursByRank(RedBallContours, 4);
                if(YelGreSize >= 4)
                    DeleteContoursByRank(YelGreBallsContours, 4);

                // 剔除自然光分量
                DeleteTopLightVoice(imgRed, RedBallContours, 5);
                DeleteTopLightVoice(imgYelGre, YelGreBallsContours, 5);

                // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                DeleteSmallContours(RedBallContours, BALL_NOBALL_LIMIT);
                DeleteSmallContours(YelGreBallsContours, BALL_NOBALL_LIMIT);

                //*****************************************
                //             先判断红球
                //*****************************************

                // 更新红球轮廓向量个数
                RedSize = RedBallContours.size();

                if(RedSize <= 0)
                    RedHave = BALL_NOBALL;
                else
                {
                    RedHave = BALL_HAVE;

                    // 取最左的轮廓为红球轮廓
                    ContourFitRotatedRect(RedBallContours[0], RedRect);
                    // 记录最大面积的RedRect
                    RotatedRect ReadyRedRect;
                    ReadyRedRect = RedRect;

                    for(int i = 0; i < RedSize; i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(RedBallContours[i], tmpRect);
                        if(RedRect.center.x > tmpRect.center.x)
                            swap(RedRect, tmpRect);
                    }

                    // 若最左轮廓的面积小于最大面积的一半，则两者交换
                    if(RedRect.size.area() < ReadyRedRect.size.area() / 2)
                        swap(RedRect, ReadyRedRect);

                    // 判断红球轮廓大小
                    float RedRectLength;
                    RedRectLength = (RedRect.size.height > RedRect.size.width)
                            ? RedRect.size.height : RedRect.size.width;

//                    cout<<"Red Rect Length: "<<RedRectLength<<endl;

                    // 根据红球轮廓尺寸大小，判断红球距离状态；
                    if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                        RedState = BALL_FAR;
                    else if(RedRectLength >= BALL_NEAR_LIMIT
                            && RedRectLength < BALL_ATTACH_LIMIT)
                        RedState = BALL_NEAR;
                    else
                        RedState = BALL_ATTACH;

//                    cout<<"Red State: "<<RedState<<endl;
                }

                //*****************************************
                //             判断黄、绿球
                //*****************************************

                // 更新黄绿球轮廓向量个数
                YelGreSize = YelGreBallsContours.size();

                // 先将红球位置的分量剔除
                if(RedHave != BALL_NOBALL)
                {
                    Rect tmpRedROI;

                    tmpRedROI = RedRect.boundingRect();
                    for(int i = YelGreSize - 1; i >= 0; --i)
                    {
                        RotatedRect tmpYelGreRect;
                        ContourFitRotatedRect(YelGreBallsContours[i], tmpYelGreRect);
                        if(tmpYelGreRect.center.x > tmpRedROI.x
                                && tmpYelGreRect.center.x < (tmpRedROI.x + tmpRedROI.width)
                                && tmpYelGreRect.center.y > tmpRedROI.y
                                && tmpYelGreRect.center.y < (tmpRedROI.y + tmpRedROI.height))
                            YelGreBallsContours.erase(YelGreBallsContours.begin() + i);
                    }
                }

                DeleteSmallContours(YelGreBallsContours, (BALL_NOBALL_LIMIT + BALL_NEAR_LIMIT) / 2);

                // 更新黄绿球轮廓向量个数
                YelGreSize = YelGreBallsContours.size();

                if(YelGreSize <= 0)
                {
                    YellowHave = BALL_NOBALL;
                    GreenHave = BALL_NOBALL;
                }
                else
                {
                    //*****************************************
                    //        轮廓数为1时，分别判断黄、绿球
                    //*****************************************
                    if(YelGreSize == 1)
                    {
                        // RedHave为1，则看到的是黄球
                        if(RedHave == BALL_HAVE)
                        {
                            YellowHave = BALL_HAVE;
                            GreenHave = BALL_NOBALL;
                            ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                        }
                        // RedHave为0，则看到的是绿球
                        else
                        {
                            GreenHave = BALL_HAVE;
                            YellowHave = BALL_NOBALL;
                            ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                        }
                    }
                    //*****************************************
                    //        轮廓数大于1时，判断黄、绿球
                    //*****************************************
                    else
                    {
                        DeleteContoursByRank(YelGreBallsContours, 2);

                        YellowHave = BALL_HAVE;
                        GreenHave = BALL_HAVE;

                        ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                        ContourFitRotatedRect(YelGreBallsContours[1], GreenRect);

                        if(YellowRect.center.x > GreenRect.center.x)
                            swap(YellowRect, GreenRect);
                    }

                    // 计算Yellow, Green尺寸，并判断距离等级
                    float YellowLength, GreenLength;
                    YellowLength = (YellowRect.size.height > YellowRect.size.width)
                            ? YellowRect.size.height : YellowRect.size.width;
                    GreenLength = (GreenRect.size.height > GreenRect.size.width)
                            ? GreenRect.size.height : GreenRect.size.width;


                    if(YellowLength >= 0 && YellowLength < BALL_NEAR_LIMIT)
                        YellowState = BALL_FAR;
                    else if(YellowLength >= BALL_NEAR_LIMIT && YellowLength < BALL_ATTACH_LIMIT)
                        YellowState = BALL_NEAR;
                    else
                        YellowState = BALL_ATTACH;

                    if(GreenLength >= 0 && GreenLength < BALL_NEAR_LIMIT)
                        GreenState = BALL_FAR;
                    else if(GreenLength >= BALL_NEAR_LIMIT && GreenLength < BALL_ATTACH_LIMIT)
                        GreenState = BALL_NEAR;
                    else
                        GreenState = BALL_ATTACH;
                }


                // 若此时寻找绿球，则应该重新对红球进行一次判断
                if(ColorNum == BALL_GREEN)
                {
                    //*****************************************
                    //             再判断红球
                    //*****************************************

                    // 更新红球轮廓向量个数
                    RedSize = RedBallContours.size();

                    // 先将黄、绿球的分量剔除
                    if(YellowHave != BALL_NOBALL || GreenHave != BALL_NOBALL)
                    {
                        Rect tmpYellowROI, tmpGreenROI;

                        if(YellowHave == BALL_NOBALL)
                            tmpYellowROI = Rect(-1, -1, -1, -1);
                        else
                        {
                            tmpYellowROI = YellowRect.boundingRect();

                            // 更新红球轮廓向量个数
                            RedSize = RedBallContours.size();

                            for(int i = RedSize - 1; i >= 0; --i)
                            {
                                RotatedRect tmpRedRect;
                                ContourFitRotatedRect(RedBallContours[i], tmpRedRect);

                                if(tmpRedRect.center.x > tmpYellowROI.x
                                        && tmpRedRect.center.x < (tmpYellowROI.x + tmpYellowROI.width)
                                        && tmpRedRect.center.y > tmpYellowROI.y
                                        && tmpRedRect.center.y < (tmpYellowROI.y + tmpYellowROI.height))
                                    RedBallContours.erase(RedBallContours.begin() + i);
                            }
                        }

                        if(GreenHave == BALL_NOBALL)
                            tmpGreenROI = Rect(-1, -1, -1, -1);
                        else
                        {
                            tmpGreenROI = GreenRect.boundingRect();

                            // 更新红球轮廓向量个数
                            RedSize = RedBallContours.size();

                            for(int i = RedSize - 1; i >= 0; --i)
                            {
                                RotatedRect tmpRedRect;
                                ContourFitRotatedRect(RedBallContours[i], tmpRedRect);

                                if(tmpRedRect.center.x > tmpGreenROI.x
                                        && tmpRedRect.center.x < (tmpGreenROI.x + tmpYellowROI.width)
                                        && tmpRedRect.center.y > tmpGreenROI.y
                                        && tmpRedRect.center.y < (tmpGreenROI.y + tmpYellowROI.height))
                                    RedBallContours.erase(RedBallContours.begin() + i);
                            }
                        }
                    }

                    DeleteSmallContours(YelGreBallsContours, (BALL_NOBALL_LIMIT + BALL_NEAR_LIMIT) / 2);

                    // 更新红球轮廓向量个数
                    RedSize = RedBallContours.size();

                    if(RedSize <= 0)
                        RedHave = BALL_NOBALL;
                    else
                    {
                        RedHave = BALL_HAVE;

                        // 取最左的轮廓为红球轮廓
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                        // 记录最大面积的RedRect
                        RotatedRect ReadyRedRect;
                        ReadyRedRect = RedRect;

                        for(int i = 0; i < RedSize; i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRect);
                            if(RedRect.center.x > tmpRect.center.x)
                                swap(RedRect, tmpRect);
                        }

                        // 若最左轮廓的面积小于最大面积的一半，则两者交换
                        if(RedRect.size.area() < ReadyRedRect.size.area() / 2)
                            swap(RedRect, ReadyRedRect);

                        // 判断红球轮廓大小
                        float RedRectLength;
                        RedRectLength = (RedRect.size.height > RedRect.size.width)
                                ? RedRect.size.height : RedRect.size.width;

    //                    cout<<"Red Rect Length: "<<RedRectLength<<endl;

                        // 根据红球轮廓尺寸大小，判断红球距离状态；
                        if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                            RedState = BALL_FAR;
                        else if(RedRectLength >= BALL_NEAR_LIMIT
                                && RedRectLength < BALL_ATTACH_LIMIT)
                            RedState = BALL_NEAR;
                        else
                            RedState = BALL_ATTACH;
                    }
                }

                int HaveNum = RedHave + YellowHave + GreenHave;

                if(HaveNum <= 0)
                    ballstate = BALL_FAR;
                else
                {
                    int AttachNum = 0;
                    if(RedState == BALL_ATTACH)     AttachNum++;
                    if(YellowState == BALL_ATTACH)     AttachNum++;
                    if(GreenState == BALL_ATTACH)     AttachNum++;

                    int FarNum = 0;
                    if(RedState == BALL_FAR && RedHave == BALL_HAVE)    FarNum++;
                    if(YellowState == BALL_FAR && YellowHave == BALL_HAVE)    FarNum++;
                    if(GreenState == BALL_FAR && GreenHave == BALL_HAVE)    FarNum++;

                    if(AttachNum >= 1)
                        ballstate = BALL_ATTACH;
                    else if(FarNum >= 3)
                        ballstate = BALL_FAR;
                    else
                        ballstate = BALL_NEAR;
                }
            }

            break;
        }
        //************************
        //  球在接近距离、已碰撞状态
        //************************
        case BALL_ATTACH:
        case BALL_ATTACHED_RED:
        case BALL_ATTACHED_GREEN:
        {
            Mat imgRed, imgYelGre;
            Mat imgtmp1, imgtmp2;
            ImgPro.getimgBinRedBall(imgRed);
            ImgPro.getimgBinYelGreBalls(imgYelGre);

            //*****************************************
            //    检查是否存在Mat格式近状态球的二值图
            //*****************************************

            if(imgRed.empty() || imgYelGre.empty())
            {
                cout<<"Analyse Balls Error: Have No Binary Yellow or Green Mats!"<<endl;
                return ;
            }

            // 按策略给定的颜色处理数据
            switch(ColorNum)
            {
            case BALL_RED:
            {
                // 先将黄、绿球状态置为无球；
                YellowHave = BALL_NOBALL;
                GreenHave = BALL_NOBALL;

                // 寻找轮廓，按面积降序排列
                vector<vector<Point>> RedBallContours;
                imgRed.copyTo(imgtmp1);
                findContours(imgtmp1, RedBallContours,
                             CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                ContoursAreaDownSort(RedBallContours);

                //*****************************************
                //              判断轮廓数量
                //*****************************************

                // 轮廓数量
                int RedSize = RedBallContours.size();

                if(RedSize <= 0)
                    ballstate = BALL_FAR;
                else
                {
                    if(RedSize >= 4)
                        DeleteContoursByRank(RedBallContours, 4);

                    // 剔除自然光分量
                    DeleteTopLightVoice(imgRed, RedBallContours, 5);

                    // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                    DeleteSmallContours(RedBallContours, BALL_NOBALL_LIMIT);

                    //*****************************************
                    //             判断红球
                    //*****************************************

                    // 更新红球轮廓向量个数
                    RedSize = RedBallContours.size();

                    if(RedSize <= 0)
                        RedHave = BALL_NOBALL;
                    else
                    {
                        RedHave = BALL_HAVE;

                        // 取最左的轮廓为红球轮廓
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                        // 记录最大面积的RedRect
                        RotatedRect ReadyRedRect;
                        ReadyRedRect = RedRect;

                        for(int i = 0; i < RedSize; i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRect);
                            if(RedRect.center.x > tmpRect.center.x)
                                swap(RedRect, tmpRect);
                        }

                        // 若最左轮廓的面积小于最大面积的一半，则两者交换
                        if(RedRect.size.area() < ReadyRedRect.size.area() / 2)
                            swap(RedRect, ReadyRedRect);

                        // 判断红球轮廓大小
                        float RedRectLength;
                        RedRectLength = (RedRect.size.height > RedRect.size.width)
                                ? RedRect.size.height : RedRect.size.width;

    //                    cout<<"Red Rect Length: "<<RedRectLength<<endl;

                        // 根据红球轮廓尺寸大小，判断红球距离状态；
                        if(RedRectLength >= 0 && RedRectLength < BALL_ATTACH_LIMIT)
                            RedState = BALL_NEAR;
                        else if(RedRectLength >= BALL_ATTACHED_LIMIT)
                            RedState = BALL_ATTACHED_RED;
                        else
                            RedState = BALL_ATTACH;

    //                    cout<<"Red State: "<<RedState<<endl;
                    }
                }
                break;
            }
            case BALL_YELLOW:
            case BALL_GREEN:
            {
                // 先将红球状态置为无球
                RedHave = BALL_NOBALL;

                vector<vector<Point>> YelGreBallsContours;
                imgYelGre.copyTo(imgtmp2);
                findContours(imgtmp2, YelGreBallsContours,
                             CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                ContoursAreaDownSort(YelGreBallsContours);

                //*****************************************
                //              判断轮廓数量
                //*****************************************

                // 轮廓数量
                int YelGreSize = YelGreBallsContours.size();

                if(YelGreSize <= 0)
                    ballstate = BALL_FAR;
                else
                {
                    if(YelGreSize >= 4)
                        DeleteContoursByRank(YelGreBallsContours, 4);

                    // 剔除自然光分量
                    DeleteTopLightVoice(imgYelGre, YelGreBallsContours, 5);

                    // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                    DeleteSmallContours(YelGreBallsContours, BALL_NOBALL_LIMIT);

                    // 更新黄绿球轮廓向量个数
                    YelGreSize = YelGreBallsContours.size();

                    if(YelGreSize <= 0)
                    {
                        YellowHave = BALL_NOBALL;
                        GreenHave = BALL_NOBALL;
                    }
                    else
                    {
                        //*****************************************
                        //        轮廓数为1时，分别判断黄、绿球
                        //*****************************************
                        if(YelGreSize == 1)
                        {
                            // RedHave为1，则看到的是黄球
                            if(RedHave == BALL_HAVE)
                            {
                                YellowHave = BALL_HAVE;
                                GreenHave = BALL_NOBALL;
                                ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                            }
                            // RedHave为0，则看到的是绿球
                            else
                            {
                                GreenHave = BALL_HAVE;
                                YellowHave = BALL_NOBALL;
                                ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                            }
                        }
                        //*****************************************
                        //        轮廓数大于1时，只找绿球
                        //*****************************************
                        else
                        {
                            DeleteContoursByRank(YelGreBallsContours, 2);

                            YellowHave = BALL_NOBALL;
                            GreenHave = BALL_HAVE;

                            ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                        }

                        // 计算Yellow, Green尺寸，并判断距离等级
                        float GreenLength;
                        GreenLength = (GreenRect.size.height > GreenRect.size.width)
                                ? GreenRect.size.height : GreenRect.size.width;

                        if(GreenLength >= 0 && GreenLength < BALL_ATTACH_LIMIT)
                            GreenState = BALL_NEAR;
                        else if(GreenLength >= BALL_ATTACHED_LIMIT)
                            GreenState = BALL_ATTACHED_GREEN;
                        else
                            GreenState = BALL_ATTACH;
                    }
                }
                break;
            }
            default:
                cout<<"Analyse Attach Balls Error: Wrong Color Number!"<<endl;
                break;
            }

            int AttachNum = 0;
            if(RedState == BALL_ATTACH)     AttachNum++;
            if(YellowState == BALL_ATTACH)     AttachNum++;
            if(GreenState == BALL_ATTACH)     AttachNum++;

            int AttachedNum = 0;
            if(RedState == BALL_ATTACHED_RED)     AttachedNum++;
            if(YellowState == BALL_ATTACHED_YELLOW)     AttachedNum++;
            if(GreenState == BALL_ATTACHED_GREEN)     AttachedNum++;

            if(AttachedNum >= 1 && RedHave == BALL_HAVE)
                ballstate = BALL_ATTACHED_RED;
            else if(AttachedNum >= 1 && GreenHave == BALL_HAVE)
                ballstate = BALL_ATTACHED_GREEN;
            else if(AttachNum >= 1)
                ballstate = BALL_ATTACH;
            else
                ballstate = BALL_NEAR;

//            cout<<"Ball State: "<<ballstate<<endl;
//            cout<<"Have Red: "<<RedHave<<endl;
//            cout<<"Have Yellow: "<<YellowHave<<endl;
//            cout<<"Have Green: "<<GreenHave<<endl;

//            if(RedHave != 0)
//                cout<<"Red Rect Center: ("<<RedRect.center.x<<", "<<RedRect.center.y<<")"<<endl;
//            if(YellowHave != 0)
//                cout<<"Yellow Rect Center: ("<<YellowRect.center.x<<", "<<YellowRect.center.y<<")"<<endl;
//            if(GreenHave != 0)
//                cout<<"Green Rect Center: ("<<GreenRect.center.x<<", "<<GreenRect.center.y<<")"<<endl;

//            cout<<endl;
            break;

        }
        default:
            ballstate = BALL_FAR;
            cout<<"Balls State has Changed as BALL_FAR."<<endl;
        }

        //*****************************************
        //            缓存更新
        //*****************************************

        // 处理ballstate相关缓存向量；
        UpdateTmpBallsState(ballstate);

        // LineID状态与LineID拟合矩形信息赋值给VInfo；
        ProcessTmpBallState(ballstate);

        //*****************************************
        //       将球的相关值赋给VInfo
        //*****************************************

        VInfo.BallColor = ColorNum;
        VInfo.ballsstate = ballstate;
        VInfo.RedHave = RedHave;
        VInfo.YellowHave = YellowHave;
        VInfo.GreenHave = GreenHave;

        if(RedHave == BALL_NOBALL)
        {
            VInfo.RedRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.RedOffsetX = 0;
            VInfo.RedOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.RedRect = RedRect;
            VInfo.RedOffsetX = RedRect.center.x - RectSize.width / 2 - 1;
            VInfo.RedOffsetY = RedRect.center.y - RectSize.height / 2 - 1;
        }

        if(YellowHave == BALL_NOBALL)
        {
            VInfo.YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.YellowOffsetX = 0;
            VInfo.YellowOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.YellowRect = YellowRect;
            VInfo.YellowOffsetX = YellowRect.center.x - RectSize.width / 2 - 1;
            VInfo.YellowOffsetY = YellowRect.center.y - RectSize.height / 2 - 1;
        }

        if(GreenHave == BALL_NOBALL)
        {
            VInfo.GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.GreenOffsetX = 0;
            VInfo.GreenOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.GreenRect = GreenRect;
            VInfo.GreenOffsetX = GreenRect.center.x - RectSize.width / 2 - 1;
            VInfo.GreenOffsetY = GreenRect.center.y - RectSize.height / 2 - 1;
        }
        break;
    }
    case 6:
    {
        // 该时刻球距离状态，作为缓存
        int ballstate;
        ballstate = VInfo.ballsstate;
        // 视野中红黄绿球存在状况
        int RedHave = 0;
        int YellowHave = 0;
        int GreenHave = 0;
        // 视野中红黄绿球距离状况
        int RedState = 0;
        int YellowState = 0;
        int GreenState = 0;
        // 视野中红黄绿球的位置（对应的Have状态不为0时起作用）
        RotatedRect RedRect;
        RotatedRect YellowRect;
        RotatedRect GreenRect;

        switch(ballstate)
        {
        case BALL_FAR:
        case BALL_NEAR:
        {

            Mat imgRed, imgYelGre;
            Mat imgtmp1, imgtmp2;
            ImgPro.getimgBinRedBall(imgRed);
            ImgPro.getimgBinYelGreBalls(imgYelGre);

            //*****************************************
            //    检查是否存在Mat格式近状态球的二值图
            //*****************************************

            if(imgRed.empty() || imgYelGre.empty())
            {
                cout<<"Analyse Balls Error: Have No Binary Yellow or Green Mats!"<<endl;
                return ;
            }


            // 寻找轮廓，按面积降序排列
            vector<vector<Point>> RedBallContours;
            vector<vector<Point>> YelGreBallsContours;
            imgRed.copyTo(imgtmp1);
            imgYelGre.copyTo(imgtmp2);
            findContours(imgtmp1, RedBallContours,
                         CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
            findContours(imgtmp2, YelGreBallsContours,
                         CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
            ContoursAreaDownSort(RedBallContours);
            ContoursAreaDownSort(YelGreBallsContours);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int RedSize = RedBallContours.size();
            int YelGreSize = YelGreBallsContours.size();

            if(RedSize + YelGreSize <= 0)
                ballstate = BALL_FAR;
            else
            {
                // 剔除自然光分量
                DeleteTopLightVoice(imgRed, RedBallContours, 8);
                DeleteTopLightVoice(imgYelGre, YelGreBallsContours, 6);

                // 剔除底部光分量
                DeleteBottomLightVoice(imgRed, RedBallContours, 6);
                DeleteBottomLightVoice(imgYelGre, YelGreBallsContours, 6);

                // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                DeleteSmallContours(RedBallContours, BALL_NOBALL_LIMIT);
                DeleteSmallContours(YelGreBallsContours, BALL_NOBALL_LIMIT);

                RedSize = RedBallContours.size();
                YelGreSize = YelGreBallsContours.size();

                if(RedSize >= 3)
                    DeleteContoursByRank(RedBallContours, 3);
                if(YelGreSize >= 5)
                    DeleteContoursByRank(YelGreBallsContours, 5);


                //*****************************************
                //             先判断红球
                //*****************************************

                // 更新红球轮廓向量个数
                RedSize = RedBallContours.size();

                if(RedSize <= 0)
                    RedHave = BALL_NOBALL;
                else
                {
                    RedHave = BALL_HAVE;

                    //*****************************************
                    //             获取红球轮廓
                    //*****************************************

                    // 红球轮廓数量为1，则该轮廓为红球轮廓
                    if(RedSize == 1)
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                    // 红球轮廓数量大于等于2，则判断最大两个红球轮廓中的偏下轮廓为红球轮廓（滤除镜面反射）
                    else
                    {
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                        for(int i = 0; i < RedSize; i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRect);

                            if(RedRect.center.y < tmpRect.center.y)
                                RedRect = tmpRect;
                        }
                    }

                    // 判断红球轮廓大小
                    float RedRectLength;
                    RedRectLength = (RedRect.size.height > RedRect.size.width)
                            ? RedRect.size.height : RedRect.size.width;

                    // 根据红球轮廓尺寸大小，判断红球距离状态；
                    if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                        RedState = BALL_FAR;
                    else if(RedRectLength >= BALL_NEAR_LIMIT
                            && RedRectLength < BALL_ATTACH_LIMIT)
                        RedState = BALL_NEAR;
                    else
                        RedState = BALL_ATTACH;
                }
            }

            //*****************************************
            //             判断黄、绿球
            //*****************************************

            // 更新黄绿球轮廓向量个数
            YelGreSize = YelGreBallsContours.size();

            // 先将红球位置的分量剔除
            if(RedHave != BALL_NOBALL)
            {
                Rect tmpRedROI;

                tmpRedROI = RedRect.boundingRect();
                for(int i = YelGreSize - 1; i >= 0; --i)
                {
                    RotatedRect tmpYelGreRect;
                    ContourFitRotatedRect(YelGreBallsContours[i], tmpYelGreRect);
                    if(tmpYelGreRect.center.x > tmpRedROI.x
                            && tmpYelGreRect.center.x < (tmpRedROI.x + tmpRedROI.width)
                            && tmpYelGreRect.center.y > tmpRedROI.y
                            && tmpYelGreRect.center.y < (tmpRedROI.y + tmpRedROI.height))
                        YelGreBallsContours.erase(YelGreBallsContours.begin() + i);
                }
            }

            DeleteSmallContours(YelGreBallsContours, (BALL_NOBALL_LIMIT + BALL_NEAR_LIMIT) / 4);

            // 更新黄绿球轮廓向量个数
            YelGreSize = YelGreBallsContours.size();

            if(YelGreSize <= 0)
            {
                YellowHave = BALL_NOBALL;
                GreenHave = BALL_NOBALL;
            }
            else
            {
                //*****************************************
                //        轮廓数为1时，分别判断黄、绿球
                //*****************************************
                if(YelGreSize == 1)
                {
                    // RedHave为1，则看到的是黄球
                    if(RedHave == BALL_HAVE)
                    {
                        YellowHave = BALL_HAVE;
                        GreenHave = BALL_NOBALL;
                        ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                        GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }
                    // RedHave为0，则看到的是绿球
                    else
                    {
                        GreenHave = BALL_HAVE;
                        YellowHave = BALL_NOBALL;
                        ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                        YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }
                }
                //*****************************************
                //        轮廓数大于1时，判断黄、绿球
                //*****************************************
                else if(YelGreSize == 2)
                {
//                    DeleteContoursByRank(YelGreBallsContours, 2);


                    ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                    ContourFitRotatedRect(YelGreBallsContours[1], GreenRect);

                    //*****************************************
                    //       判断当前是否为镜面反射情况
                    //*****************************************

                    // 计算两个RotatedRect尺寸
                    float YellowLength, GreenLength;
                    YellowLength = (YellowRect.size.height > YellowRect.size.width)
                            ? YellowRect.size.height : YellowRect.size.width;
                    GreenLength = (GreenRect.size.height > GreenRect.size.width)
                            ? GreenRect.size.height : GreenRect.size.width;

                    float length = (YellowLength + GreenLength) / 2;

                    float deltaCenterX;\
                    deltaCenterX = fabs(YellowRect.center.x - GreenRect.center.x);

                    // 此时两Rect的X坐标基本相同，此时为镜面反射情况
                    if(deltaCenterX < length)
                    {
                        // RedHave为1，则看到的是黄球
                        if(RedHave == BALL_HAVE)
                        {
                            YellowHave = BALL_HAVE;
                            GreenHave = BALL_NOBALL;

                            YellowRect = (YellowRect.center.y > GreenRect.center.y)
                                    ? YellowRect : GreenRect;
                            GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                        // RedHave为0，则看到的是绿球
                        else
                        {
                            GreenHave = BALL_HAVE;
                            YellowHave = BALL_NOBALL;
//                            ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                            GreenRect = (GreenRect.center.y > YellowRect.center.y)
                                    ? GreenRect : YellowRect;
                            YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                    }
                    // 此时两Rect的X坐标差别较大，此时不是镜面反射情况
                    else
                    {
                        YellowHave = BALL_HAVE;
                        GreenHave = BALL_HAVE;

                        if(YellowRect.center.x > GreenRect.center.x)
                            swap(YellowRect, GreenRect);
                    }
                }
                else
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(YelGreBallsContours[0], tmpRect);
                    float length = (tmpRect.size.height > tmpRect.size.width)
                            ? tmpRect.size.height : tmpRect.size.width;
                    length *= 2;

                    vector<vector<Point>> tmpContours;
                    tmpContours = YelGreBallsContours;

                    // 轮廓中心Y，从下向上排序
                    ContoursUpWardDownSort(tmpContours);
                    ContourFitRotatedRect(tmpContours[0], tmpRect);

                    for(int i = YelGreSize - 1; i >= 0; --i)
                    {
                        RotatedRect tmp_tmpRect;
                        ContourFitRotatedRect(tmpContours[i], tmp_tmpRect);

                        float deltaCenterY = fabs(tmp_tmpRect.center.y - tmpRect.center.y);
                        if(deltaCenterY > length)
                            tmpContours.erase(tmpContours.begin() + i);
                    }

                    int tmpYelGreSize = tmpContours.size();

                    if(tmpYelGreSize > 2)
                        DeleteContoursByRank(tmpContours, 2);

                    tmpYelGreSize = tmpContours.size();

                    switch(tmpYelGreSize)
                    {
                    case 0:
                    {
                        YellowHave = BALL_NOBALL;
                        GreenHave = BALL_NOBALL;
                        YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        break;
                    }
                    case 1:
                    {
                        // RedHave为1，则看到的是黄球
                        if(RedHave == BALL_HAVE)
                        {
                            YellowHave = BALL_HAVE;
                            GreenHave = BALL_NOBALL;
                            ContourFitRotatedRect(tmpContours[0], YellowRect);
                            GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                        // RedHave为0，则看到的是绿球
                        else
                        {
                            GreenHave = BALL_HAVE;
                            YellowHave = BALL_NOBALL;
                            ContourFitRotatedRect(tmpContours[0], GreenRect);
                            YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                        break;
                    }
                    case 2:
                    {
                        YellowHave = BALL_HAVE;
                        GreenHave = BALL_HAVE;
                        ContourFitRotatedRect(tmpContours[0], YellowRect);
                        ContourFitRotatedRect(tmpContours[1], GreenRect);

                        if(YellowRect.center.x > GreenRect.center.x)
                            swap(YellowRect, GreenRect);
                        break;
                    }
                    default:
                        cout<<"Analyse Balls Error: tmp Yellow & Green Balls' Number Wrong!"<<endl;
                        break;
                    }
                }

                //*****************************************
                //     判断处理后的黄、绿球是否存在包含关系
                //*****************************************

                if(YellowHave == BALL_HAVE && GreenHave == BALL_HAVE)
                {
                    Rect YellowROI, GreenROI;
                    YellowROI = YellowRect.boundingRect();
                    GreenROI = GreenRect.boundingRect();

                    // 黄球面积大于绿球面积，判断GreenRect是否在YellowRect内
                    if(YellowRect.size.area() > GreenRect.size.area())
                    {
                        if(GreenRect.center.x >= YellowROI.x
                                && GreenRect.center.x <= (YellowROI.x + YellowROI.width)
                                && GreenRect.center.y >= YellowROI.y
                                && GreenRect.center.y <= (YellowROI.y + YellowROI.height))
                        {
                            GreenHave = BALL_NOBALL;
                            GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                    }

                    // 绿球面积大于黄球面积，判断YellowRect是否在GreenRect内
                    if(GreenRect.size.area() > YellowRect.size.area())
                    {
                        if(YellowRect.center.x >= GreenROI.x
                                && YellowRect.center.x <= (GreenROI.x + GreenROI.width)
                                && YellowRect.center.y >= GreenROI.y
                                && YellowRect.center.y <= (GreenROI.y + GreenROI.height))
                        {
                            YellowHave = BALL_NOBALL;
                            YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                    }
                }

                // 计算Yellow, Green尺寸，并判断距离等级
                float YellowLength, GreenLength;
                YellowLength = (YellowRect.size.height > YellowRect.size.width)
                        ? YellowRect.size.height : YellowRect.size.width;
                GreenLength = (GreenRect.size.height > GreenRect.size.width)
                        ? GreenRect.size.height : GreenRect.size.width;


//                if(YellowLength >= 0 && YellowLength < BALL_NEAR_LIMIT)
                if(YellowLength < BALL_NEAR_LIMIT)
                    YellowState = BALL_FAR;
                else if(YellowLength >= BALL_NEAR_LIMIT && YellowLength < BALL_ATTACH_LIMIT)
                    YellowState = BALL_NEAR;
                else
                    YellowState = BALL_ATTACH;

//                if(GreenLength >= 0 && GreenLength < BALL_NEAR_LIMIT)
                if(GreenLength < BALL_NEAR_LIMIT)
                    GreenState = BALL_FAR;
                else if(GreenLength >= BALL_NEAR_LIMIT && GreenLength < BALL_ATTACH_LIMIT)
                    GreenState = BALL_NEAR;
                else
                    GreenState = BALL_ATTACH;
            }

            // 若此时寻找绿球，则应该重新对红球进行一次判断
            if(ColorNum == BALL_GREEN)
            {
                //*****************************************
                //             再判断红球
                //*****************************************

                // 更新红球轮廓向量个数
                RedSize = RedBallContours.size();

                // 先将黄、绿球的分量剔除
                if(YellowHave != BALL_NOBALL || GreenHave != BALL_NOBALL)
                {
                    Rect tmpYellowROI, tmpGreenROI;

                    if(YellowHave == BALL_NOBALL)
                        tmpYellowROI = Rect(-1, -1, -1, -1);
                    else
                    {
                        tmpYellowROI = YellowRect.boundingRect();

                        // 更新红球轮廓向量个数
                        RedSize = RedBallContours.size();

                        for(int i = RedSize - 1; i >= 0; --i)
                        {
                            RotatedRect tmpRedRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRedRect);

                            if(tmpRedRect.center.x > tmpYellowROI.x
                                    && tmpRedRect.center.x < (tmpYellowROI.x + tmpYellowROI.width)
                                    && tmpRedRect.center.y > tmpYellowROI.y
                                    && tmpRedRect.center.y < (tmpYellowROI.y + tmpYellowROI.height))
                                RedBallContours.erase(RedBallContours.begin() + i);
                        }
                    }

                    if(GreenHave == BALL_NOBALL)
                        tmpGreenROI = Rect(-1, -1, -1, -1);
                    else
                    {
                        tmpGreenROI = GreenRect.boundingRect();

                        // 更新红球轮廓向量个数
                        RedSize = RedBallContours.size();

                        for(int i = RedSize - 1; i >= 0; --i)
                        {
                            RotatedRect tmpRedRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRedRect);

                            if(tmpRedRect.center.x > tmpGreenROI.x
                                    && tmpRedRect.center.x < (tmpGreenROI.x + tmpYellowROI.width)
                                    && tmpRedRect.center.y > tmpGreenROI.y
                                    && tmpRedRect.center.y < (tmpGreenROI.y + tmpYellowROI.height))
                                RedBallContours.erase(RedBallContours.begin() + i);
                        }
                    }
                }

//                DeleteSmallContours(YelGreBallsContours, (BALL_NOBALL_LIMIT + BALL_NEAR_LIMIT) / 2);

                DeleteSmallContours(YelGreBallsContours, BALL_NOBALL_LIMIT);

                // 更新红球轮廓向量个数
                RedSize = RedBallContours.size();

                if(RedSize <= 0)
                    RedHave = BALL_NOBALL;
                else
                {
                    RedHave = BALL_HAVE;

                    //*****************************************
                    //             获取红球轮廓
                    //*****************************************

                    // 红球轮廓数量为1，则该轮廓为红球轮廓
                    if(RedSize == 1)
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                    // 红球轮廓数量大于等于2，则判断最大两个红球轮廓中的偏下轮廓为红球轮廓（滤除镜面反射）
                    else
                    {
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                        for(int i = 0; i < RedSize; i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRect);

                            if(RedRect.center.y < tmpRect.center.y)
                                RedRect = tmpRect;
                        }
                    }

                    // 判断红球轮廓大小
                    float RedRectLength;
                    RedRectLength = (RedRect.size.height > RedRect.size.width)
                            ? RedRect.size.height : RedRect.size.width;

    //                    cout<<"Red Rect Length: "<<RedRectLength<<endl;

                    // 根据红球轮廓尺寸大小，判断红球距离状态；
//                    if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                    if(RedRectLength < BALL_NEAR_LIMIT)
                        RedState = BALL_FAR;
                    else if(RedRectLength >= BALL_NEAR_LIMIT
                            && RedRectLength < BALL_ATTACH_LIMIT)
                        RedState = BALL_NEAR;
                    else
                        RedState = BALL_ATTACH;
                }
            }

            int HaveNum = RedHave + YellowHave + GreenHave;

            if(HaveNum <= 0)
                ballstate = BALL_FAR;
            else
            {
                int AttachNum = 0;
                if(RedState == BALL_ATTACH && RedHave == BALL_HAVE)     AttachNum++;
                if(YellowState == BALL_ATTACH && YellowHave == BALL_HAVE)   AttachNum++;
                if(GreenState == BALL_ATTACH && GreenHave == BALL_HAVE)     AttachNum++;

                int NearNum = 0;
                if(RedState == BALL_NEAR && RedHave == BALL_HAVE)   NearNum++;
                if(YellowState == BALL_NEAR && YellowHave == BALL_HAVE)     NearNum++;
                if(GreenState == BALL_NEAR && GreenHave == BALL_HAVE)   NearNum++;

                if(AttachNum >= 1)
                    ballstate = BALL_ATTACH;
                else if(NearNum >= 1)
                    ballstate = BALL_NEAR;
                else
                    ballstate = BALL_FAR;
            }


//            Mat img;
//            img.create(imgYelGre.size(), CV_8UC3);
//            drawContours(img, YelGreBallsContours, -1, Scalar(255), 2);
//            imshow("Contours", img);

            break;
        }
        case BALL_ATTACH:
        case BALL_ATTACHED_RED:
        case BALL_ATTACHED_GREEN:
        {
            Mat imgRed, imgYelGre;
            Mat imgtmp1, imgtmp2;
            ImgPro.getimgBinRedBall(imgRed);
            ImgPro.getimgBinYelGreBalls(imgYelGre);

            //*****************************************
            //    检查是否存在Mat格式近状态球的二值图
            //*****************************************

            if(imgRed.empty() || imgYelGre.empty())
            {
                cout<<"Analyse Balls Error: Have No Binary Yellow or Green Mats!"<<endl;
                return ;
            }

            // 按策略给定的颜色处理数据
            switch(ColorNum)
            {
            case BALL_RED:
            {
                // 先将黄、绿球状态置为无球；
                YellowHave = BALL_NOBALL;
                GreenHave = BALL_NOBALL;

                // 寻找轮廓，按面积降序排列
                vector<vector<Point>> RedBallContours;
                imgRed.copyTo(imgtmp1);
                findContours(imgtmp1, RedBallContours,
                             CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
                ContoursAreaDownSort(RedBallContours);

                //*****************************************
                //              判断轮廓数量
                //*****************************************

                // 轮廓数量
                int RedSize = RedBallContours.size();

                if(RedSize <= 0)
                    ballstate = BALL_FAR;
                else
                {
                    if(RedSize >= 4)
                        DeleteContoursByRank(RedBallContours, 4);

                    // 剔除自然光分量
                    DeleteTopLightVoice(imgRed, RedBallContours, 5);

                    // 剔除底部光分量
                    DeleteBottomLightVoice(imgRed, RedBallContours, 6);

                    // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                    DeleteSmallContours(RedBallContours, BALL_NOBALL_LIMIT);

                    //*****************************************
                    //             判断红球
                    //*****************************************

                    // 更新红球轮廓向量个数
                    RedSize = RedBallContours.size();

                    if(RedSize <= 0)
                        RedHave = BALL_NOBALL;
                    else
                    {
                        RedHave = BALL_HAVE;

                        // 取最左的轮廓为红球轮廓
                        ContourFitRotatedRect(RedBallContours[0], RedRect);
                        // 记录最大面积的RedRect
                        RotatedRect ReadyRedRect;
                        ReadyRedRect = RedRect;

                        for(int i = 0; i < RedSize; i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedBallContours[i], tmpRect);
                            if(RedRect.center.x > tmpRect.center.x)
                                swap(RedRect, tmpRect);
                        }

                        // 若最左轮廓的面积小于最大面积的一半，则两者交换
                        if(RedRect.size.area() < ReadyRedRect.size.area() / 2)
                            swap(RedRect, ReadyRedRect);

                        // 判断红球轮廓大小
                        float RedRectLength;
                        RedRectLength = (RedRect.size.height > RedRect.size.width)
                                ? RedRect.size.height : RedRect.size.width;

    //                    cout<<"Red Rect Length: "<<RedRectLength<<endl;

                        // 根据红球轮廓尺寸大小，判断红球距离状态；
                        if(RedRectLength >= 0 && RedRectLength < BALL_ATTACH_LIMIT)
                            RedState = BALL_NEAR;
                        else if(RedRectLength >= BALL_ATTACHED_LIMIT)
                            RedState = BALL_ATTACHED_RED;
                        else
                            RedState = BALL_ATTACH;

    //                    cout<<"Red State: "<<RedState<<endl;
                    }
                }
                break;
            }
            case BALL_YELLOW:
            case BALL_GREEN:
            {
                // 先将红球状态置为无球
                RedHave = BALL_NOBALL;

                vector<vector<Point>> YelGreBallsContours;
                imgYelGre.copyTo(imgtmp2);
                findContours(imgtmp2, YelGreBallsContours,
                             CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                ContoursAreaDownSort(YelGreBallsContours);

                //*****************************************
                //              判断轮廓数量
                //*****************************************

                // 轮廓数量
                int YelGreSize = YelGreBallsContours.size();

                if(YelGreSize <= 0)
                    ballstate = BALL_FAR;
                else
                {
                    if(YelGreSize >= 4)
                        DeleteContoursByRank(YelGreBallsContours, 4);

                    // 剔除自然光分量
                    DeleteTopLightVoice(imgYelGre, YelGreBallsContours, 5);

                    // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                    DeleteSmallContours(YelGreBallsContours, BALL_NOBALL_LIMIT);

                    // 更新黄绿球轮廓向量个数
                    YelGreSize = YelGreBallsContours.size();

                    if(YelGreSize <= 0)
                    {
                        YellowHave = BALL_NOBALL;
                        GreenHave = BALL_NOBALL;
                    }
                    else
                    {
                        //*****************************************
                        //        轮廓数为1时，分别判断黄、绿球
                        //*****************************************
                        if(YelGreSize == 1)
                        {
                            // RedHave为1，则看到的是黄球
                            if(RedHave == BALL_HAVE)
                            {
                                YellowHave = BALL_HAVE;
                                GreenHave = BALL_NOBALL;
                                ContourFitRotatedRect(YelGreBallsContours[0], YellowRect);
                            }
                            // RedHave为0，则看到的是绿球
                            else
                            {
                                GreenHave = BALL_HAVE;
                                YellowHave = BALL_NOBALL;
                                ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                            }
                        }
                        //*****************************************
                        //        轮廓数大于1时，只找绿球
                        //*****************************************
                        else
                        {
                            DeleteContoursByRank(YelGreBallsContours, 2);

                            YellowHave = BALL_NOBALL;
                            GreenHave = BALL_HAVE;

                            ContourFitRotatedRect(YelGreBallsContours[0], GreenRect);
                        }

                        // 计算Yellow, Green尺寸，并判断距离等级
                        float GreenLength;
                        GreenLength = (GreenRect.size.height > GreenRect.size.width)
                                ? GreenRect.size.height : GreenRect.size.width;

//                        if(GreenLength >= 0 && GreenLength < BALL_ATTACH_LIMIT)
                        if(GreenLength < BALL_ATTACH_LIMIT)
                            GreenState = BALL_NEAR;
                        else if(GreenLength >= BALL_ATTACHED_LIMIT)
                            GreenState = BALL_ATTACHED_GREEN;
                        else
                            GreenState = BALL_ATTACH;
                    }
                }
                break;
            }
            default:
                cout<<"Analyse Attach Balls Error: Wrong Color Number!"<<endl;
                break;
            }

            int AttachNum = 0;
            if(RedState == BALL_ATTACH)     AttachNum++;
            if(YellowState == BALL_ATTACH)     AttachNum++;
            if(GreenState == BALL_ATTACH)     AttachNum++;

            int AttachedNum = 0;
            if(RedState == BALL_ATTACHED_RED)     AttachedNum++;
            if(YellowState == BALL_ATTACHED_YELLOW)     AttachedNum++;
            if(GreenState == BALL_ATTACHED_GREEN)     AttachedNum++;

            if(AttachedNum >= 1 && RedHave == BALL_HAVE)
                ballstate = BALL_ATTACHED_RED;
            else if(AttachedNum >= 1 && GreenHave == BALL_HAVE)
                ballstate = BALL_ATTACHED_GREEN;
            else if(AttachNum >= 1)
                ballstate = BALL_ATTACH;
            else
                ballstate = BALL_NEAR;

            break;
        }
        default:
            ballstate = BALL_FAR;
            cout<<"Balls State has Changed as BALL_FAR."<<endl;
        }

        //*****************************************
        //            缓存更新
        //*****************************************

        // 处理ballstate相关缓存向量；
        UpdateTmpBallsState(ballstate);

        // LineID状态与LineID拟合矩形信息赋值给VInfo；
        ProcessTmpBallState(ballstate);

        //*****************************************
        //       将球的相关值赋给VInfo
        //*****************************************

        VInfo.BallColor = ColorNum;
        VInfo.ballsstate = ballstate;
        VInfo.RedHave = RedHave;
        VInfo.YellowHave = YellowHave;
        VInfo.GreenHave = GreenHave;

        if(RedHave == BALL_NOBALL)
        {
            VInfo.RedRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.RedOffsetX = 0;
            VInfo.RedOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.RedRect = RedRect;
            VInfo.RedOffsetX = RedRect.center.x - RectSize.width / 2 - 1;
            VInfo.RedOffsetY = RedRect.center.y - RectSize.height / 2 - 1;
        }

        if(YellowHave == BALL_NOBALL)
        {
            VInfo.YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.YellowOffsetX = 0;
            VInfo.YellowOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.YellowRect = YellowRect;
            VInfo.YellowOffsetX = YellowRect.center.x - RectSize.width / 2 - 1;
            VInfo.YellowOffsetY = YellowRect.center.y - RectSize.height / 2 - 1;
        }

        if(GreenHave == BALL_NOBALL)
        {
            VInfo.GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.GreenOffsetX = 0;
            VInfo.GreenOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.GreenRect = GreenRect;
            VInfo.GreenOffsetX = GreenRect.center.x - RectSize.width / 2 - 1;
            VInfo.GreenOffsetY = GreenRect.center.y - RectSize.height / 2 - 1;
        }

        break;
    }
    case 7:
    {
        // 该时刻球距离状态，作为缓存
        int ballstate;
        ballstate = VInfo.ballsstate;
        // 视野中红黄绿球存在状况
        int RedHave = 0;
        int YellowHave = 0;
        int GreenHave = 0;
        // 视野中红黄绿球距离状况
        int RedState = 0;
        int YellowState = 0;
        int GreenState = 0;
        // 视野中红黄绿球的位置（对应的Have状态不为0时起作用）
        RotatedRect RedRect;
        RotatedRect YellowRect;
        RotatedRect GreenRect;

        Mat imgRed, imgYellow, imgGreen;
        Mat imgtmp1, imgtmp2, imgtmp3;

        ImgPro.getimgBinRedBall(imgRed);
        ImgPro.getimgBinYellowBall(imgYellow);
        ImgPro.getimgBinGreenBall(imgGreen);

        //*****************************************
        //    检查是否存在Mat格式近状态球的二值图
        //*****************************************

        if(imgRed.empty() || imgYellow.empty() || imgGreen.empty())
        {
            cout<<"Analyse Balls Error: Have No Binary Yellow or Green Mats!"<<endl;
            return ;
        }

        // 寻找轮廓，按面积降序排列
        vector<vector<Point>> RedContours;
        vector<vector<Point>> YellowContours;
        vector<vector<Point>> GreenContours;
        imgRed.copyTo(imgtmp1);
        imgYellow.copyTo(imgtmp2);
        imgGreen.copyTo(imgtmp3);

        findContours(imgtmp1, RedContours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        findContours(imgtmp2, YellowContours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        findContours(imgtmp3, GreenContours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        ContoursAreaDownSort(RedContours);
        ContoursAreaDownSort(YellowContours);
        ContoursAreaDownSort(GreenContours);

        //*****************************************
        //              判断轮廓数量
        //*****************************************

        // 轮廓数量
        if(RedContours.size() + YellowContours.size() + GreenContours.size() <= 0)
            ballstate = BALL_FAR;
        else
        {
            // 剔除红色导引路径
            for(int i = RedContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(RedContours[i], tmpRect);
                double ratio = tmpRect.size.height / tmpRect.size.width;
                if(ratio < 1)   ratio = 1/ ratio;
                if(ratio > 3)
                    RedContours.erase(RedContours.begin() + i);
            }

            // 剔除自然光分量
            DeleteTopLightVoice(imgRed, RedContours, 8);
            DeleteTopLightVoice(imgYellow, YellowContours, 8);
            DeleteTopLightVoice(imgGreen, GreenContours, 8);

            // 剔除底部光分量
            DeleteBottomLightVoice(imgRed, RedContours, 8);
            DeleteBottomLightVoice(imgYellow, YellowContours, 8);
            DeleteBottomLightVoice(imgGreen, GreenContours, 8);


            // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
            DeleteSmallContours(RedContours, BALL_NOBALL_LIMIT);
            DeleteSmallContours(YellowContours, BALL_NOBALL_LIMIT);
            DeleteSmallContours(GreenContours, BALL_NOBALL_LIMIT);

            if(RedContours.size() >= 3)
                DeleteContoursByRank(RedContours, 3);
            if(YellowContours.size() >= 3)
                DeleteContoursByRank(YellowContours, 3);
            if(GreenContours.size() >= 3)
                DeleteContoursByRank(GreenContours, 3);

            //****************************************
            //             判断红球
            //*****************************************

            // 剔除长宽比过大的较长轮廓（剔除红色导引路径影响）
            for(int i = RedContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(RedContours[i], tmpRect);
                double ratio = tmpRect.size.height / tmpRect.size.width;
                double length = tmpRect.size.height > tmpRect.size.width
                        ? tmpRect.size.height : tmpRect.size.width;
                if(ratio < 1)   ratio = 1 / ratio;
                if(ratio > 4 && length > imgtmp1.size().height / 8)
                    RedContours.erase(RedContours.begin() + i);

            }

            if(RedContours.size() <= 0)
                RedHave = BALL_NOBALL;
            else
            {
                RedHave = BALL_HAVE;

                //*****************************************
                //             获取红球轮廓
                //*****************************************

                // 红球轮廓数量为1，则该轮廓为红球轮廓
                if(RedContours.size() == 1)
                    ContourFitRotatedRect(RedContours[0], RedRect);
                // 红球轮廓数量大于等于2，则判断最大两个红球轮廓中的偏下轮廓为红球轮廓（滤除镜面反射）
                else
                {
                    ContourFitRotatedRect(RedContours[0], RedRect);
                    for(int i = 0; i < RedContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(RedContours[i], tmpRect);

                        if(RedRect.center.y < tmpRect.center.y)
                            RedRect = tmpRect;
                    }
                }

                // 判断红球轮廓大小
                float RedRectLength;
                RedRectLength = (RedRect.size.height > RedRect.size.width)
                        ? RedRect.size.height : RedRect.size.width;

                // 根据红球轮廓尺寸大小，判断红球距离状态；
                if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                    RedState = BALL_FAR;
                else if(RedRectLength >= BALL_NEAR_LIMIT
                        && RedRectLength < BALL_ATTACH_LIMIT)
                    RedState = BALL_NEAR;
                else if(RedRectLength >= BALL_ATTACH_LIMIT
                        && RedRectLength < BALL_ATTACHED_LIMIT)
                    RedState = BALL_ATTACH;
                else
                    RedState = BALL_ATTACHED_RED;
            }

            //****************************************
            //             判断黄球
            //*****************************************

            if(YellowContours.size() <= 0)
                YellowHave = BALL_NOBALL;
            else
            {
                YellowHave = BALL_HAVE;

                //*****************************************
                //             获取黄球轮廓
                //*****************************************

                // 黄球轮廓数量为1，则该轮廓为黄球轮廓
                if(YellowContours.size() == 1)
                    ContourFitRotatedRect(YellowContours[0], YellowRect);
                // 黄球轮廓数量大于等于2，则判断最大两个黄球轮廓中的偏下轮廓为黄球轮廓（滤除镜面反射）
                else
                {
                    ContourFitRotatedRect(YellowContours[0], YellowRect);
                    for(int i = 0; i < YellowContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(YellowContours[i], tmpRect);

                        if(YellowRect.center.y < tmpRect.center.y)
                            YellowRect = tmpRect;
                    }
                }

                // 判断黄球轮廓大小
                float YellowRectLength;
                YellowRectLength = (YellowRect.size.height > YellowRect.size.width)
                        ? YellowRect.size.height : YellowRect.size.width;

                // 根据红球轮廓尺寸大小，判断红球距离状态；
                if(YellowRectLength >= 0 && YellowRectLength < BALL_NEAR_LIMIT)
                    YellowState = BALL_FAR;
                else if(YellowRectLength >= BALL_NEAR_LIMIT
                        && YellowRectLength < BALL_ATTACH_LIMIT)
                    YellowState = BALL_NEAR;
                else if(YellowRectLength >= BALL_ATTACH_LIMIT
                        && YellowRectLength < BALL_ATTACHED_LIMIT)
                    YellowState = BALL_ATTACH;
                else
                    YellowState = BALL_ATTACHED_YELLOW;
            }

            //****************************************
            //             判断绿球
            //*****************************************

            if(GreenContours.size() <= 0)
                GreenHave = BALL_NOBALL;
            else
            {
                GreenHave = BALL_HAVE;

                //*****************************************
                //             获取绿球轮廓
                //*****************************************

                // 绿球轮廓数量为1，则该轮廓为绿球轮廓
                if(GreenContours.size() == 1)
                    ContourFitRotatedRect(GreenContours[0], GreenRect);
                // 绿球轮廓数量大于等于2，则判断最大两个绿球轮廓中的偏下轮廓为绿球轮廓（滤除镜面反射）
                else
                {
                    ContourFitRotatedRect(GreenContours[0], GreenRect);
                    for(int i = 0; i < GreenContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(GreenContours[i], tmpRect);

                        if(RedRect.center.y < tmpRect.center.y)
                            GreenRect = tmpRect;
                    }
                }

                // 判断绿球轮廓大小
                float GreenRectLength;
                GreenRectLength = (GreenRect.size.height > GreenRect.size.width)
                        ? GreenRect.size.height : GreenRect.size.width;

                // 根据绿球轮廓尺寸大小，判断绿球距离状态；
                if(GreenRectLength >= 0 && GreenRectLength < BALL_NEAR_LIMIT)
                    GreenState = BALL_FAR;
                else if(GreenRectLength >= BALL_NEAR_LIMIT
                        && GreenRectLength < BALL_ATTACH_LIMIT)
                    GreenState = BALL_NEAR;
                else if(GreenRectLength >= BALL_ATTACH_LIMIT
                        && GreenRectLength < BALL_ATTACHED_LIMIT)
                    GreenState = BALL_ATTACH;
                else
                    GreenState = BALL_ATTACHED_GREEN;
            }

            //**************************************
            //        黄球绿球轮廓过近时的分析
            //**************************************

            if(YellowHave == BALL_HAVE && GreenHave == BALL_HAVE)
            {
                double YellowLength, GreenLength;
                // 求黄球绿球长度
                YellowLength = YellowRect.size.height > YellowRect.size.width
                        ? YellowRect.size.height : YellowRect.size.width;
                GreenLength = GreenRect.size.height > GreenRect.size.width
                        ? GreenRect.size.height : GreenRect.size.width;

                // 如果两者X方向距离过近，则进入判断
                if(fabs(YellowRect.center.x - GreenRect.center.x)
                        < (YellowLength + GreenLength) * 2)
                {
                    // 在红球存在的情况下，如果黄球绿球X方向太过接近，则剔除绿球
                    if(RedHave == BALL_HAVE)
                    {
                        GreenHave = BALL_NOBALL;
                        GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }
                    else
                    {
                        double YelGreAreaRatio;
                        YelGreAreaRatio = YellowRect.size.area() / GreenRect.size.area();

                        // 若黄球远大于绿球，则绿球视为杂波剔除
                        if(YelGreAreaRatio > 3)
                        {
                            GreenHave = BALL_NOBALL;
                            GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                        // 若绿球远大于黄球，则黄球视为杂波剔除
                        else if(YelGreAreaRatio < 1/3)
                        {
                            YellowHave = BALL_NOBALL;
                            GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                        }
                        // 若两球面积比介于两者之间，则认为两者面积大小相似，则暂不处理
                    }
                }
                // 如果两球相距较远，则如果判别到的黄球在绿球右边，则交换两球的外接矩形
                else
                {
                    if(YellowRect.center.x > GreenRect.center.x)
                        swap(YellowRect, GreenRect);
                }
            }


            // 若此时寻找绿球，则应该重新对红球进行一次判断
            if(ColorNum == BALL_GREEN)
            {
                //*****************************************
                //             再判断红球
                //*****************************************

                // 先将黄、绿球的分量剔除
                if(YellowHave != BALL_NOBALL || GreenHave != BALL_NOBALL)
                {
                    Rect tmpYellowROI, tmpGreenROI;

                    // 如果黄球存在，则从黄球分量中剔除红色杂波
                    if(YellowHave == BALL_NOBALL)
                        tmpYellowROI = Rect(-1, -1, -1, -1);
                    else
                    {
                        tmpYellowROI = YellowRect.boundingRect();

                        for(int i = RedContours.size() - 1; i >= 0; --i)
                        {
                            RotatedRect tmpRedRect;
                            ContourFitRotatedRect(RedContours[i], tmpRedRect);

                            if(tmpRedRect.center.x > tmpYellowROI.x
                                    && tmpRedRect.center.x < (tmpYellowROI.x + tmpYellowROI.width)
                                    && tmpRedRect.center.y > tmpYellowROI.y
                                    && tmpRedRect.center.y < (tmpYellowROI.y + tmpYellowROI.height))
                                RedContours.erase(RedContours.begin() + i);
                        }
                    }

                    // 如果绿球存在，则从绿球分量中剔除红色杂波
                    if(GreenHave == BALL_NOBALL)
                        tmpGreenROI = Rect(-1, -1, -1, -1);
                    else
                    {
                        tmpGreenROI = GreenRect.boundingRect();

                        for(int i = RedContours.size() - 1; i >= 0; --i)
                        {
                            RotatedRect tmpRedRect;
                            ContourFitRotatedRect(RedContours[i], tmpRedRect);

                            if(tmpRedRect.center.x > tmpGreenROI.x
                                    && tmpRedRect.center.x < (tmpGreenROI.x + tmpGreenROI.width)
                                    && tmpRedRect.center.y > tmpGreenROI.y
                                    && tmpRedRect.center.y < (tmpGreenROI.y + tmpGreenROI.height))
                                RedContours.erase(RedContours.begin() + i);
                        }
                    }
                }

                // 重新判断红球状态
                if(RedContours.size() <= 0)
                    RedHave = BALL_NOBALL;
                else
                {
                    RedHave = BALL_HAVE;

                    //*****************************************
                    //             获取红球轮廓
                    //*****************************************

                    // 红球轮廓数量为1，则该轮廓为红球轮廓
                    if(RedContours.size() == 1)
                        ContourFitRotatedRect(RedContours[0], RedRect);
                    // 红球轮廓数量大于等于2，则判断最大两个红球轮廓中的偏下轮廓为红球轮廓（滤除镜面反射）
                    else
                    {
                        ContourFitRotatedRect(RedContours[0], RedRect);
                        for(int i = 0; i < RedContours.size(); i++)
                        {
                            RotatedRect tmpRect;
                            ContourFitRotatedRect(RedContours[i], tmpRect);

                            if(RedRect.center.y < tmpRect.center.y)
                                RedRect = tmpRect;
                        }
                    }

                    // 判断红球轮廓大小
                    float RedRectLength;
                    RedRectLength = (RedRect.size.height > RedRect.size.width)
                            ? RedRect.size.height : RedRect.size.width;

                    // 根据红球轮廓尺寸大小，判断红球距离状态；
                    if(RedRectLength >= 0 && RedRectLength < BALL_NEAR_LIMIT)
                        RedState = BALL_FAR;
                    else if(RedRectLength >= BALL_NEAR_LIMIT
                            && RedRectLength < BALL_ATTACH_LIMIT)
                        RedState = BALL_NEAR;
                    else
                        RedState = BALL_ATTACH;
                }

                //*****************************************
                //             再判断绿球
                //*****************************************

                // 识别黄球时，绿球识别度比较低
                // 然而识别绿球时，黄球识别度几乎和绿球一样高

                if(YellowHave == BALL_HAVE && GreenHave == BALL_HAVE)
                {
                    Rect tmpYellowROI, tmpGreenROI;
                    tmpYellowROI = YellowRect.boundingRect();
                    tmpGreenROI = GreenRect.boundingRect();

                    // 如果黄球在绿球ROI范围内，则黄球被滤除，保留绿球
                    if(YellowRect.center.x > tmpGreenROI.x
                            && YellowRect.center.x < tmpGreenROI.x + tmpGreenROI.width
                            && YellowRect.center.y > tmpGreenROI.y
                            && YellowRect.center.y < tmpGreenROI.y + tmpGreenROI.height)
                    {
                        YellowHave = BALL_NOBALL;
                        YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }
                }

            }

            int HaveNum = RedHave + YellowHave + GreenHave;

            if(HaveNum <= 0)
                ballstate = BALL_FAR;
            else
            {
                int AttachNum = 0;
                if(RedState == BALL_ATTACH && RedHave == BALL_HAVE)     AttachNum++;
                if(YellowState == BALL_ATTACH && YellowHave == BALL_HAVE)   AttachNum++;
                if(GreenState == BALL_ATTACH && GreenHave == BALL_HAVE)     AttachNum++;

                int NearNum = 0;
                if(RedState == BALL_NEAR && RedHave == BALL_HAVE)   NearNum++;
                if(YellowState == BALL_NEAR && YellowHave == BALL_HAVE)     NearNum++;
                if(GreenState == BALL_NEAR && GreenHave == BALL_HAVE)   NearNum++;

                if(AttachNum >= 1)
                    ballstate = BALL_ATTACH;
                else if(NearNum >= 1)
                    ballstate = BALL_NEAR;
                else
                    ballstate = BALL_FAR;
            }

        }


        //*****************************************
        //            缓存更新
        //*****************************************

        // 处理ballstate相关缓存向量；
        UpdateTmpBallsState(ballstate);

        // LineID状态与LineID拟合矩形信息赋值给VInfo；
        ProcessTmpBallState(ballstate);

        //*****************************************
        //       将球的相关值赋给VInfo
        //*****************************************

        VInfo.BallColor = ColorNum;
        VInfo.ballsstate = ballstate;
        VInfo.RedHave = RedHave;
        VInfo.YellowHave = YellowHave;
        VInfo.GreenHave = GreenHave;

        if(RedHave == BALL_NOBALL)
        {
            VInfo.RedRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.RedOffsetX = 0;
            VInfo.RedOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.RedRect = RedRect;
            VInfo.RedOffsetX = RedRect.center.x - RectSize.width / 2 - 1;
            VInfo.RedOffsetY = RedRect.center.y - RectSize.height / 2 - 1;
        }

        if(YellowHave == BALL_NOBALL)
        {
            VInfo.YellowRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.YellowOffsetX = 0;
            VInfo.YellowOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.YellowRect = YellowRect;
            VInfo.YellowOffsetX = YellowRect.center.x - RectSize.width / 2 - 1;
            VInfo.YellowOffsetY = YellowRect.center.y - RectSize.height / 2 - 1;
        }

        if(GreenHave == BALL_NOBALL)
        {
            VInfo.GreenRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            VInfo.GreenOffsetX = 0;
            VInfo.GreenOffsetY = 0;
        }
        else
        {
            Mat tmp;
            ImgPro.getDownFramePrePro(tmp);
            Size2f RectSize = Size2f(tmp.size().width, tmp.size().height);

            VInfo.GreenRect = GreenRect;
            VInfo.GreenOffsetX = GreenRect.center.x - RectSize.width / 2 - 1;
            VInfo.GreenOffsetY = GreenRect.center.y - RectSize.height / 2 - 1;
        }

        break;
    }
    default:
        cout<<"Wrong Analyse Balls Number!"<<endl;
        break;
    }
}

// 分析二值图，获得风格过门信息；
void NCVision::AnalyseStyleDoor(int polesNum, int num)
{
    switch(num)
    {
    case 6:
    {
        switch(polesNum)
        {
        // 调试阶段，只有两个竖直方向的短杆
        case 2:
        {
            // 该时刻风格过门距离状态，作为缓存
            int stylestate;
            stylestate = VInfo.StyleDoorState;

            RotatedRect StyleDoorRect;
            Mat imgStyleDoor, imgtmp;
            ImgPro.getimgBinStyleDoor(imgStyleDoor);

            //*****************************************
            //    检查是否存在Mat格式远状态球的二值图
            //*****************************************

            if(imgStyleDoor.empty())
            {
                cout<<"Analyse Style Door Error: Have No Binary Style Door Mat!"<<endl;
                return ;
            }

            // 寻找轮廓，按轮廓长度降序排列
            vector<vector<Point>> StyleDoorContours;
            imgStyleDoor.copyTo(imgtmp);
            findContours(imgtmp, StyleDoorContours,
                         CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            ContoursLengthDownSort(StyleDoorContours);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int StyleDoorSize = StyleDoorContours.size();

            if(StyleDoorSize <= 0)
            {
                stylestate = STYLE_FAR;
            }
            else
            {
                if(StyleDoorSize >= 4)
                    DeleteContoursByRank(StyleDoorContours, 4);

                // 剔除自然光分量
                DeleteTopLightVoice(imgStyleDoor, StyleDoorContours, 5);

                // 剔除自然光分量
                DeleteBottomLightVoice(imgStyleDoor, StyleDoorContours, 5);

                // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                DeleteSmallContours(StyleDoorContours, STYLE_NODOOR_LIMIT);

                //*****************************************
                //           判断最大轮廓
                //*****************************************

                // 更新轮廓数量
                StyleDoorSize = StyleDoorContours.size();

                // 此时若无轮廓，则视为远距离状态；
                if(StyleDoorSize <= 0)
                    stylestate = STYLE_FAR;
                else
                {
                    ContoursLengthDownSort(StyleDoorContours);

                    float MaxLength;
                    ContourRotatedRectLength(StyleDoorContours[0], MaxLength);

                    if(MaxLength >= 0 && MaxLength < STYLE_NEAR_LIMIT)
                        stylestate = STYLE_FAR;
                    else if(MaxLength >= STYLE_NEAR_LIMIT)
                        stylestate = STYLE_NEAR;
                }

                // STYLE_FAR状态，则设StyleDoorRect为缺省值，中心点为(-1, -1)
                if(stylestate == STYLE_FAR)
                    StyleDoorRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                // STYLE_NEAR状态，则StyleDoorRect为图中检测出的轮廓拟合出的矩形；
                else
                {
                    // 更新轮廓数量
                    StyleDoorSize = StyleDoorContours.size();

                    // 此时若无轮廓，则视为远距离状态；
                    if(StyleDoorSize > 2)
                        DeleteContoursByRank(StyleDoorContours, 2);

                    ContoursMinAreaRect(StyleDoorContours, StyleDoorRect);
                }
            }

            //*****************************************
            //            缓存更新
            //*****************************************

            // 处理StyleDoorState相关缓存向量；
            UpdateTmpStyleDoorState(stylestate);
            UpdateTmpStyleDoorRect(stylestate, StyleDoorRect);

            // 风格过门状态与风格过门拟合矩形赋给函数参数
            ProcessTmpStyleDoor(stylestate, StyleDoorRect);

            //*****************************************
            //       计算整个门状态时的相关信息
            //*****************************************

            //====================
            //  风格过门距离状态赋给VInfo
            //====================
            VInfo.StyleDoorState = stylestate;

            //====================
            //风格过门轮廓矩形vector赋给VInfo
            //====================
            int size = StyleDoorContours.size();
            vector<RotatedRect> StyleDoorVecRects;
            for(int i = 0; i < size; i++)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(StyleDoorContours[i], tmpRect);
                StyleDoorVecRects.push_back(tmpRect);
            }
            VInfo.StyleDoorVecRects = StyleDoorVecRects;

            //====================
            //风格过门拟合矩形赋给VInfo
            //====================
            VInfo.StyleDoorRect = StyleDoorRect;

            //===========================
            // 风格过门拟合矩形的X, Y方向偏移量计算：
            //===========================
            if(stylestate == STYLE_NEAR)
            {
                Point2f StyleRectCenter = StyleDoorRect.center;
                int ScreenCenterX = imgtmp.size().width / 2 - 1;
                int ScreenCenterY = imgtmp.size().height / 2 - 1;

                VInfo.StyleCenterOffsetX = StyleRectCenter.x - ScreenCenterX;
                VInfo.StyleCenterOffsetY = StyleRectCenter.y - ScreenCenterY;
            }
            else
            {
                // 其他状态，则偏移值回归一个定值（此处暂定为0）
                VInfo.StyleCenterOffsetX = 0;
                VInfo.StyleCenterOffsetY = 0;
            }

//            cout<<"Style Door State: "<<stylestate<<endl<<endl;

//            Point2f stylepoints[4];
//            StyleDoorRect.points(stylepoints);

//            cout<<"Style Door Points:"<<endl;
//            for(int i = 0; i < 4; i++)
//            {
//                cout<<"("<<stylepoints[i].x<<", "<<stylepoints[i].y<<") ";
//            }
//            cout<<endl;

            break;
        }
        // 实际比赛阶段，三个杆
        case 3:
        {
            // 门状态
            int stylestate;
            //
            vector<RotatedRect> PolesRects;
            // 门总轮廓
            RotatedRect StyleRect;
            Mat imgBinStyleDoor, imgtmp;
            ImgPro.getimgBinStyleDoor(imgBinStyleDoor);

            //*****************************************
            //    检查是否存在Mat格式第一个门的二值图
            //*****************************************

            if(imgBinStyleDoor.size().width <= 0 || imgBinStyleDoor.size().height <= 0)
            {
                cout<<"Analyse Style Door Error: Have No Binary Style Door Mat!"<<endl;
                return ;
            }

            // 寻找轮廓，按面积降序排列
            vector<vector<Point>> StyleContours;
            imgBinStyleDoor.copyTo(imgtmp);
            findContours(imgtmp, StyleContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

            ContoursAreaDownSort(StyleContours);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int StyleSize = StyleContours.size();

            if(StyleSize <= 0)
                stylestate = STYLE_NODOOR;
            else
            {
                //*****************************************
                //        滤除杂波，判断是否存在门；
                //*****************************************

                // 求最大长度
                RotatedRect MaxContour;
                ContourFitRotatedRect(StyleContours[0], MaxContour);

                float len, wid;
                len = MaxContour.size.height;
                wid = MaxContour.size.width;
                if(len <= wid)
                    std::swap(len, wid);
                float contourMaxlen = len;

                // 若最大长度仍小于某值，则视为所有轮廓都为杂波，此时为无门状态
                if(contourMaxlen <= DOOR_CONTOURMAXLIMIT)
                    stylestate = STYLE_NODOOR;
                else
                {
                    // 轮廓数>=5，删除剩5个
                    // （只需要3个，剩5个是为了留余量）
                    if(StyleSize > 5)
                        DeleteContoursByRank(StyleContours, 5);


                    // 更新此时的轮廓数量；
                    StyleSize = StyleContours.size();

                    // 再次检查轮廓数量
                    if(StyleSize >= 3)
                        DeleteContoursByRank(StyleContours, 3);

                    //********
                    //  暂定
                    //********
                    // 遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                    DeleteSmallContours(StyleContours, STYLE_MAXFILTER_LIMIT);


                    //*****************************************
                    //        根据不同的轮廓数量，判断相应情况
                    //*****************************************

                    // 更新此时的轮廓数量；
                    StyleSize = StyleContours.size();

                    switch(StyleSize)
                    {
                    case 0:
                    {
                        stylestate = STYLE_NODOOR;
                        break;
                    }
                    case 1:
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(StyleContours[0], tmpRect);

                        float length;
                        length = tmpRect.size.width > tmpRect.size.height
                                ?  tmpRect.size.width : tmpRect.size.height;

                        double Angle;
                        Angle = getRcDegree(tmpRect);
//                        cout<<"Angle: "<<Angle<<endl;

                        if(length >= (imgBinStyleDoor.size().width / 4)
                                && Angle < 20 || Angle > 160)
                            stylestate = STYLE_BOTTOMONLY;
                        else
                            stylestate = STYLE_NODOOR;

//                        for(int i = 0; i < 1; i++)
//                        {
//                            RotatedRect tmpRect;
//                            ContourFitRotatedRect(StyleContours[i], tmpRect);

//                            double Angle;
//                            Angle = getRcDegree(tmpRect);

//                            cout<<"Rect "<<i<<" Angle: "<<Angle<<endl;
//                        }
                        break;
                    }
                    case 2:
                    {
                        RotatedRect UpRect, DownRect;
                        ContourFitRotatedRect(StyleContours[0], UpRect);
                        ContourFitRotatedRect(StyleContours[1], DownRect);

                        // 比较上下轮廓，并作位置调节
                        if(UpRect.center.y > DownRect.center.y)
                            swap(UpRect, DownRect);

                        double UpAngle, DownAngle;
                        UpAngle = getRcDegree(UpRect);
                        DownAngle = getRcDegree(DownRect);
//                        cout<<"UpRect Angle: "<<UpAngle<<endl;
//                        cout<<"DownRect Angle: "<<DownAngle<<endl;

                        // 首先判断是否为两侧的状态；
                        double AbsAngleSub;
                        AbsAngleSub = fabs(UpAngle - DownAngle);

                        if(AbsAngleSub < 20)
                        {
                            stylestate = STYLE_WHOLEDOOR;
                            PolesRects.push_back(UpRect);
                            PolesRects.push_back(DownRect);
                        }
                        else
                        {
                            if(AbsAngleSub > 60 && AbsAngleSub < 120)
                            {
                                if(UpRect.center.x < DownRect.center.x)
                                    stylestate = STYLE_HALFLEFT;
                                else
                                    stylestate = STYLE_HALFRIGHT;
                            }
                            else
                                stylestate = STYLE_NODOOR;
                        }

                        break;
                    }
                    case 3:
                    {
//                        for(int i = 0; i < 3; i++)
//                        {
//                            RotatedRect tmpRect;
//                            ContourFitRotatedRect(StyleContours[i], tmpRect);

//                            double Angle;
//                            Angle = getRcDegree(tmpRect);

//                            cout<<"Rect "<<i<<" Angle: "<<Angle<<endl;
//                        }



                        RotatedRect BottomRect, SideRect1, SideRect2;
                        ContourFitRotatedRect(StyleContours[0], BottomRect);
                        ContourFitRotatedRect(StyleContours[1], SideRect1);
                        ContourFitRotatedRect(StyleContours[2], SideRect2);

                        // 三个轮廓位置调整；
                        if(BottomRect.center.y < SideRect1.center.y)
                            swap(BottomRect, SideRect1);
                        if(BottomRect.center.y < SideRect2.center.y)
                            swap(BottomRect, SideRect2);
                        if(SideRect1.center.x > SideRect2.center.x)
                            swap(SideRect1, SideRect2);

//                        double BottomAngle;
//                        double SideAngle1, SideAngle2;

//                        BottomAngle = getRcDegree(BottomRect);
//                        SideAngle1 = getRcDegree(SideRect1);
//                        SideAngle2 = getRcDegree(SideRect2);

                        PolesRects.push_back(BottomRect);
                        PolesRects.push_back(SideRect1);
                        PolesRects.push_back(SideRect2);

                        stylestate = STYLE_WHOLEDOOR;

                        break;
                    }
                    default:
                        cout<<"Analyse Style Door Error: Wrong Style Size!"<<endl;
                        break;
                    }

//                    Mat img;
//                    img.create(imgBinStyleDoor.size(), CV_8UC3);
//                    drawContours(img, StyleContours, -1, Scalar(255), 2);
//                    imshow("Contours", img);
                }
            }

            if(stylestate == STYLE_WHOLEDOOR)
                RectsMinAreaRect(PolesRects, StyleRect);
            else
                StyleRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);

            //*****************************************
            //            缓存更新
            //*****************************************

            UpdateTmpStyleDoorSeenState(stylestate);
            UpdateTmpStyleDoorSeenRect(stylestate, StyleRect);

            ProcessTmpStyleSeenDoor(stylestate, StyleRect);

            //*****************************************
            //       计算整个门状态时的相关信息
            //*****************************************

            //====================
            //  风格过门状态赋给VInfo
            //====================
            VInfo.StyleDoorSeenState = stylestate;

            //====================
            //风格过门轮廓矩形vector赋给VInfo
            //====================
            int size = StyleContours.size();
            vector<RotatedRect> StyleDoorVecRects;
            for(int i = 0; i < size; i++)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(StyleContours[i], tmpRect);
                StyleDoorVecRects.push_back(tmpRect);
            }
            VInfo.StyleDoorVecRects = StyleDoorVecRects;

            //====================
            //风格过门拟合矩形赋给VInfo
            //====================
            VInfo.StyleDoorRect = StyleRect;
            double Angle;
            Angle = getRcDegree(StyleRect);
            cout<<"Angle: "<<Angle<<endl;


            //===========================
            // 风格过门拟合矩形的X, Y方向偏移量计算：
            //===========================
            if(stylestate == STYLE_WHOLEDOOR)
            {
                Point2f StyleRectCenter = StyleRect.center;
                int ScreenCenterX = imgtmp.size().width / 2 - 1;
                int ScreenCenterY = imgtmp.size().height / 2 - 1;

                VInfo.StyleCenterOffsetX = StyleRectCenter.x - ScreenCenterX;
                VInfo.StyleCenterOffsetY = StyleRectCenter.y - ScreenCenterY;
            }
            else
            {
                // 其他状态，则偏移值回归一个定值（此处暂定为0）
                VInfo.StyleCenterOffsetX = 0;
                VInfo.StyleCenterOffsetY = 0;
            }

            break;
        }
        default:
            cout<<"Analyse Style Door Error: Wrong Analyse Style Door's poles Number!"<<endl;
            break;
        }

        break;
    }
    case 7:
    {
        switch(polesNum)
        {
        case 3:
        {
            // 门状态
            int stylestate;
            // 风格过门距离状态
            int StyleDistState;
            // 风格过门角度状态
            int StyleAngleState;

            vector<RotatedRect> PolesRects;
            // 面积前三轮廓相关数据
            RotatedRect FirstRect, SecondRect, ThirdRect;
            double FirstAngle, SecondAngle, ThirdAngle;

            // 门总轮廓
            RotatedRect StyleRect;

            Mat imgBinStyleDoor, imgtmp;
            ImgPro.getimgBinStyleDoor(imgBinStyleDoor);

            //*****************************************
            //    检查是否存在Mat格式第一个门的二值图
            //*****************************************

            if(imgBinStyleDoor.size().width <= 0 || imgBinStyleDoor.size().height <= 0)
            {
                cout<<"Analyse Style Door Error: Have No Binary Style Door Mat!"<<endl;
                return ;
            }

            // 寻找轮廓，按面积降序排列
            vector<vector<Point>> StyleContours;
            imgBinStyleDoor.copyTo(imgtmp);
            findContours(imgtmp, StyleContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

            //*****************************************
            //              判断轮廓数量
            //*****************************************

            // 轮廓数量
            int StyleSize = StyleContours.size();

            if(StyleContours.size() > 0)
            {
                // 面积降序排列，遍历轮廓，计算轮廓长边，小于某值，则剔除该轮廓
                ContoursAreaDownSort(StyleContours);
                DeleteSmallContours(StyleContours, STYLE_SMALLFILTER_LIMIT);
            }

            StyleSize = StyleContours.size();
            if(StyleContours.size() <= 0)
                stylestate = STYLE_NODOOR;
            else
            {
                //*****************************************
                //        滤除杂波，判断是否存在门；
                //*****************************************

                // 求最大长度
//                RotatedRect FirstRect;
                ContourFitRotatedRect(StyleContours[0], FirstRect);

                float len, wid;
                len = FirstRect.size.height;
                wid = FirstRect.size.width;
                if(len <= wid)
                    std::swap(len, wid);
                float contourMaxlen = len;

                // 若最大长度仍小于某值，则视为所有轮廓都为杂波，此时为无门状态
                if(contourMaxlen <= STYLE_CONTOURMAXLIMIT)
                    stylestate = STYLE_NODOOR;
                else
                {
                    // 剔除自然光分量
                    DeleteTopLightVoice(imgBinStyleDoor, StyleContours, 7);

                    // 剔除底部分量
                    DeleteBottomLightVoice(imgBinStyleDoor, StyleContours, 8);

                    StyleSize = StyleContours.size();
                    // 轮廓数>3，删除剩3个
                    if(StyleContours.size() > 3)
                        DeleteContoursByRank(StyleContours, 3);


                    StyleSize = StyleContours.size();

                    // 拟合最小矩形，如果该矩形位于其余外接矩形的内部，则将该最小轮廓剔除s
                    if(StyleSize > 2)
                    {
                        RotatedRect MinRect;

    //                    ContourFitRotatedRect(StyleContours[StyleContours.size() - 2], MinRect);
                        ContourFitRotatedRect(StyleContours[StyleContours.size() - 1], MinRect);

                        for(int i = StyleContours.size() - 2; i >= 0; i--)
                        {
                            Rect tmpROI;
                            ContourFitROIRect(StyleContours[i], tmpROI);
                            if(MinRect.center.x > tmpROI.x
                                    && MinRect.center.x < tmpROI.x + tmpROI.width
                                    && MinRect.center.y > tmpROI.y
                                    && MinRect.center.y < tmpROI.y  + tmpROI.height)
                            {
                                StyleContours.erase(StyleContours.begin() + i);
    //                            StyleContours.erase(StyleContours.size() - 1);
                                break;
                            }
                        }
                    }

                    // 更新此时的轮廓数量；
                    StyleSize = StyleContours.size();

//                    cout<<"Style Size: "<<StyleSize<<endl;

                    switch(StyleContours.size())
                    {
                    case 0:
                    {
                        stylestate = STYLE_NODOOR;
                        break;
                    }
                    case 1:
                    {
                        ContourFitRotatedRect(StyleContours[0], FirstRect);
                        FirstAngle = getRcDegree(FirstRect);

//                        cout<<"First Angle: "<<FirstAngle<<endl;

                        //*****************************************
                        //          L型
                        //*****************************************
                        if(FirstAngle > 30 && FirstAngle < 75)
                        {
                            stylestate = STYLE_HALFRIGHT;
                        }
                        //*****************************************
                        //          」型
                        //*****************************************
                        else if(FirstAngle > 105 && FirstAngle < 150)
                        {
                            stylestate = STYLE_HALFLEFT;
                        }
                        else
                        {
                            float length, width;
                            float imgwidth;
                            length = FirstRect.size.height;
                            width = FirstRect.size.width;
                            imgwidth = imgBinStyleDoor.size().width;

                            if(length < width)
                                swap(length, width);

                            //*****************************************
                            //           一型
                            //*****************************************
                            if(length / width > 3
                                    && length / imgwidth > 2.0/3.0)
                            {
//                                cout<<"Ratio: "<<length/imgwidth<<endl;
                                stylestate = STYLE_BOTTOMONLY;
                            }
                            //*****************************************
                            //          |_|型（连体型）
                            //*****************************************
                            else if(length / width < 2)
                            {
                                stylestate = STYLE_WHOLEDOOR;
                            }
                            else
                            {
                                stylestate = STYLE_OTHER;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        ContourFitRotatedRect(StyleContours[0], FirstRect);
                        ContourFitRotatedRect(StyleContours[1], SecondRect);
                        FirstAngle = getRcDegree(FirstRect);
                        SecondAngle = getRcDegree(SecondRect);

//                        cout<<"First Angle: "<<FirstAngle<<endl;
//                        cout<<"Second Angle: "<<SecondAngle<<endl;

                        if((FirstAngle > 30 && FirstAngle < 75)
                                || (FirstAngle > 105 && FirstAngle < 150))
                        {
                            double ratio;
                            ratio = FirstRect.size.area() / SecondRect.size.area();

                            if(SecondAngle > 75 && SecondAngle < 105)
                                stylestate = STYLE_WHOLEDOOR;
                            else if(FirstAngle > 30 && FirstAngle < 75 && ratio > 100)
                                stylestate = STYLE_HALFRIGHT;
                            else if(FirstAngle > 105 && FirstAngle < 150 && ratio > 100)
                                stylestate = STYLE_HALFLEFT;
                            else
                                stylestate = STYLE_OTHER;
                        }
                        else if(FirstAngle > 75 && FirstAngle < 105)
                        {
                            double ratio = FirstRect.size.area() / SecondRect.size.area();

                            //*****************************************
                            //          L型
                            //*****************************************
                            if(FirstAngle > 30 && FirstAngle < 75 && ratio > 200)
                            {
                                stylestate = STYLE_HALFRIGHT;
                            }
                            //*****************************************
                            //          」型
                            //*****************************************
                            else if(FirstAngle > 105 && FirstAngle < 150 && ratio > 200)
                            {
                                stylestate = STYLE_HALFLEFT;
                            }
                            else
                            {
                                //*****************************************
                                //          L型，且左部分比下部分长
                                //*****************************************
                                if((SecondAngle > 0 && SecondAngle < 20)
                                        || (SecondAngle > 160 && SecondAngle < 180)
                                        && SecondRect.center.y > FirstRect.center.y
                                        && SecondRect.center.x > FirstRect.center.x)
                                {
                                    stylestate = STYLE_HALFLEFT;
                                }
                                //*****************************************
                                //          」型，且右部分比下部分长
                                //*****************************************
                                else if((SecondAngle > 0 && SecondAngle < 20)
                                        || (SecondAngle > 160 && SecondAngle < 180)
                                        && SecondRect.center.y > FirstRect.center.y
                                        && SecondRect.center.x < FirstRect.center.x)
                                {
                                    stylestate = STYLE_HALFRIGHT;
                                }
                                else
                                    stylestate = STYLE_OTHER;
                            }
                        }
                        else
                        {
                            double ratio = FirstRect.size.area() / SecondRect.size.area();
//                            cout<<"Ratio: "<<ratio<<endl;

                            //*****************************************
                            //          L型，且左部分比下部分短
                            //*****************************************
                            if(SecondAngle > 75 && SecondAngle < 105
                                    && SecondRect.center.y < FirstRect.center.y
                                    && SecondRect.center.x < FirstRect.center.x
                                    && ratio < 5)
                            {
                                stylestate = STYLE_HALFLEFT;
                            }
                            //*****************************************
                            //          」型，且右部分比下部分短
                            //*****************************************
                            else if(SecondAngle > 75 && SecondAngle < 105
                                    && SecondRect.center.y < FirstRect.center.y
                                    && SecondRect.center.x > FirstRect.center.x
                                    && ratio < 5)
                            {
                                stylestate = STYLE_HALFRIGHT;
                            }
                            else
                                stylestate = STYLE_WHOLEDOOR;
                        }
                        break;
                    }
                    case 3:
                    {
                        stylestate = STYLE_WHOLEDOOR;
                        break;
                    }
                    default:
                        cout<<"Analyse Style Door Error: Wrong Style Door Rects' Size!"<<endl;
                        break;
                    }
                }
            }

            StyleSize = StyleContours.size();

            // 存储PolesRects向量；
            for(int i = 0; i < StyleSize; i++)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(StyleContours[i], tmpRect);
                PolesRects.push_back(tmpRect);
            }

            // 拟合多个轮廓的最小外接矩形StyleRect
            vector<RotatedRect> tmpRects;
            for(int i = 0; i < StyleSize; i++)
            {
                Rect tmpROI;
                RotatedRect tmpRect;
                ContourFitROIRect(StyleContours[i], tmpROI);
                tmpRect = RotatedRect2Rect(tmpROI);
                tmpRects.push_back(tmpRect);
            }

            RectsMinAreaRect(tmpRects, StyleRect);

            // 根据StyleRect判断距离等级
            double StyleWidth;
            StyleWidth = StyleRect.size.width < StyleRect.size.height
                    ? StyleRect.size.width : StyleRect.size.height;

            if(StyleWidth < STYLE_NEAR_LIMIT)
            {
                StyleDistState = STYLE_FAR;
            }
            else
                StyleDistState = STYLE_NEAR;

            // NEAR状态下，如果StyleState为STYLE_OTHER，则判断最小外界矩形，改为LEFT, RIGHT, WHOLEDOOR状态
            if(StyleDistState == STYLE_NEAR && stylestate == STYLE_OTHER)
            {
                int imgWidth = imgtmp.size().width;
                if(StyleRect.center.x < imgWidth * 2/5)
                    stylestate = STYLE_HALFRIGHT;
                else if(StyleRect.center.x > imgWidth * 3/5)
                    stylestate = STYLE_HALFLEFT;
                else
                    stylestate = STYLE_WHOLEDOOR;
            }


            // 只有形态状态为STYLE_WHOLEDOOR状态下，最后拟合矩形的最小外接矩形才得以保存
            if(stylestate != STYLE_WHOLEDOOR)
                StyleRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);

            // STYLE_WHOLEDOOR状态下，根据底部横杠角度判断与时间门的角度
            if(stylestate == STYLE_WHOLEDOOR)
            {
                if(StyleContours.size() >= 3)
                {
                    RotatedRect BottomRect;
                    ContourFitRotatedRect(StyleContours[0], BottomRect);

                    // 求出底部矩形
                    for(int i = 0 ; i < StyleContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(StyleContours[i], tmpRect);
                        if(tmpRect.center.y > BottomRect.center.y)
                            BottomRect = tmpRect;
                    }

                    double BottomAngle;
                    BottomAngle = getRcDegree(BottomRect);

//                    cout<<"Bottom Angle: "<<BottomAngle<<endl;

                    if(BottomAngle < 12 || BottomAngle > 168)
                        StyleAngleState = STYLE_CENTER;

                    // /型底部轮廓，垂直于风格门右偏
                    else if(BottomAngle >= 12 && BottomAngle < 90)
                        StyleAngleState = STYLE_RIGHT;
                    // \型底部轮廓，垂直于风格门左偏
                    else
                        StyleAngleState = STYLE_LEFT;

                }
                // 如果时间门轮廓没有分开，则不能根据轮廓识别角度
                else
                    StyleAngleState = STYLE_UNKNOWNANGLE;
            }
            else
                StyleAngleState = STYLE_UNKNOWNANGLE;

//            //*****************************************
//            //            缓存更新
//            //*****************************************

//            UpdateTmpStyleDoorSeenState(stylestate);
//            UpdateTmpStyleDoorSeenRect(stylestate, StyleRect);

//            ProcessTmpStyleSeenDoor(stylestate, StyleRect);

            //*****************************************
            //       计算整个门状态时的相关信息
            //*****************************************

            //====================
            //  风格过门距离状态赋给VInfo
            //====================
            VInfo.StyleDoorState = StyleDistState;

            //====================
            //  风格过门状态赋给VInfo
            //====================
            VInfo.StyleDoorSeenState = stylestate;

            //====================
            //  风格过门角度状态赋给VInfo
            //====================
            VInfo.StyleDoorAngleState = StyleAngleState;

            //====================
            //风格过门轮廓矩形vector赋给VInfo
            //====================

            VInfo.StyleDoorVecRects = PolesRects;

            //====================
            //风格过门拟合矩形赋给VInfo
            //====================
            VInfo.StyleDoorRect = StyleRect;

//            cout<<"Style Rect (X, Y): ("<<StyleRect.center.x<<", "<<StyleRect.center.y<<")"<<endl;

            //===========================
            // 风格过门拟合矩形的X, Y方向偏移量计算：
            //===========================
            if(stylestate == STYLE_WHOLEDOOR)
            {
                Point2f StyleRectCenter = StyleRect.center;
                int ScreenCenterX = imgtmp.size().width / 2 - 1;
                int ScreenCenterY = imgtmp.size().height / 2 - 1;

                VInfo.StyleCenterOffsetX = StyleRectCenter.x - ScreenCenterX;
                VInfo.StyleCenterOffsetY = StyleRectCenter.y - ScreenCenterY;
            }
            else
            {
                // 其他状态，则偏移值回归一个定值（此处暂定为0）
                VInfo.StyleCenterOffsetX = 0;
                VInfo.StyleCenterOffsetY = 0;
            }

            break;
        }
        default:
            cout<<"Analyse Style Door Error: Wrong Analyse Style Door's poles Number!"<<endl;
            break;
        }
        break;
    }
    default:
        cout<<"Wrong Analyse Style Door Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：AnalyseTorBoard()
 *
 * 说明：分析鱼雷切板，获得相关信息；
 *  整体步骤：
 *  1、计算最大轮廓、孔轮廓、红色边缘轮廓；
 *  2、距离等级与状态判断；
 *  3、根据不同距离等级与状态，提供不同信息；
 *
 * 参数：
 *  int num:        识别门策略程序撰写月份
/*===================================================================
 */
void NCVision::AnalyseTorBoard(int num)
{
    switch(num)
    {
    case 7:
    {
        /*===================================================*/
        /*===============  计算轮廓部分  ======================*/
        /*===================================================*/

        int TorBoardState;
        // 鱼雷切板轮廓
        vector<vector<Point>> TorBoardContours;
        // 孔洞轮廓
        vector<vector<Point>> HolesContours;
        // 缓存轮廓，存储凹陷部分的轮廓
        vector<vector<Point>> tmpConvex;
        // 红色边缘轮廓
        vector<vector<Point>> RedEdgesContours;
        // 最大孔洞轮廓
        vector<Point> MaxHoleContour;
        // 最大孔洞拟合矩形
        RotatedRect MaxHoleRect;
        // 鱼雷切板拟合矩形
        RotatedRect TorBoardRect;

        // 鱼雷切板拟合矩形中心点
        Point2f TorBoardCenter;
        // 鱼雷切板中心距屏幕X, Y方向偏移量
        int TorBoardOffsetX;
        int TorBoardOffsetY;

        // 最大轮廓向量
        vector<Point> MaxTorBoard;
        //
        vector<Vec4i> hierarchy;

        // 左上，左下，右上，右下孔洞（用于NEAR状态）
        Hole ULHole, DLHole, URHole, DRHole;
        // 大小孔洞（用于ATTACH相关状态）
        Hole SmallHole, BigHole;
        // 当前目标孔洞
        Hole CurrentHole;
        // 是否正对鱼雷切板
        bool IsFacing;


        // 最大轮廓向量的ROI区域
        Rect MaxTorBoardROI;

        Mat imgTorBoard, imgTorRedEdge;
        Mat imgtmp1, imgtmp2;
        Mat imgFwd;
        ImgPro.getimgBinTorBoard(imgTorBoard);
        ImgPro.getimgBinTorRedEdge(imgTorRedEdge);
        ImgPro.getFwdFramePrePro(imgFwd);

        imgTorBoard.copyTo(imgtmp1);
        imgTorRedEdge.copyTo(imgtmp2);

        //****************************************
        //          鱼雷切板与孔洞识别
        //****************************************

        // 寻找轮廓
        findContours(imgtmp1, TorBoardContours, hierarchy,
                     CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        // 判断鱼雷切板轮廓数量，若数量为0，则没有找到鱼雷切板
        if(TorBoardContours.size() <= 0)
        {
            TorBoardState = TORPEDO_NOBOARD;
            MaxTorBoardROI = Rect(-1, -1, -1, -1);
        }
        else
        {
            // 轮廓降序排序
            ContoursAreaDownSort(TorBoardContours);

            MaxTorBoard = TorBoardContours[0];
            ContourFitROIRect(MaxTorBoard, MaxTorBoardROI);

            // 删除剩10个
            if(TorBoardContours.size() > 10)
                DeleteContoursByRank(TorBoardContours, 10);
            // 删除过小轮廓值
            DeleteSmallContours(TorBoardContours, TORPEDO_NODOOR_LIMIT);


            vector<Vec4i> defects;

            //*************************************
            //       剔除在最大轮廓之外的轮廓
            //*************************************
            for(int i = TorBoardContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(TorBoardContours[i], tmpRect);

                if(tmpRect.center.x > MaxTorBoardROI.x
                        && tmpRect.center.x < (MaxTorBoardROI.x + MaxTorBoardROI.width)
                        && tmpRect.center.y > MaxTorBoardROI.y
                        && tmpRect.center.y < (MaxTorBoardROI.y + MaxTorBoardROI.height))
                    ;
                else
                {
                    TorBoardContours.erase(TorBoardContours.begin() + i);
                }
            }

            //*************************************
            //  如果最大轮廓有凹陷，则进行凹陷部分的处理
            //*************************************
            if(!isContourConvex(MaxTorBoard))
            {
                vector<int> hull;
                // 计算凸包
                convexHull(MaxTorBoard, hull, true);

                //*************************************
                //计算凹陷部分，并记录与Vec4i的容器defects中
                //*************************************
                convexityDefects(MaxTorBoard,
                                 Mat(hull),
                                 defects);

                int DefectsSize = defects.size();

                //*************************************
                //          填充凹陷部分轮廓向量
                //*************************************
                for(int i = 0; i < DefectsSize; i++)
                {
                    vector<Point> tmpPoints;
                    // 存储单个凹陷部分轮廓的点
                    for(int j = defects[i][0]; j < defects[i][1]; j++)
                    {
                        tmpPoints.push_back(MaxTorBoard[j]);
                    }
                    // 防止出现空向量存入tmpConvex
                    // (比如两个index的首尾分别为934, 0, 则会出现空向量，导致容器处理出错，程序崩溃)
                    if(defects[i][0] < defects[i][1])
                        // 将该凹陷部分轮廓存入凹陷轮廓向量中
                        tmpConvex.push_back(tmpPoints);
                }

                //*************************************
                //处理孔洞轮廓向量中的小轮廓，并最多取最大的4个轮廓
                //*************************************
                if(tmpConvex.size() > 0)
                {
                    // 凹陷轮廓向量降序排序
                    ContoursAreaDownSort(tmpConvex);

                    if(tmpConvex.size() >= 4)
                        // 删除剩余4个
                        DeleteContoursByRank(tmpConvex, 4);

                    // 删除过小的轮廓
                    DeleteSmallContours(tmpConvex, TORPEDO_NODOOR_LIMIT);

                    // 将处理后的凹陷轮廓计入空洞轮廓向量中，为往后的分析作准备
                    for(int i = 0; i < tmpConvex.size(); i++)
                        HolesContours.push_back(tmpConvex[i]);
                }
            }

            // 重新将鱼雷切板轮廓降序排序
            ContoursAreaDownSort(TorBoardContours);

            if(TorBoardContours.size() >= 5)
                // 删除剩5个
                DeleteContoursByRank(TorBoardContours, 5);

            // 在最大轮廓的ROI范围内寻找面积比较大的轮廓
            for(int i = 1; i < TorBoardContours.size(); i++)
            {
                HolesContours.push_back(TorBoardContours[i]);
            }

            // 剔除孔洞轮廓中比较小的轮廓
            DeleteSmallContours(HolesContours, TORPEDO_HOLESIZE_LIMIT);

            //*************************************
            //      长宽比太高的孔洞轮廓，剔除之
            //*************************************
            for(int i = HolesContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(HolesContours[i], tmpRect);
                double ratio = tmpRect.size.width / tmpRect.size.height;
                if(ratio < 1)   ratio = 1 / ratio;

                if(ratio > 3)
                    HolesContours.erase(HolesContours.begin() + i);
            }

            //*************************************
            //      面积远小于最大孔洞的轮廓，剔除之
            //*************************************

            // 孔洞轮廓容器降序排列，取最大孔洞
            ContoursAreaDownSort(HolesContours);

            if(HolesContours.size() > 0)
            {
                MaxHoleContour = HolesContours[0];
                ContourFitRotatedRect(MaxHoleContour, MaxHoleRect);

                for(int i = HolesContours.size() - 1; i >= 0; i--)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(HolesContours[i], tmpRect);
                    // 遍历孔洞，若该孔洞与最大孔洞面积比值超过某值，则剔除该值
                    if(MaxHoleRect.size.area() / tmpRect.size.area() > TORPEDO_HOLESIZE_RATIO)
                        HolesContours.erase(HolesContours.begin() + i);
                }
            }

            //*************************************
            //     ROI边缘的Hole轮廓，剔除之
            //*************************************
            if(HolesContours.size() > 0)
            {
                for(int i = HolesContours.size() - 1; i >= 0; i--)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(HolesContours[i], tmpRect);

                    if(tmpRect.center.x < MaxTorBoardROI.x + MaxTorBoardROI.width / 8
                            || tmpRect.center.x > MaxTorBoardROI.x + MaxTorBoardROI.width * 7/8
                            || tmpRect.center.y < MaxTorBoardROI.y + MaxTorBoardROI.height / 8
                            || tmpRect.center.y > MaxTorBoardROI.y + MaxTorBoardROI.height *7/8)
                        HolesContours.erase(HolesContours.begin() + i);
                }
            }
        }

        //****************************************
        //          鱼雷切板红色边缘识别
        //****************************************

        // 寻找轮廓
        findContours(imgtmp2, RedEdgesContours,
                     hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        // 红色边缘轮廓数量大于0时，进行处理
        if(RedEdgesContours.size() > 0)
        {
            // 红色边缘轮廓容器降序排序
            ContoursAreaDownSort(RedEdgesContours);
            // 删除剩6个
            DeleteContoursByRank(RedEdgesContours, 6);
            // 剔除过小分量
            DeleteSmallContours(RedEdgesContours, TORPEDO_REDEDGHES_LIMIT);
        }

        /*===================================================*/
        /*=============  距离等级与状态判断  ===================*/
        /*===================================================*/

        // 鱼雷切板最长尺寸
        int MaxTorBoardLength;
        MaxTorBoardLength = MaxTorBoardROI.size().height > MaxTorBoardROI.size().width
                ? MaxTorBoardROI.size().height : MaxTorBoardROI.size().width;

        if(MaxTorBoardROI == Rect(-1, -1, -1, -1))
        {
            TorBoardState = TORPEDO_NOBOARD;
        }
        else
        {
            if(MaxTorBoardLength < TORPEDO_FAR_LIMIT)
                TorBoardState = TORPEDO_NOBOARD;
            else
            {
                if(MaxTorBoardLength < TORPEDO_NEAR_LIMIT)
                    TorBoardState = TORPEDO_FAR;
                else
                {
                    if(MaxTorBoardLength < TORPEDO_ATTACH_LIMIT)
                        TorBoardState = TORPEDO_NEAR;
                    else
                    {
                        // 鱼雷孔洞最长尺寸
                        int MaxHoleLength;
                        MaxHoleLength = MaxHoleRect.size.width > MaxHoleRect.size.height
                                ? MaxHoleRect.size.width : MaxHoleRect.size.height;

                        if(RedEdgesContours.size() <= 0)
                            TorBoardState = TORPEDO_ATTACH_NOHOLE;
                        else if(RedEdgesContours.size() <= 1)
                            TorBoardState = TORPEDO_ATTACH_ONE;
                        else
                        {
                            if(RedEdgesContours.size() >= 3)
                                TorBoardState = TORPEDO_ATTACH_CROSS;
                            else
                            {
//                                int LeftLimit, RightLimit;
//                                int MaxBoardCenterX;
//                                LeftLimit = imgtmp1.size().width * 2 / 5;
//                                RightLimit = imgtmp1.size().width * 3 / 5;
//                                MaxBoardCenterX = MaxTorBoardROI.x + MaxTorBoardROI.width / 2;

//                                if(MaxBoardCenterX < LeftLimit)
//                                    TorBoardState = TORPEDO_ATTACH_LEFT;
//                                else if(MaxBoardCenterX > RightLimit)
//                                    TorBoardState = TORPEDO_ATTACH_RIGHT;
//                                else
//                                    TorBoardState = TORPEDO_ATTACH_TOP;

                                RotatedRect RedRect1, RedRect2;
                                ContourFitRotatedRect(RedEdgesContours[0], RedRect1);
                                ContourFitRotatedRect(RedEdgesContours[1], RedRect2);

                                if(fabs(RedRect1.center.y - RedRect2.center.y) < 100)
                                    TorBoardState = TORPEDO_ATTACH_TOP;
                                else
                                {
                                    if(RedRect1.center.x > imgtmp1.size().width * 1/2
                                            && RedRect2.center.x > imgtmp1.size().width * 1/2)
                                        TorBoardState = TORPEDO_ATTACH_LEFT;
                                    else
                                        TorBoardState = TORPEDO_ATTACH_RIGHT;
                                }
                            }
                        }
                    }
                }
            }
        }

        /*===================================================*/
        /*================ 不同状态信息分析 ===================*/
        /*===================================================*/

        switch(TorBoardState)
        {
        case TORPEDO_NOBOARD:
        {
            break;
        }
        case TORPEDO_FAR:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            Size ImgSize;
            ImgSize = imgtmp1.size();
            TorBoardCenter = TorBoardRect.center;
            TorBoardOffsetX = TorBoardCenter.x - ImgSize.width / 2;
            TorBoardOffsetY = TorBoardCenter.y - ImgSize.height / 2;

            break;
        }
        case TORPEDO_NEAR:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            Size ImgSize;
            ImgSize = imgtmp1.size();
            TorBoardCenter = TorBoardRect.center;
            TorBoardOffsetX = TorBoardCenter.x - ImgSize.width / 2;
            TorBoardOffsetY = TorBoardCenter.y - ImgSize.height / 2;

            // 计算鱼雷切板外接矩形长宽比
            double TorBoardSizeRatio;
            TorBoardSizeRatio = TorBoardRect.size.width / TorBoardRect.size.height;
            if(TorBoardSizeRatio < 1)
                TorBoardSizeRatio = 1 / TorBoardSizeRatio;

            // 判断航行器是否正对鱼雷切板
            if(TorBoardSizeRatio < TORPEDO_ISFACING_RATIO)
                IsFacing = true;
            else
                IsFacing = false;

            if(IsFacing)
            {
                vector<vector<Point>> tmpContours;
                vector<double> ContoursMultiXYs;
                tmpContours = HolesContours;

                if(tmpContours.size() > 0)
                {
                    if(tmpContours.size() > 4)
                        DeleteContoursByRank(tmpContours, 4);

                    for(int i = 0; i < tmpContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(tmpContours[i], tmpRect);

                        Point2f center = tmpRect.center;
                        ContoursMultiXYs.push_back(center.x * center.y);
                    }

                    // 降序冒泡法
                    for(int i = 0; i < tmpContours.size(); i++)
                    {
                        for(int j = 0; j < tmpContours.size() - 1; j ++)
                            if(ContoursMultiXYs[j] > ContoursMultiXYs[j+1])
                            {
                                std::swap(ContoursMultiXYs[j], ContoursMultiXYs[j+1]);
                                std::swap(tmpContours[j], tmpContours[j+1]);
                            }
                    }

                    switch(tmpContours.size())
                    {
                    // 1个孔洞轮廓
                    case 1:
                    {
                        Point2f ScreenCenter = Point(imgtmp1.size().width / 2, imgtmp1.size().height / 2);
                        // 看到右下角，则为左上角轮廓
                        if(TorBoardCenter.x > ScreenCenter.x
                                && TorBoardCenter.y > ScreenCenter.y)
                        {
                            ULHole.HoleHave = true;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = false;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            ULHole.CalHoleState(tmpRect);
                        }
                        // 看到左下角，则为右上角轮廓
                        else if(TorBoardCenter.x < ScreenCenter.x
                                && TorBoardCenter.y > ScreenCenter.y)
                        {
                            ULHole.HoleHave = false;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = true;
                            DRHole.HoleHave = false;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            URHole.CalHoleState(tmpRect);
                        }
                        // 看到右上角，则为左下角轮廓
                        else if(TorBoardCenter.x > ScreenCenter.x
                                && TorBoardCenter.y < ScreenCenter.y)
                        {
                            ULHole.HoleHave = false;
                            DLHole.HoleHave = true;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = false;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            DLHole.CalHoleState(tmpRect);
                        }
                        // 看到左上角，则为右下角轮廓
                        else if(TorBoardCenter.x < ScreenCenter.x
                                && TorBoardCenter.y < ScreenCenter.y)
                        {
                            ULHole.HoleHave = false;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = true;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            DRHole.CalHoleState(tmpRect);
                        }
                        break;
                    }
                    // 2个空洞轮廓
                    case 2:
                    {
                        Size2f ScreenSize = imgtmp1.size();

                        //**************
                        // 调试用代码段
                        //**************
                        // 鱼雷切板拟合矩形在视野中心，则视为左上和右下孔洞轮廓；
                        // （小轮廓可能在前面被滤去）
                        if(TorBoardCenter.x > ScreenSize.width * 1 / 4
                                && TorBoardCenter.x < ScreenSize.width * 3 / 4
                                && TorBoardCenter.x > ScreenSize.height * 1 / 4
                                && TorBoardCenter.x < ScreenSize.height * 3 / 4)
                        {
                            ULHole.HoleHave = true;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = true;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            ULHole.CalHoleState(tmpRect);
                            ContourFitRotatedRect(tmpContours[1], tmpRect);
                            DRHole.CalHoleState(tmpRect);
                        }
                        // 鱼雷切板拟合矩形在视野左边，则视为右上和右下孔洞轮廓；
                        else if(TorBoardCenter.x < ScreenSize.width * 1 / 4)
                        {
                            ULHole.HoleHave = false;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = true;
                            DRHole.HoleHave = true;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            URHole.CalHoleState(tmpRect);
                            ContourFitRotatedRect(tmpContours[1], tmpRect);
                            DRHole.CalHoleState(tmpRect);
                        }
                        // 鱼雷切板拟合矩形在视野右边，则视为左上和左下孔洞轮廓；
                        else if(TorBoardCenter.x > ScreenSize.width * 3 / 4)
                        {
                            ULHole.HoleHave = true;
                            DLHole.HoleHave = true;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = false;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            ULHole.CalHoleState(tmpRect);
                            ContourFitRotatedRect(tmpContours[1], tmpRect);
                            DLHole.CalHoleState(tmpRect);
                        }
                        // 鱼雷切板拟合矩形在视野上边，则视为左下和右下孔洞轮廓；
                        else if(TorBoardCenter.y < ScreenSize.height * 1 / 4)
                        {
                            ULHole.HoleHave = false;
                            DLHole.HoleHave = true;
                            URHole.HoleHave = false;
                            DRHole.HoleHave = true;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            DLHole.CalHoleState(tmpRect);
                            ContourFitRotatedRect(tmpContours[1], tmpRect);
                            DRHole.CalHoleState(tmpRect);
                        }
                        // 鱼雷切板拟合矩形在视野下边，则视为左上和右上孔洞轮廓；
                        else if(TorBoardCenter.y > ScreenSize.height * 3 / 4)
                        {
                            ULHole.HoleHave = true;
                            DLHole.HoleHave = false;
                            URHole.HoleHave = true;
                            DRHole.HoleHave = false;

                            RotatedRect tmpRect;
                            ContourFitRotatedRect(tmpContours[0], tmpRect);
                            ULHole.CalHoleState(tmpRect);
                            ContourFitRotatedRect(tmpContours[1], tmpRect);
                            URHole.CalHoleState(tmpRect);
                        }

                        break;
                    }
                    // 3个孔洞轮廓
                    case 3:
                    {
                        //**************
                        // 调试用代码段
                        //**************
                        RotatedRect tmpRect;

                        // 中心坐标X, Y值乘积最小者，为左上角孔洞
                        ULHole.HoleHave = true;
                        ContourFitRotatedRect(tmpContours[0], tmpRect);
                        ULHole.CalHoleState(tmpRect);

                        // 中心坐标X, Y值乘积最大者，为右下角孔洞
                        DRHole.HoleHave = true;
                        ContourFitRotatedRect(tmpContours[2], tmpRect);
                        DRHole.CalHoleState(tmpRect);

                        ContourFitRotatedRect(tmpContours[1], tmpRect);
                        // tmpRect中心坐标Y值大于左上孔洞，X值小于右下孔洞时，认为是左下孔洞
                        if(tmpRect.center.y > ULHole.HoleRect.center.y
                                && tmpRect.center.x < DRHole.HoleRect.center.x)
                        {
                            DLHole.HoleHave = true;
                            URHole.HoleHave = false;
                            DLHole.CalHoleState(tmpRect);

                        }
                        else
                        {
                            DLHole.HoleHave = false;
                            URHole.HoleHave = true;
                            URHole.CalHoleState(tmpRect);
                        }

                        break;
                    }
                    case 4:
                    {
                        ULHole.HoleHave = true;
                        DLHole.HoleHave = true;
                        URHole.HoleHave = true;
                        DRHole.HoleHave = true;

                        RotatedRect tmpRect, tmpRect2;
                        // 中心坐标X, Y值乘积最小者，为左上角孔洞
                        ContourFitRotatedRect(tmpContours[0], tmpRect);
                        ULHole.CalHoleState(tmpRect);

                        // 中心坐标X, Y值乘积最大者，为右下角孔洞
                        ContourFitRotatedRect(tmpContours[3], tmpRect);
                        DRHole.CalHoleState(tmpRect);

                        ContourFitRotatedRect(tmpContours[1], tmpRect);
                        ContourFitRotatedRect(tmpContours[2], tmpRect2);

                        // 根据tmpRect, tmpRect2的中心坐标判断左下、右上孔洞的位置
                        if(tmpRect.center.x > tmpRect2.center.x
                                && tmpRect.center.y < tmpRect2.center.y)
                        {
                            URHole.CalHoleState(tmpRect);
                            DLHole.CalHoleState(tmpRect2);
                        }
                        else
                        {
                            URHole.CalHoleState(tmpRect2);
                            DLHole.CalHoleState(tmpRect);
                        }

                        break;
                    }
                    default:
                        break;
                    }
                }
            }

            break;
        }
        case TORPEDO_ATTACH_NOHOLE:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            CurrentHole.HoleHave = false;

            break;
        }
        case TORPEDO_ATTACH_ONE:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            CurrentHole.HoleHave = true;
            CurrentHole.CalHoleState(MaxHoleRect);

            break;
        }
        case TORPEDO_ATTACH_CROSS:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            vector<vector<Point>> tmpContours;
            vector<double> ContoursMultiXYs;
            tmpContours = RedEdgesContours;

            if(tmpContours.size() > 0)
            {
                if(tmpContours.size() > 4)
                    DeleteContoursByRank(tmpContours, 4);

                for(int i = 0; i < tmpContours.size(); i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(tmpContours[i], tmpRect);

                    Point2f center = tmpRect.center;
                    ContoursMultiXYs.push_back(center.x * center.y);
                }

                // 降序冒泡法
                for(int i = 0; i < tmpContours.size(); i++)
                {
                    for(int j = 0; j < tmpContours.size() - 1; j ++)
                        if(ContoursMultiXYs[j] > ContoursMultiXYs[j+1])
                        {
                            std::swap(ContoursMultiXYs[j], ContoursMultiXYs[j+1]);
                            std::swap(tmpContours[j], tmpContours[j+1]);
                        }
                }

                switch(tmpContours.size())
                {
                case 3:
                {
                    //**************
                    // 调试用代码段
                    //**************
                    RotatedRect tmpRect;

                    // 中心坐标X, Y值乘积最小者，为左上角孔洞
                    ULHole.HoleHave = true;
                    ContourFitRotatedRect(tmpContours[0], tmpRect);
                    ULHole.CalHoleState(tmpRect);

                    // 中心坐标X, Y值乘积最大者，为右下角孔洞
                    DRHole.HoleHave = true;
                    ContourFitRotatedRect(tmpContours[2], tmpRect);
                    DRHole.CalHoleState(tmpRect);

                    ContourFitRotatedRect(tmpContours[1], tmpRect);
                    // tmpRect中心坐标Y值大于左上孔洞，X值小于右下孔洞时，认为是左下孔洞
                    if(tmpRect.center.y > ULHole.HoleRect.center.y
                            && tmpRect.center.x < DRHole.HoleRect.center.x)
                    {
                        DLHole.HoleHave = true;
                        URHole.HoleHave = false;
                        DLHole.CalHoleState(tmpRect);
                    }
                    else
                    {
                        DLHole.HoleHave = false;
                        URHole.HoleHave = true;
                        URHole.CalHoleState(tmpRect);
                    }

                    break;
                }
                case 4:
                {
                    ULHole.HoleHave = true;
                    DLHole.HoleHave = true;
                    URHole.HoleHave = true;
                    DRHole.HoleHave = true;

                    RotatedRect tmpRect, tmpRect2;
                    // 中心坐标X, Y值乘积最小者，为左上角孔洞
                    ContourFitRotatedRect(tmpContours[0], tmpRect);
                    ULHole.CalHoleState(tmpRect);

                    // 中心坐标X, Y值乘积最大者，为右下角孔洞
                    ContourFitRotatedRect(tmpContours[3], tmpRect);
                    DRHole.CalHoleState(tmpRect);

                    ContourFitRotatedRect(tmpContours[1], tmpRect);
                    ContourFitRotatedRect(tmpContours[2], tmpRect2);

                    // 根据tmpRect, tmpRect2的中心坐标判断左下、右上孔洞的位置
                    if(tmpRect.center.x > tmpRect2.center.x
                            && tmpRect.center.y < tmpRect2.center.y)
                    {
                        URHole.CalHoleState(tmpRect);
                        DLHole.CalHoleState(tmpRect2);
                    }
                    else
                    {
                        URHole.CalHoleState(tmpRect2);
                        DLHole.CalHoleState(tmpRect);
                    }

                    break;
                }
                default:
                    break;
                }
            }


            break;
        }
        case TORPEDO_ATTACH_LEFT:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            vector<vector<Point>> tmpContours;
            vector<double> ContoursMultiXYs;
            tmpContours = RedEdgesContours;

            if(tmpContours.size() > 0)
            {
                if(tmpContours.size() > 4)
                    DeleteContoursByRank(tmpContours, 4);

                for(int i = 0; i < tmpContours.size(); i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(tmpContours[i], tmpRect);

                    Point2f center = tmpRect.center;
                    ContoursMultiXYs.push_back(center.x * center.y);
                }

                // 降序冒泡法
                for(int i = 0; i < tmpContours.size(); i++)
                {
                    for(int j = 0; j < tmpContours.size() - 1; j ++)
                        if(ContoursMultiXYs[j] > ContoursMultiXYs[j+1])
                        {
                            std::swap(ContoursMultiXYs[j], ContoursMultiXYs[j+1]);
                            std::swap(tmpContours[j], tmpContours[j+1]);
                        }
                }

                ULHole.HoleHave = true;
                DLHole.HoleHave = true;
                URHole.HoleHave = false;
                DRHole.HoleHave = false;

                RotatedRect tmpRect;
                // 中心坐标X, Y乘积最小，为左上角孔洞
                ContourFitRotatedRect(tmpContours[0], tmpRect);
                ULHole.CalHoleState(tmpRect);

                // 中心坐标X, Y乘积最大，为左下角孔洞
                ContourFitRotatedRect(tmpContours[1], tmpRect);
                DLHole.CalHoleState(tmpRect);

                // 根据两孔洞大小，判断大小孔洞信息
                double HoleSize1, HoleSize2;

                HoleSize1 = ULHole.HoleRect.size.width > ULHole.HoleRect.size.height
                        ? ULHole.HoleRect.size.width : ULHole.HoleRect.size.height;
                HoleSize2 = DLHole.HoleRect.size.width > DLHole.HoleRect.size.height
                        ? DLHole.HoleRect.size.width : DLHole.HoleRect.size.height;

                if(HoleSize1 > HoleSize2)
                {
                    SmallHole = DLHole;
                    BigHole = ULHole;
                }
                else
                {
                    SmallHole = ULHole;
                    BigHole = DLHole;
                }

                // 将大孔洞设定为当前寻找目标孔洞
                CurrentHole = BigHole;
            }

            break;
        }
        case TORPEDO_ATTACH_RIGHT:
        {            
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            vector<vector<Point>> tmpContours;
            vector<double> ContoursMultiXYs;
            tmpContours = RedEdgesContours;

            if(tmpContours.size() > 0)
            {
                if(tmpContours.size() > 4)
                    DeleteContoursByRank(tmpContours, 4);

                for(int i = 0; i < tmpContours.size(); i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(tmpContours[i], tmpRect);

                    Point2f center = tmpRect.center;
                    ContoursMultiXYs.push_back(center.x * center.y);
                }

                // 降序冒泡法
                for(int i = 0; i < tmpContours.size(); i++)
                {
                    for(int j = 0; j < tmpContours.size() - 1; j ++)
                        if(ContoursMultiXYs[j] > ContoursMultiXYs[j+1])
                        {
                            std::swap(ContoursMultiXYs[j], ContoursMultiXYs[j+1]);
                            std::swap(tmpContours[j], tmpContours[j+1]);
                        }
                }

                ULHole.HoleHave = false;
                DLHole.HoleHave = false;
                URHole.HoleHave = true;
                DRHole.HoleHave = true;

                RotatedRect tmpRect;
                // 中心坐标X, Y乘积最小，为右上角孔洞
                ContourFitRotatedRect(tmpContours[0], tmpRect);
                URHole.CalHoleState(tmpRect);

                // 中心坐标X, Y乘积最大，为右下角孔洞
                ContourFitRotatedRect(tmpContours[1], tmpRect);
                DRHole.CalHoleState(tmpRect);

                // 根据两孔洞大小，判断大小孔洞信息
                double HoleSize1, HoleSize2;

                HoleSize1 = URHole.HoleRect.size.width > URHole.HoleRect.size.height
                        ? URHole.HoleRect.size.width : URHole.HoleRect.size.height;
                HoleSize2 = DRHole.HoleRect.size.width > DRHole.HoleRect.size.height
                        ? DRHole.HoleRect.size.width : DRHole.HoleRect.size.height;

                if(HoleSize1 > HoleSize2)
                {
                    SmallHole = DRHole;
                    BigHole = URHole;
                }
                else
                {
                    SmallHole = URHole;
                    BigHole = DRHole;
                }

                // 将大孔洞设定为当前寻找目标孔洞
                CurrentHole = BigHole;

            }

            break;
        }
        case TORPEDO_ATTACH_TOP:
        {
            // 求鱼雷切板最大外界矩形
            ContourFitRotatedRect(MaxTorBoard, TorBoardRect);

            vector<vector<Point>> tmpContours;
            vector<double> ContoursMultiXYs;
            tmpContours = RedEdgesContours;

            if(tmpContours.size() > 0)
            {
                if(tmpContours.size() > 4)
                    DeleteContoursByRank(tmpContours, 4);

                for(int i = 0; i < tmpContours.size(); i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(tmpContours[i], tmpRect);

                    Point2f center = tmpRect.center;
                    ContoursMultiXYs.push_back(center.x * center.y);
                }

                // 降序冒泡法
                for(int i = 0; i < tmpContours.size(); i++)
                {
                    for(int j = 0; j < tmpContours.size() - 1; j ++)
                        if(ContoursMultiXYs[j] > ContoursMultiXYs[j+1])
                        {
                            std::swap(ContoursMultiXYs[j], ContoursMultiXYs[j+1]);
                            std::swap(tmpContours[j], tmpContours[j+1]);
                        }
                }

                ULHole.HoleHave = true;
                DLHole.HoleHave = false;
                URHole.HoleHave = true;
                DRHole.HoleHave = false;

                RotatedRect tmpRect;
                // 中心坐标X, Y乘积最小，为左上角孔洞
                ContourFitRotatedRect(tmpContours[0], tmpRect);
                ULHole.CalHoleState(tmpRect);

                // 中心坐标X, Y乘积最大，为右上角孔洞
                ContourFitRotatedRect(tmpContours[1], tmpRect);
                URHole.CalHoleState(tmpRect);

                // 根据两孔洞大小，判断大小孔洞信息
                double HoleSize1, HoleSize2;

                HoleSize1 = ULHole.HoleRect.size.width > ULHole.HoleRect.size.height
                        ? ULHole.HoleRect.size.width : ULHole.HoleRect.size.height;
                HoleSize2 = URHole.HoleRect.size.width > URHole.HoleRect.size.height
                        ? URHole.HoleRect.size.width : URHole.HoleRect.size.height;

                if(HoleSize1 > HoleSize2)
                {
                    SmallHole = URHole;
                    BigHole = ULHole;
                }
                else
                {
                    SmallHole = ULHole;
                    BigHole = URHole;
                }

                // 将大孔洞设定为当前寻找目标孔洞
                CurrentHole = BigHole;
            }

            break;
        }
        default:
            cout<<"Wrong Torpedo Board State!"<<endl;
            break;
        }

        Mat drawingDefects = Mat::zeros(imgtmp1.size(), CV_8UC3);
        Scalar Yellow = Scalar(0, 255, 255);
        Scalar Red = Scalar(0, 0, 255);
        Scalar Blue = Scalar(255, 255, 0);
        drawContours(drawingDefects, TorBoardContours, -1, Yellow);
        drawContours(drawingDefects, HolesContours, -1, Blue);
        drawContours(drawingDefects, RedEdgesContours, -1, Red);

        imshow("Draw Defects", drawingDefects);



        /*===================================================*/
        /*================ 分析信息传给VInfo ==================*/
        /*===================================================*/

        VInfo.TorBoardState = TorBoardState;
        VInfo.TorBoardRect = TorBoardRect;
        VInfo.TorBoardOffsetX = TorBoardOffsetX;
        VInfo.TorBoardOffsetY = TorBoardOffsetY;
        VInfo.IsFacing = IsFacing;

        VInfo.ULHole = ULHole;
        VInfo.DLHole = DLHole;
        VInfo.URHole = URHole;
        VInfo.DRHole = DRHole;

        VInfo.SmallHole = SmallHole;
        VInfo.BigHole = BigHole;
        VInfo.CurrentHole = CurrentHole;

        break;
    }
    default:
        cout<<"Wrong Analyse Torpedo Board Number!"<<endl;
        break;
    }
}

/*===================================================================
 * 函数：AnalyseAnchor()
 *
 * 说明：分析投放标志物，获得相关信息；
 *  整体步骤：
 *  1、提取轮廓：箱子外围轮廓，土黄盖子轮廓，内部黄色图案轮廓，投放目标箱子内部轮廓；
 *
 * 参数：
 *  int num:        识别门策略程序撰写月份
/*===================================================================
 */
void NCVision::AnalyseAnchor(int num)
{
    switch(num)
    {
    case 7:
    {
        /*===================================================*/
        /*===============  计算轮廓部分  ======================*/
        /*===================================================*/

        // 目标是否存在
        bool AimHave = false;
        // 投放标志物任务识别状态
        int AnchorState;
        // 投放标志物任务距离等级
        int AnchorDistState;

        // 是否存在盖子
        bool CoverHave = false;
        // 是否存在目标箱子
        bool AimBoxHave = false;
        // 是否存在黄色图案轮廓
        bool PatternHave = false;

        // 箱子外围轮廓数量
        int BoxNum = 0;

        // 箱子外围轮廓
        vector<vector<Point>> BoxContours;
        // 盖子轮廓
        vector<vector<Point>> CoverContours;
        // 内部黄色图案轮廓
        vector<vector<Point>> PatternContours;
        // 投放目标（没有盖子的箱子）轮廓
        vector<vector<Point>> AimContours;

        // 两个箱子外围轮廓
        vector<Point> Box;
        // 盖子轮廓
        vector<Point> Cover;
        // 图案轮廓
        vector<Point> Pattern;
        // 目标箱子内轮廓
        vector<Point> Aim;
        // 目标箱子外围轮廓
        vector<Point> AimOutBox;

        // 目标箱子孔洞
        Hole AimHole;
        // 盖子孔洞
        Hole CoverHole;
        // 黄色图案孔洞
        Hole PatternHole;

        // 缓存轮廓，存储凹陷部分的轮廓
        vector<vector<Point>> tmpConvex;

        // 箱子的ROI区域
        Rect BoxROI;
        // 盖子的ROI区域
        Rect CoverROI;
        // 取盖子ROI区域后，再取较大范围的ROI
        Rect LargeROI;
        // 目标箱子的ROI区域
        Rect AimROI;

        // 箱子的拟合矩形
        RotatedRect BoxRect;
        // 盖子的拟合矩形
        RotatedRect CoverRect;
        // 目标箱子的拟合矩形
        RotatedRect AimRect;
        // 黄色图案的拟合矩形
        RotatedRect PatternRect;

        // 盖子X, Y方向偏移量
        int CoverOffsetX, CoverOffsetY;
        // 目标X, Y方向偏移量
        int AimOffsetX, AimOffsetY;

        Mat imgBox, imgCover, imgPattern;
        Mat imgDown;
        Mat imgtmp;
        vector<Vec4i> hierarchy;

        //****************************************
        //         箱子、盖子、黄色图案轮廓识别
        //****************************************

        // 提取箱子外围轮廓
        ImgPro.getimgBinAnchorBox(imgBox);
        imgBox.copyTo(imgtmp);
        findContours(imgtmp, BoxContours, hierarchy,
                     CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        // 提取盖子轮廓
        ImgPro.getimgBinAnchorCover(imgCover);
        imgCover.copyTo(imgtmp);
        findContours(imgtmp, CoverContours, hierarchy,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        // 提取黄色图案轮廓
        ImgPro.getimgBinAnchorPattern(imgPattern);
        imgPattern.copyTo(imgtmp);
        findContours(imgtmp, PatternContours, hierarchy,
                     CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        //****************************************
        //           获取盖子信息
        //****************************************

        // 按面积降序排序
        ContoursAreaDownSort(BoxContours);
        ContoursAreaDownSort(CoverContours);
//        ContoursAreaDownSort(PatternContours);

        // 删除较小分量
        DeleteSmallContours(BoxContours, ANCHOR_BOXFILTER_LIMIT);
        DeleteSmallContours(CoverContours, ANCHOR_COVERFILTER_LIMIT);

        // 宽度过小的盖子轮廓，剔出
        for(int i = CoverContours.size() - 1; i >= 0; i--)
        {
            RotatedRect tmpRect;
            ContourFitRotatedRect(CoverContours[i], tmpRect);
            double width = tmpRect.size.height < tmpRect.size.width
                    ? tmpRect.size.height : tmpRect.size.width;
            if(width < 30)
                CoverContours.erase(CoverContours.begin() + i);
        }

        // 判断是否有盖子
        if(CoverContours.size() > 0)
        {
            CoverHave = true;

            // 取面积最大者为盖子轮廓
            Cover = CoverContours[0];
            ContourFitRotatedRect(Cover, CoverRect);
            DeleteContoursByRank(CoverContours, 1);
        }
        else
            CoverHave = false;

        //****************************************
        //    根据盖子信息，获取箱子外界轮廓信息
        //****************************************

        if(CoverHave)
        {
            vector<Vec4i> defects;
            vector<vector<Point>> tmpBoxContours;

            // 取盖子的ROI范围
            ContourFitROIRect(Cover, CoverROI);

            // 取较大范围的ROI，用于定位箱子外围
            LargeROI = Rect(CoverROI.x - CoverROI.width * 3,
                            CoverROI.y - CoverROI.height * 3,
                            CoverROI.width * 6,
                            CoverROI.height * 6);

            // 剔除大范围ROI之外的箱子轮廓
            for(int i = BoxContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(BoxContours[i], tmpRect);
                if(tmpRect.center.x < LargeROI.x
                        || tmpRect.center.x > LargeROI.x + LargeROI.width
                        || tmpRect.center.y < LargeROI.y
                        || tmpRect.center.y > LargeROI.y + LargeROI.height)
                    BoxContours.erase(BoxContours.begin() + i);
            }

            //****************************************
            //    求取目标箱子内侧轮廓
            //****************************************

            if(BoxContours.size() > 0)
            {
                ContourFitROIRect(BoxContours[0], BoxROI);

                // 判断此时箱子的轮廓数量

                // 特殊情况，箱子在边缘状态，且看到了盖子
                if(CoverRect.center.x < imgtmp.size().width * 1/6
                        || CoverRect.center.x > imgtmp.size().width * 5/6
                        || CoverRect.center.y < imgtmp.size().height * 1/6
                        || CoverRect.center.y > imgtmp.size().height * 5/6)
                    BoxNum = 0;
                else
                {
                    if(CoverRect.center.x > BoxROI.x + BoxROI.width * 1/4
                            && CoverRect.center.x < BoxROI.x + BoxROI.width * 3/4
                            && CoverRect.center.y > BoxROI.y + BoxROI.height * 1/4
                            && CoverRect.center.y < BoxROI.y + BoxROI.height * 3/4)
                        BoxNum = 1;
                    else
                        BoxNum = 2;
                }
                switch(BoxNum)
                {
                case 1:
                {
                    // 只剩余一个BoxContour
                    DeleteContoursByRank(BoxContours, 3);

                    AimOutBox = BoxContours[0];

                    tmpBoxContours.clear();
                    // 将BoxContour向量存入tmpBoxContouts
                    tmpBoxContours.push_back(AimOutBox);

                    ContoursMinAreaRect(tmpBoxContours, BoxRect);

                    //*************************************
                    //  如果Box轮廓有凹陷，则进行凹陷部分的处理
                    //*************************************

                    if(!isContourConvex(AimOutBox))
                    {
                        vector<int> hull;

                        // 计算凸包
                        convexHull(AimOutBox, hull, true);

                        //*************************************
                        //计算凹陷部分，并记录与Vec4i的容器defects中
                        //*************************************
                        convexityDefects(AimOutBox,
                                         Mat(hull),
                                         defects);

                        //*************************************
                        //          填充凹陷部分轮廓向量
                        //*************************************

                        for(int i = 0; i < defects.size(); i++)
                        {
                            vector<Point> tmpPoints;
                            // 存储单个凹陷部分轮廓的点
                            for(int j = defects[i][0]; j < defects[i][1]; j++)
                                tmpPoints.push_back(AimOutBox[j]);

                            // 防止出现空向量存入tmpConvex
                            // (比如两个index的首尾分别为934, 0, 则会出现空向量，导致容器处理出错，程序崩溃)
                            if(defects[i][0] < defects[i][1])
                                // 将该凹陷部分轮廓存入凹陷轮廓向量中
                                tmpConvex.push_back(tmpPoints);
                        }

                        //*************************************
                        //处理箱子外围轮廓向量中的内轮廓，并取最大的2个轮廓
                        //*************************************

                        if(tmpConvex.size() > 0)
                        {
                            // 凹陷轮廓向量降序排序
                            ContoursAreaDownSort(tmpConvex);

                            if(tmpConvex.size() >= 2)
                                // 删除剩余2个
                                DeleteContoursByRank(tmpConvex, 2);

                            // 删除过小的轮廓
                            DeleteSmallContours(tmpConvex, ANCHOR_AIMFILTER_LIMIT);

                            // 将处理后的凹陷轮廓计入目标箱子轮廓向量中，为往后的分析作准备
                            for(int i = 0; i < tmpConvex.size(); i++)
                                AimContours.push_back(tmpConvex[i]);
                        }
                    }

                    // 重新将箱子轮廓降序排序
                    ContoursAreaDownSort(BoxContours);

                    if(BoxContours.size() >= 4)
                        // 删除剩4个
                        DeleteContoursByRank(BoxContours, 4);

                    // 在最大轮廓的ROI内寻找面积比较大的轮廓
                    for(int i = 1; i < BoxContours.size(); i++)
                        AimContours.push_back(BoxContours[i]);

                    // 剔除目标箱子轮廓中的比较小的轮廓
                    DeleteSmallContours(AimContours, ANCHOR_AIMFILTER_LIMIT);

                    // 目标轮廓面积降序排列
                    ContoursAreaDownSort(AimContours);

                    // 长宽比过大的目标箱子轮廓，以及在盖子ROI范围内，剔除
                    for(int i = AimContours.size() - 1; i >= 0; i--)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(AimContours[i], tmpRect);
                        double ratio = tmpRect.size.height / tmpRect.size.width;
                        if(ratio < 1)       ratio = 1 / ratio;

                        if(ratio > 3
                                || (tmpRect.center.x > CoverROI.x
                                    && tmpRect.center.x < CoverROI.x + CoverROI.width
                                    && tmpRect.center.y > CoverROI.y
                                    && tmpRect.center.y < CoverROI.y + CoverROI.height)
                                )
                            AimContours.erase(AimContours.begin() + i);
                    }

                    for(int i = 0; i < AimContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(AimContours[i], tmpRect);
                        // 遍历目标箱子轮廓集，如果拟合矩形在盖子范围内，则continue
                        if(tmpRect.center.x > CoverROI.x
                                && tmpRect.center.x < CoverROI.x + CoverROI.width
                                && tmpRect.center.y > CoverROI.y
                                && tmpRect.center.y < CoverROI.y + CoverROI.height)
                            continue;
                        else
                        {
                            Aim = AimContours[i];
                            break;
                        }
                    }

                    // 判断Aim是否存在
                    if(Aim.size() > 0)
                    {
                        AimBoxHave = true;
                        ContourFitRotatedRect(Aim, AimRect);
                    }
                    else
                    {
                        AimBoxHave = false;
                        AimRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }

                    break;
                }
                case 0:
                case 2:
                {
                    tmpBoxContours.clear();

                    // 按面积降序排序
                    ContoursAreaDownSort(BoxContours);

                    // 检测盖子3倍ROI范围内，除了盖子之外的大轮廓
                    for(int i = BoxContours.size() - 1; i >= 0; i--)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(BoxContours[i], tmpRect);

                        if(tmpRect.center.x > LargeROI.x
                                && tmpRect.center.x < LargeROI.x + LargeROI.width
                                && tmpRect.center.y > LargeROI.y
                                && tmpRect.center.y < LargeROI.y + LargeROI.height)
                        {
                            if(tmpRect.center.x > CoverROI.x
                                    && tmpRect.center.x < CoverROI.x + CoverROI.width
                                    && tmpRect.center.y > CoverROI.y
                                    && tmpRect.center.y < CoverROI.y + CoverROI.height)
                            {
                                // 将BoxContour向量存入tmpBoxContouts
                                tmpBoxContours.push_back(BoxContours[i]);
                                continue;
                            }
                            else
                            {
                                // 将BoxContour向量存入tmpBoxContouts
                                tmpBoxContours.push_back(BoxContours[i]);
                                AimOutBox = BoxContours[i];
                                break;
                            }
                        }
                    }

                    // 判断是否存在AimOutBox
                    if(AimOutBox.size() <= 0)
                    {
//                        cout<<"Error: No Aim Out Box!"<<endl;
                        AimBoxHave = false;
                        AimRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);

                        break;
                    }
                    else
                    {
                        ContoursMinAreaRect(tmpBoxContours, BoxRect);

                        //*************************************
                        //  如果Box轮廓有凹陷，则进行凹陷部分的处理
                        //*************************************

                        if(!isContourConvex(AimOutBox))
                        {
                            vector<int> hull;

                            // 计算凸包
                            convexHull(AimOutBox, hull, true);

                            //*************************************
                            //计算凹陷部分，并记录与Vec4i的容器defects中
                            //*************************************
                            convexityDefects(AimOutBox,
                                             Mat(hull),
                                             defects);

                            //*************************************
                            //          填充凹陷部分轮廓向量
                            //*************************************

                            for(int i = 0; i < defects.size(); i++)
                            {
                                vector<Point> tmpPoints;
                                // 存储单个凹陷部分轮廓的点
                                for(int j = defects[i][0]; j < defects[i][1]; j++)
                                    tmpPoints.push_back(AimOutBox[j]);

                                // 防止出现空向量存入tmpConvex
                                // (比如两个index的首尾分别为934, 0, 则会出现空向量，导致容器处理出错，程序崩溃)
                                if(defects[i][0] < defects[i][1])
                                    // 将该凹陷部分轮廓存入凹陷轮廓向量中
                                    tmpConvex.push_back(tmpPoints);
                            }

                            //*************************************
                            //处理箱子外围轮廓向量中的内轮廓，并取最大的2个轮廓
                            //*************************************

                            if(tmpConvex.size() > 0)
                            {
                                // 凹陷轮廓向量降序排序
                                ContoursAreaDownSort(tmpConvex);

                                if(tmpConvex.size() >= 2)
                                    // 删除剩余2个
                                    DeleteContoursByRank(tmpConvex, 2);

                                // 删除过小的轮廓
                                DeleteSmallContours(tmpConvex, ANCHOR_AIMFILTER_LIMIT);

                                // 将处理后的凹陷轮廓计入目标箱子轮廓向量中，为往后的分析作准备
                                for(int i = 0; i < tmpConvex.size(); i++)
                                    AimContours.push_back(tmpConvex[i]);
                            }
                        }
                    }

                    // 重新将箱子轮廓降序排序
                    ContoursAreaDownSort(BoxContours);

                    if(BoxContours.size() >= 4)
                        // 删除剩4个
                        DeleteContoursByRank(BoxContours, 4);

                    // 在最大轮廓的ROI内寻找面积比较大的轮廓
                    for(int i = 1; i < BoxContours.size(); i++)
                        AimContours.push_back(BoxContours[i]);

                    // 剔除目标箱子轮廓中的比较小的轮廓
                    DeleteSmallContours(AimContours, ANCHOR_AIMFILTER_LIMIT);

                    // 目标轮廓面积降序排列
                    ContoursAreaDownSort(AimContours);

                    // 长宽比过大的目标箱子轮廓，剔除
                    for(int i = AimContours.size() - 1; i >= 0; i--)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(AimContours[i], tmpRect);
                        double ratio = tmpRect.size.height / tmpRect.size.width;
                        if(ratio < 1)       ratio = 1 / ratio;
                        if(ratio > 3)
                            AimContours.erase(AimContours.begin() + i);
                    }

                    for(int i = 0; i < AimContours.size(); i++)
                    {
                        RotatedRect tmpRect;
                        ContourFitRotatedRect(AimContours[i], tmpRect);
                        // 遍历目标箱子轮廓集，如果拟合矩形在盖子范围内，则continue
                        if(tmpRect.center.x > CoverROI.x
                                && tmpRect.center.x < CoverROI.x + CoverROI.width
                                && tmpRect.center.y > CoverROI.y
                                && tmpRect.center.y < CoverROI.y + CoverROI.height)
                            continue;
                        else
                        {
                            Aim = AimContours[i];
                            break;
                        }
                    }

                    // 判断Aim是否存在
                    if(Aim.size() > 0)
                    {
                        AimBoxHave = true;
                        ContourFitRotatedRect(Aim, AimRect);
                    }
                    else
                    {
                        AimBoxHave = false;
                        AimRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                    }

                    break;
                }
                default:
                    break;
                }
            }
        }
        // 没有盖子时的识别
        else
        {
            vector<Vec4i> defects;
            vector<vector<Point>> tmpBoxContours;

            tmpBoxContours.clear();

            if(BoxContours.size() > 0)
            {
                // 存在BoxContour，则认为最大轮廓为目标箱子外接轮廓
                ContoursAreaDownSort(BoxContours);
                AimOutBox = BoxContours[0];

                // 将BoxContour向量存入tmpBoxContouts
                tmpBoxContours.push_back(AimOutBox);

                ContoursMinAreaRect(tmpBoxContours, BoxRect);


                //*************************************
                //  如果Box轮廓有凹陷，则进行凹陷部分的处理
                //*************************************

                if(!isContourConvex(AimOutBox))
                {
                    vector<int> hull;

                    // 计算凸包
                    convexHull(AimOutBox, hull, true);

                    //*************************************
                    //计算凹陷部分，并记录与Vec4i的容器defects中
                    //*************************************
                    convexityDefects(AimOutBox,
                                     Mat(hull),
                                     defects);

                    //*************************************
                    //          填充凹陷部分轮廓向量
                    //*************************************

                    for(int i = 0; i < defects.size(); i++)
                    {
                        vector<Point> tmpPoints;
                        // 存储单个凹陷部分轮廓的点
                        for(int j = defects[i][0]; j < defects[i][1]; j++)
                            tmpPoints.push_back(AimOutBox[j]);

                        // 防止出现空向量存入tmpConvex
                        // (比如两个index的首尾分别为934, 0, 则会出现空向量，导致容器处理出错，程序崩溃)
                        if(defects[i][0] < defects[i][1])
                            // 将该凹陷部分轮廓存入凹陷轮廓向量中
                            tmpConvex.push_back(tmpPoints);
                    }

                    //*************************************
                    //处理箱子外围轮廓向量中的内轮廓，并取最大的2个轮廓
                    //*************************************

                    if(tmpConvex.size() > 0)
                    {
                        // 凹陷轮廓向量降序排序
                        ContoursAreaDownSort(tmpConvex);

                        if(tmpConvex.size() >= 2)
                            // 删除剩余2个
                            DeleteContoursByRank(tmpConvex, 2);

                        // 删除过小的轮廓
                        DeleteSmallContours(tmpConvex, ANCHOR_AIMFILTER_LIMIT);

                        // 将处理后的凹陷轮廓计入目标箱子轮廓向量中，为往后的分析作准备
                        for(int i = 0; i < tmpConvex.size(); i++)
                            AimContours.push_back(tmpConvex[i]);
                    }
                }

                // 重新将箱子轮廓降序排序
                ContoursAreaDownSort(BoxContours);

                if(BoxContours.size() >= 4)
                    // 删除剩4个
                    DeleteContoursByRank(BoxContours, 4);

                // 在最大轮廓的ROI内寻找面积比较大的轮廓
                for(int i = 1; i < BoxContours.size(); i++)
                    AimContours.push_back(BoxContours[i]);

                // 剔除目标箱子轮廓中的比较小的轮廓
                DeleteSmallContours(AimContours, ANCHOR_AIMFILTER_LIMIT);

                // 目标轮廓面积降序排列
                ContoursAreaDownSort(AimContours);

                // 长宽比过大的目标箱子轮廓，剔除
                for(int i = AimContours.size() - 1; i >= 0; i--)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(AimContours[i], tmpRect);
                    double ratio = tmpRect.size.height / tmpRect.size.width;
                    if(ratio < 1)       ratio = 1 / ratio;
                    if(ratio > 3)
                        AimContours.erase(AimContours.begin() + i);
                }

                for(int i = 0; i < AimContours.size(); i++)
                {
                    RotatedRect tmpRect;
                    ContourFitRotatedRect(AimContours[i], tmpRect);
                    // 遍历目标箱子轮廓集，如果拟合矩形在盖子范围内，则continue
                    if(tmpRect.center.x > CoverROI.x
                            && tmpRect.center.x < CoverROI.x + CoverROI.width
                            && tmpRect.center.y > CoverROI.y
                            && tmpRect.center.y < CoverROI.y + CoverROI.height)
                        continue;
                    else
                    {
                        Aim = AimContours[i];
                        break;
                    }
                }

                // 判断Aim是否存在
                if(Aim.size() > 0)
                {
                    AimBoxHave = true;
                    ContourFitRotatedRect(Aim, AimRect);
                }
                else
                {
                    AimBoxHave = false;
                    AimRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
                }
            }
            else
            {
                AimBoxHave = false;
                AimRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            }
        }

        //****************************************
        //       识别黄色图案轮廓
        //****************************************

        if(AimBoxHave)
        {
            // 拟合目标箱子的ROI区域
            ContourFitROIRect(Aim, AimROI);

            // 黄色图案轮廓出现在目标箱子ROI范围之外，则剔除
            for(int i = PatternContours.size() - 1; i >= 0; i--)
            {
                RotatedRect tmpRect;
                ContourFitRotatedRect(PatternContours[i], tmpRect);
                if(tmpRect.center.x < AimROI.x
                        || tmpRect.center.x > AimROI.x + AimROI.width
                        || tmpRect.center.y < AimROI.y
                        || tmpRect.center.y > AimROI.y + AimROI.height)
                    PatternContours.erase(PatternContours.begin() + i);
            }

            // 判断黄色图案轮廓是否存在
            if(PatternContours.size() > 0)
            {
                DeleteSmallContours(PatternContours, 20);



                PatternHave = true;
                ContoursMinAreaRect(PatternContours, PatternRect);
            }
            else
            {
                PatternHave = false;
                PatternRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
            }
        }
        else
        {
            PatternHave = false;
            PatternRect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
        }

        /*===================================================*/
        /*=============  距离等级与状态判断  ===================*/
        /*===================================================*/

        // 根据目标箱子的大小，判断距离等级
        if(AimHave)
        {
            // AimRect的长度
            double AimBoxLength = AimRect.size.height > AimRect.size.width
                    ? AimRect.size.height : AimRect.size.width;

            if(AimBoxLength > ANCHOR_NEAR_LIMIT)
                AnchorDistState = ANCHOR_NEAR;
            else
                AnchorDistState = ANCHOR_FAR;
        }
        else
        {
            if(CoverHave)
            {
                // CoverRect的长度
                double CoverLength = CoverRect.size.height > CoverRect.size.width
                        ? CoverRect.size.height : CoverRect.size.width;

                if(CoverLength > ANCHOR_NEAR_LIMIT)
                    AnchorDistState = ANCHOR_NEAR;
                else
                    AnchorDistState = ANCHOR_FAR;
            }
            else
                AnchorDistState = ANCHOR_FAR;
        }

        if(CoverHave && AnchorDistState == ANCHOR_NEAR)
        {
            double width = CoverRect.size.width < CoverRect.size.height
                    ? CoverRect.size.width : CoverRect.size.height;

            if(width < 70)
                CoverHave = 0;
        }

        if(AimBoxHave)
        {
            // 目标箱子与黄色图案或盖子同时存在时，判断目标出现
            if(PatternHave || CoverHave)
            {
                AimHave = true;
                AnchorState = ANCHOR_AIMBOX;
            }
            // 否则认为没有目标
            else
            {
                AimHave = false;
                AnchorState = ANCHOR_NOANCHOR;
            }
        }
        else
        {
            AimHave = false;
            if(CoverHave)
                AnchorState = ANCHOR_COVERONLY;
            else
                AnchorState = ANCHOR_NOANCHOR;
        }

//        // 根据目标箱子的大小，判断距离等级
//        if(AimHave)
//        {
//            // AimRect的长度
//            double AimBoxLength = AimRect.size.height > AimRect.size.width
//                    ? AimRect.size.height : AimRect.size.width;

//            if(AimBoxLength > ANCHOR_NEAR_LIMIT)
//                AnchorDistState = ANCHOR_NEAR;
//            else
//                AnchorDistState = ANCHOR_FAR;
//        }
//        else
//        {
//            if(CoverHave)
//            {
//                // CoverRect的长度
//                double CoverLength = CoverRect.size.height > CoverRect.size.width
//                        ? CoverRect.size.height : CoverRect.size.width;

//                if(CoverLength > ANCHOR_NEAR_LIMIT)
//                    AnchorDistState = ANCHOR_NEAR;
//                else
//                    AnchorDistState = ANCHOR_FAR;
//            }
//            else
//                AnchorDistState = ANCHOR_FAR;
//        }

        /*===================================================*/
        /*========== 根据不同的状态和距离计算相关信息  ===========*/
        /*===================================================*/

//        int imgWidth = imgtmp.size().width;
//        int imgHeight = imgtmp.size().height;

        switch(AnchorState)
        {
        case ANCHOR_NOANCHOR:
        {
            CoverHole.HoleHave = false;
            AimHole.HoleHave = false;
            PatternHole.HoleHave = false;

            break;
        }
        case ANCHOR_COVERONLY:
        {
            CoverHole.HoleHave = true;
            AimHole.HoleHave = false;
            PatternHole.HoleHave = false;

            CoverHole.CalHoleState(CoverRect);

            break;
        }
        case ANCHOR_AIMBOX:
        {
            AimHole.HoleHave = true;
            AimHole.CalHoleState(AimRect);

            if(CoverHave)
            {
                CoverHole.HoleHave = true;
                CoverHole.CalHoleState(CoverRect);
            }
            else
            {
                CoverHole.HoleHave = false;
            }

            if(PatternHave)
            {
                PatternHole.HoleHave = true;
                PatternHole.CalHoleState(PatternRect);
            }
            else
                PatternHole.CalHoleState(PatternRect);

            break;
        }
        default:
            cout<<"Analyse Anchor Error: Wrong Anchor Distance State!"<<endl;
            break;
        }

//        //***************************
//        //       绘制轮廓
//        //***************************
//        Mat drawingDefects = Mat::zeros(imgtmp.size(), CV_8UC3);
//        Scalar Yellow = Scalar(0, 255, 255);
//        Scalar Red = Scalar(0, 0, 255);
//        Scalar Blue = Scalar(255, 255, 0);
//        Scalar White = Scalar(255, 255, 255, 0);

//        drawContours(drawingDefects, BoxContours, -1, White);
//        drawContours(drawingDefects, CoverContours, -1, Blue);
//        drawContours(drawingDefects, AimContours, -1, Red);
//        drawContours(drawingDefects, PatternContours, -1, Yellow);

//        imshow("Anchor Contours", drawingDefects);

        /*===================================================*/
        /*================ 分析信息传给VInfo ==================*/
        /*===================================================*/

        VInfo.AnchorState = AnchorState;
        VInfo.AnchorDistState = AnchorDistState;

        VInfo.AimHole = AimHole;
        VInfo.CoverHole = CoverHole;
        VInfo.PatternHole = PatternHole;

        VInfo.BoxRect = BoxRect;

        break;
    }
    default:
        cout<<"Wrong Analyse Anchor Number!"<<endl;
        break;
    }
}

void NCVision::AnalyseOctCoins()
{
    bool RedCoinHave = false;
    RotatedRect RedCoinRect = RotatedRect(Point2f(-1, -1),
                                          Size2f(-1, -1),
                                          0);

    vector<vector<Point>> RedCoinContours;
    Mat imgRedCoin;
    Mat imgtmp;
    vector<Vec4i> hierarchy;

    ImgPro.getimgBinOctRedCoins(imgRedCoin);
    imgRedCoin.copyTo(imgtmp);
    findContours(imgtmp, RedCoinContours, hierarchy,
                 CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // 按面积降序排序
    ContoursAreaDownSort(RedCoinContours);

    // 删除较小分量
    DeleteSmallContours(RedCoinContours, REDCOIN_LIMIT);

    if(RedCoinContours.size() <= 0)
    {
        RedCoinHave = false;
        RedCoinRect = RotatedRect(Point2f(-1, -1),
                                  Size2f(-1, -1),
                                  0);
    }
    else
    {
        RedCoinHave = true;
        ContourFitRotatedRect(RedCoinContours[0], RedCoinRect);
    }

    /*===================================================*/
    /*================ 分析信息传给VInfo ==================*/
    /*===================================================*/

    VInfo.RedCoinHave = RedCoinHave;
    VInfo.RedCoinRect = RedCoinRect;
}








