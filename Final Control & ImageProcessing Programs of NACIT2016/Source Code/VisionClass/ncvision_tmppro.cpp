#include <ncvision.h>

/*===================================================================
 * 函数：UpdateTmpDoorState(int src)
 *
 * 说明：更新DoorState缓存向量；
 *  1、DoorState缓存向量中存储四个值，这四个值经过一定分析过程，得到该时刻门究竟
 * 处于什么状态；
 *  2、若size小于等于4，则将该时刻门状态值存入DoorState缓存向量尾部；
 *  3、若size大于4，则删去缓存向量第一个值，再将该时刻门状态存于缓存向量尾部；这样
 * 保证了缓存向量个数始终为4个；
 *
 * 参数：
 *  int src:        更新的门状态值；
/*===================================================================
 */
void NCVision::UpdateTmpDoorState(int src)
{
    int size = tmpDoorState.size();

    if (size <= 4 - 1)
    {
        tmpDoorState.push_back(src);
    }
    else
    {
        tmpDoorState.erase(tmpDoorState.begin());
        tmpDoorState.push_back(src);
    }

//    size = tmpDoorState.size();
//    cout<<"Tmp Door Size: "<<size<<endl;
//    cout<<"Tmp Door State: "<<endl;
//    for(int i = 0; i< size; i++)
//        cout<<tmpDoorState[i]<<" ";
//    cout<<endl;
}

/*===================================================================
 * 函数：UpdateTmpLineIDState(int src)
 *
 * 说明：更新LineIDState缓存向量；
 *  1、LineIDState缓存向量中存储3个值，这3个值经过一定分析过程，得到该时刻门究竟
 * 处于什么状态；
 *  2、若size小于等于3，则将该时刻门状态值存入LineIDState缓存向量尾部；
 *  3、若size大于3，则删去缓存向量第一个值，再将该时刻门状态存于缓存向量尾部；这样
 * 保证了缓存向量个数始终为3个；
 *
 * 参数：
 *  int src:        更新的门状态值；
/*===================================================================
 */
void NCVision::UpdateTmpLineIDState(int src)
{
    int size = tmpLineIDState.size();

    if (size <= 3 - 1)
    {
        tmpLineIDState.push_back(src);
    }
    else
    {
        tmpLineIDState.erase(tmpLineIDState.begin());
        tmpLineIDState.push_back(src);
    }

//    size = tmpLineIDState.size();
//    cout<<"Tmp Line In Door Size: "<<size<<endl;
//    cout<<"Tmp Line In Door State: "<<endl;
//    for(int i = 0; i< size; i++)
//        cout<<tmpLineIDState[i]<<" ";
//    cout<<endl;
}

/*===================================================================
 * 函数：UpdateTmpDoorRect(int doorstate, RotatedRect src)
 *
 * 说明：更新DoorRect缓存向量；
 *  1、判断当前门状态；若门状态不为DOOR_WHOLEDOOR，则将缓存矩形统一为center为
 * (-1,-1), Size为(-1,-1)的默认矩形；若门状态为DOOR_WHOLEDOOR，则缓存矩形就
 * 等于输入参数中的Rect；
 *  2、若size小于等于4，则将判断得到的缓存矩形存入tmpDoorRect缓存向量尾部；
 *  3、若size大于4，则删除缓存向量第一个值，再将该时刻缓存矩形存入tmpDoorRect尾部；
 *
 * 参数：
 *  int doorstate:              当前时刻门状态；
 *  RotatedRect src:            多个轮廓拟合出的当前时刻最小外包矩形；
/*===================================================================
 */
void NCVision::UpdateTmpDoorRect(int doorstate, RotatedRect src)
{
    int size = tmpDoorRect.size();
    RotatedRect rect;

    if(doorstate != DOOR_WHOLEDOOR)
    {
//        RotatedRect(const Point2f& center, const Size2f& size, float angle);
        rect = RotatedRect(Point2f(-1,-1), Size2f(-1, -1), -1);
    }
    else
    {
        rect = src;
    }

    if(size <= 4 - 1)
        tmpDoorRect.push_back(rect);
    else
    {
        tmpDoorRect.erase(tmpDoorRect.begin());
        tmpDoorRect.push_back(rect);
    }

//    size = tmpDoorRect.size();
//    cout<<"Tmp DoorRect Size: "<<size<<endl;
////    cout<<"Tmp DoorRect State: "<<endl;
//    for(int i = 0; i< size; i++)
//        cout<<"("<<tmpDoorRect[i].center.x<<", "<<tmpDoorRect[i].center.y<<")"<<endl;
//    cout<<endl<<endl;
}

/*===================================================================
 * 函数：UpdateTmpLineIDROI(int lineIDstate, Rect src)
 *
 * 说明：更新tmpLineIDROI缓存向量；
 *  1、判断当前线状态；若线状态不为DOOR_FOUNDLINE，则将缓存ROI统一为起始点(-1,-1)
 * 长宽为-1,-1的矩形；若线状态为DOOR_WHOLEDOOR，则缓存矩形就等于输入参数中的Rect；
 *  2、若size小于等于3，则将判断得到的缓存矩形存入tmpLineIDROI缓存向量尾部；
 *  3、若size大于3，则删除缓存向量第一个值，再将该时刻缓存矩形存入tmpLineIDROI尾部；
 *
 * 参数：
 *  int lineIDstate:     当前时刻线状态；
 *  Rect src:            前视摄像头中线的ROI；
/*===================================================================
 */
void NCVision::UpdateTmpLineIDROI(int lineIDstate, Rect src)
{
    int size = tmpLineIDROI.size();
    Rect ROI;
    if(lineIDstate != DOOR_FOUNDLINE)
        ROI = Rect(-1, -1, -1, -1);
    else
        ROI = src;

    if(size <= 3 - 1)
        tmpLineIDROI.push_back(ROI);
    else
    {
        tmpLineIDROI.erase(tmpLineIDROI.begin());
        tmpLineIDROI.push_back(ROI);
    }

//    size = tmpLineIDROI.size();
//    cout<<"Tmp Line In Door ROI Size: "<<size<<endl;
////    cout<<"Tmp Line In Door ROI State: "<<endl;
//    for(int i = 0; i< size; i++)
//    {
//        cout<<"("<<tmpLineIDROI[i].x<<", "<<tmpLineIDROI[i].y<<")"<<endl;
//        cout<<"Size: "<<tmpLineIDROI[i].width<<", "<<tmpLineIDROI[i].height<<endl;
//    }
//    cout<<endl<<endl;
}

/*===================================================================
 * 函数：UpdateTmpBallsState(int ballstate)
 *
 * 说明：更新BallState缓存向量；
 *  1、tmpBallState缓存向量中存储5个值，这5个值经过一定分析过程，得到该时刻球究竟
 * 处于什么状态；
 *  2、若size小于等于5，则将该时刻球距离状态值存入tmpBallState缓存向量尾部；
 *  3、若size大于5，则删去缓存向量第一个值，再将该时刻门状态存于缓存向量尾部；这样
 * 保证了缓存向量个数始终为5个；
 *
 * 参数：
 *  int ballstate:        更新的球距离状态值；
/*===================================================================
 */
void NCVision::UpdateTmpBallsState(int ballstate)
{
    int size = tmpBallState.size();

    if(size <= 5 - 1)
        tmpBallState.push_back(ballstate);
    else
    {
        tmpBallState.erase(tmpBallState.begin());
        tmpBallState.push_back(ballstate);
    }

//    size = tmpBallState.size();
//    cout<<"Tmp Ball State Size: "<<size<<endl;
//    cout<<"Tmp Ball State State: "<<endl;
//    for(int i = 0; i < size; i++)
//        cout<<tmpBallState[i]<<" ";
//    cout<<endl;
}

/*===================================================================
 * 函数：AverageTmpDoorRect(RotatedRect &dst)
 *
 * 说明：求DoorRect缓存向量中的旋转矩形的平均值；
 *  1、检查DoorRect向量个数，个数小于等于0则退出函数
 *  2、遍历缓存矩形向量，若检测到当前矩形center为(-1,-1), Size为(-1,-1)，则删除该
 * 矩形；
 *  3、重新计算size，计算缓存矩形向量的平均值；
 *  4、将计算后的平均值传给RotatedRect dst；
 *
 * 参数：
 *  RotatedRect &dst:       输出计算后旋转矩形的平均值；
/*===================================================================
 */
void NCVision::AverageTmpDoorRect(RotatedRect &dst)
{
//    int size = tmpDoorRect.size();
    Point2f dstPoints[4];

    vector<RotatedRect> tmp_tmpRect;
    tmp_tmpRect = tmpDoorRect;

    int size = tmp_tmpRect.size();

    // 检查DoorRect向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Average Tmp Door Rect Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    for(int i = size - 1; i >= 0; i--)
    {
        if(tmp_tmpRect[i].center.x == -1
                && tmp_tmpRect[i].center.y == -1)
            tmp_tmpRect.erase(tmp_tmpRect.begin() + i);
    }


    size = tmp_tmpRect.size();

    // 计算平均值
    for(int i = 0; i < size; i++)
    {
        Point2f tmpPoints[4];
        tmp_tmpRect[i].points(tmpPoints);

        // RotatedRect四点累加；
        for(int j = 0; j < 4; j++)
        {
            dstPoints[j].x += tmpPoints[j].x;
            dstPoints[j].y += tmpPoints[j].y;
        }
    }

    // 求每点平均值
    for(int i = 0; i < 4; i++)
    {
        dstPoints[i].x /= size;
        dstPoints[i].y /= size;
    }

    // 拟合已求出的四个平均点的矩形；
    vector<Point2f> points;
    for(int i = 0; i < 4; i++)
    {
        points.push_back(dstPoints[i]);
    }
    dst = minAreaRect(points);

//    cout<<"Average Rect:"<<endl;
//    cout<<"  Center: ("<<dst.center.x<<", "<<dst.center.y<<")"<<endl;
//    cout<<"  Angle:   "<<dst.angle<<endl;
}

// 求tmpLineIDRI缓存向量中的ROI矩形的平均值；
void NCVision::AverageTmpLineIDROI(Rect &ROI)
{
    Point2f dstPoints[2];

    vector<Rect> tmp_tmpROI;
    tmp_tmpROI = tmpLineIDROI;

    int size = tmp_tmpROI.size();

    // 检查tmpLineIDROI向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Average Tmp Line ID ROI Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    for (int i = size - 1; i >= 0; i --)
    {
        if(tmp_tmpROI[i].x == -1
                && tmp_tmpROI[i].y == -1
                && tmp_tmpROI[i].width == -1
                && tmp_tmpROI[i].height == -1)
            tmp_tmpROI.erase(tmp_tmpROI.begin() + i);
    }

    size = tmp_tmpROI.size();

    // 计算平均值
    for(int i = 0; i < size; i++)
    {
        dstPoints[0].x += tmp_tmpROI[i].x;
        dstPoints[0].y += tmp_tmpROI[i].y;
        dstPoints[1].x += tmp_tmpROI[i].x + tmp_tmpROI[i].width;
        dstPoints[1].y += tmp_tmpROI[i].y + tmp_tmpROI[i].height;
    }

    // 求每点平均值
    for(int i = 0; i < 2; i++)
    {
        dstPoints[i].x /= size;
        dstPoints[i].y /= size;
    }

    ROI = Rect(dstPoints[0], dstPoints[1]);

//    cout<<"Average ROI:"<<endl;
//    cout<<"  Coordinate: ("<<ROI.x<<", "<<ROI.y<<")"<<endl;
//    cout<<"  Size:        "<<ROI.width<<","<<ROI.height<<endl;
}

/*===================================================================
 * 函数：ProcessTmpDoor(int &doorstate, RotatedRect &rect)
 *
 * 说明：处理DoorState缓存向量；
 *  1、检查DoorState向量个数，个数小于等于0则退出函数；
 *  2、遍历DoorState门状态缓存向量，计算门状态缓存向量中，五种状态的个数；
 *  3、依据DOOR_OTHER的个数，对门状态缓存向量分析处理，算出当前的门状态与最小外界矩形：
 *   (1)n5 = 0:
 *      若n3 >= 1: 状态|——|，计算缓存矩形向量的平均矩形；
 *      若n3 = 0 : n1 > n2: 状态|——；
 *                 n1 < n2: 状态 ——|；
 *                 n1 = n2: n4 >= 1: 状态 ——；
 *                          n4 =  0: n0 > 2: 状态无门；
 *                                   n0 <=2: 状态其他；
 *   (2)n5 = 1, 2:
 *      若n3 >= 1: 状态|——|，计算缓存矩形向量的平均矩形；
 *      若n3 = 0 : n1 > n2: 状态|——；
 *                 n1 < n2: 状态 ——|；
 *                 n1 = n2: n4 >= 1: 状态 ——；
 *                          n4 =  0: n0 > 1: 状态无门；
 *                                   n0 <=1: 状态其他；
 *   (3)n5 = 3, 4: 状态其他；
 *
 * 参数：
 *  RotatedRect &dst:       输出计算后旋转矩形的平均值；
/*===================================================================
 */
void NCVision::ProcessTmpDoor(int &doorstate, RotatedRect &rect)
{
    int size = tmpDoorState.size();

    // 检查DoorState向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Process Tmp Door State Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    int n0, n1, n2, n3, n4, n5;
    n0 = n1 = n2 = n3 = n4 = n5 = 0;

//    for(int i = 0; i < size - 1; i++)
    for(int i = 0; i < size; i++)
    {
        switch(tmpDoorState[i])
        {
        case DOOR_NODOOR:       {n0++;break;}
        case DOOR_HALFLEFT:     {n1++;break;}
        case DOOR_HALFRIGHT:    {n2++;break;}
        case DOOR_WHOLEDOOR:    {n3++;break;}
        case DOOR_TOPONLY:      {n4++;break;}
        case DOOR_OTHER:        {n5++;break;}
        default:                {break;}
        }
    }

    switch(n5)
    {
    case 0:
    {
        if(n1 > n3) doorstate = DOOR_HALFLEFT;
        else if (n2 > n3) doorstate = DOOR_HALFRIGHT;
        else
        {
            if(n3 >= 1)
            {
                AverageTmpDoorRect(rect);
                doorstate = DOOR_WHOLEDOOR;
            }
            else
            {
                if(n1 > n2)             doorstate = DOOR_HALFLEFT;
                else if(n1 < n2)        doorstate = DOOR_HALFRIGHT;
                else if(n1 == n2)
                {
                    if(n4 >= 1)         doorstate = DOOR_TOPONLY;
                    else if(n4 == 0)
                    {
                        if(n0 > 2
                                || (n0 > 0 && n1 == 0 && n2 == 0 && n3 == 0 && n4 == 0 && n5 ==0))
                            doorstate = DOOR_NODOOR;
                        else
                            doorstate = DOOR_OTHER;
                    }
                }
            }
        }
        break;
    }
    case 1:
    case 2:
    {
        if(n1 > n3) doorstate = DOOR_HALFLEFT;
        else if (n2 > n3) doorstate = DOOR_HALFRIGHT;
        else
        {
            if(n3 >= 1)
            {
                AverageTmpDoorRect(rect);
                doorstate = DOOR_WHOLEDOOR;
            }
            else
            {
                if(n1 > n2)             doorstate = DOOR_HALFLEFT;
                else if(n1 < n2)        doorstate = DOOR_HALFRIGHT;
                else if(n1 == n2)
                {
                    if(n4 >= 1)         doorstate = DOOR_TOPONLY;
                    else if(n4 == 0)
                    {
                        if(n0 > 1)      doorstate = DOOR_NODOOR;
                        else            doorstate = DOOR_OTHER;
                    }
                }
            }
        }
        break;
    }
    case 3:
    case 4:
    {
        doorstate = DOOR_OTHER;
        break;
    }
    default: break;
    }
}

// 处理LineID相关缓存向量；
void NCVision::ProcessTmpLineID(int &lineIDstate, Rect &ROI)
{
    int size = tmpLineIDState.size();

    // 检查LineIDState向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Process Tmp Line ID State Error: Vector Size is smaller than 0!"<<endl;
    }

    int n0, n1;
    n0 = n1 = 0;

//    for(int i = 0; i < size - 1; i++)
    for(int i = 0; i < size; i++)
    {
        switch(tmpLineIDState[i])
        {
        case DOOR_NOLINE:   {n0++;break;}
        case DOOR_FOUNDLINE:{n1++;break;}
        default:            {break;}
        }
    }

    switch(n1)
    {
    case 0:
    case 1:
    {
        lineIDstate = DOOR_NOLINE;
        break;
    }
    case 2:
    case 3:
    {
        AverageTmpLineIDROI(ROI);
        lineIDstate = DOOR_FOUNDLINE;
        break;
    }
    default: break;
    }
}

// 处理ballstate相关缓存向量；
void NCVision::ProcessTmpBallState(int &ballstate)
{
    int size = tmpBallState.size();

    // 检查ballstate向量个数，个数小于等于0则退出函数
    if(size <= 0)
        cout<<"Process Tmp Balls State Error: Vector Size is smaller than 0!"<<endl;

    int n0, n1, n2, n3, n4, n5;
    n0 = n1 = n2 = n3 = n4 = n5 = 0;
    for(int i = 0; i < size; i++)
    {
        switch(tmpBallState[i])
        {
        case BALL_FAR:              {n0++;break;}
        case BALL_NEAR:             {n1++;break;}
        case BALL_ATTACH:           {n2++;break;}
            // ATTACHED状态被记为ATTACH状态
        case BALL_ATTACHED_RED:     {n2++;n3++;break;}
        case BALL_ATTACHED_YELLOW:  {n2++;n4++;break;}
        case BALL_ATTACHED_GREEN:   {n2++;n5++;break;}
        default:                    {break;}
        }
    }

    if(n3 > 0)      VInfo.RedAttached = true;
    if(n5 > 0)      VInfo.GreenAttached = true;

//    cout<<n0<<" "<<n1<<" "<<n2<<" "<<n3<<" "<<n4<<" "<<n5<<" "<<endl;

    if(n0 > n1 && n0 > n2)
        ballstate = BALL_FAR;
    else if(n1 >= n0 && n1 > n2)
        ballstate = BALL_NEAR;
    else if(n2 >= n0 && n2 >= n1)
        ballstate = BALL_ATTACH;

//    if(n2 >= n0 && n2 >= n1)
//        ballstate = BALL_ATTACH;
//    else if(n1 >= n0 && n1 > n2)
//        ballstate = BALL_NEAR;
//    else if(n0 > n1 && n0 > n2)
//        ballstate = BALL_FAR;
}

// 更新StyleDoorState缓存向量；
void NCVision::UpdateTmpStyleDoorState(int stylestate)
{
    int size = tmpStyleDoorState.size();

    if(size <= 3 - 1)
        tmpStyleDoorState.push_back(stylestate);
    else
    {
        tmpStyleDoorState.erase(tmpStyleDoorState.begin());
        tmpStyleDoorState.push_back(stylestate);
    }

//    size = tmpStyleDoorState.size();
//    cout<<"Tmp Style Door State Size: "<<size<<endl;
//    cout<<"Tmp Style Door States: "<<endl;
//    for(int i = 0; i < size; i++)
//        cout<<tmpStyleDoorState[i]<<" ";
//    cout<<endl;
}

// 更新StyleDoorRect缓存向量；
void NCVision::UpdateTmpStyleDoorRect(int stylestate, RotatedRect src)
{
    int size = tmpStyleDoorRect.size();
    RotatedRect rect;

    if(stylestate != STYLE_NEAR)
        rect = RotatedRect(Point2f(-1,-1), Size2f(-1, -1), -1);
    else
        rect = src;

    if(size <= 3 - 1)
        tmpStyleDoorRect.push_back(rect);
    else
    {
        tmpStyleDoorRect.erase(tmpStyleDoorRect.begin());
        tmpStyleDoorRect.push_back(rect);
    }

//    cout<<"Tmp Style Door Rect Center: ";
//    cout<<"("<<rect.center.x<<", "<<rect.center.y<<")"<<endl;
}

// 更新StyleDoorSeenState缓存向量；
void NCVision::UpdateTmpStyleDoorSeenState(int stylestate)
{
    int size = tmpStyleDoorSeenState.size();

    if(size <= 3 - 1)
        tmpStyleDoorSeenState.push_back(stylestate);
    else
    {
        tmpStyleDoorSeenState.erase(tmpStyleDoorSeenState.begin());
        tmpStyleDoorSeenState.push_back(stylestate);
    }
}

// 更新StyleDoorRect缓存向量(num = 3)；
void NCVision::UpdateTmpStyleDoorSeenRect(int stylestate, RotatedRect src)
{
    int size = tmpStyleDoorRect.size();
    RotatedRect rect;

    if(stylestate != STYLE_WHOLEDOOR)
        rect = RotatedRect(Point2f(-1,-1), Size2f(-1, -1), 0);
    else
        rect = src;

    if(size <= 3 - 1)
        tmpStyleDoorRect.push_back(rect);
    else
    {
        tmpStyleDoorRect.erase(tmpStyleDoorRect.begin());
        tmpStyleDoorRect.push_back(rect);
    }
}

// 处理StyleDoorState相关缓存向量(num = 2)；
void NCVision::ProcessTmpStyleDoor(int &stylestate, RotatedRect &rect)
{
    int size = tmpStyleDoorState.size();

    // 检查StyleDoorState向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Process Tmp Style Door (num = 2) State Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    int n0, n1;
    n0 = n1 = 0;

    for(int i = 0; i < size - 1; i++)
    {
        switch(tmpStyleDoorState[i])
        {
        case STYLE_FAR:     {n0++;break;}
        case STYLE_NEAR:    {n1++;break;}
        default:            {break;}
        }
    }

    if(n1 > n0)
    {
        stylestate = STYLE_NEAR;
        AverageTmpStyleDoorRect(rect);
    }
    else
        stylestate = STYLE_FAR;
}

// 处理StyleDoorState相关缓存向量(num = 3)；
void NCVision::ProcessTmpStyleSeenDoor(int &stylestate, RotatedRect &rect)
{
    int size = tmpStyleDoorSeenState.size();

    // 检查StyleDoorState向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Process Tmp Style Door (num = 3) State Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    int n0, n1, n2, n3, n4;
    n0 = n1 = n2 = n3 = n4 = 0;

    for(int i = 0; i < size - 1; i++)
    {
        switch(tmpStyleDoorSeenState[i])
        {
        case STYLE_NODOOR:      {n0++;break;}
        case STYLE_HALFLEFT:    {n1++;break;}
        case STYLE_HALFRIGHT:   {n2++;break;}
        case STYLE_WHOLEDOOR:   {n3++;break;}
        case STYLE_BOTTOMONLY:  {n4++;break;}
        default:                {break;}
        }
    }

    if(n3 >= n0 && n3 >= n1 && n3 >= n2 && n3 >= n4)
        stylestate = STYLE_WHOLEDOOR;
    else if(n4 >= n0 && n4 >= n1 && n4 >= n2 && n4 >= n3)
        stylestate = STYLE_BOTTOMONLY;
    else if(n1 >= n0 && n1 >= n2 && n1 >= n3 && n1 >= n4)
        stylestate = STYLE_HALFLEFT;
    else if(n2 >= n0 && n2 >= n1 && n2 >= n3 && n2 >= n4)
        stylestate = STYLE_HALFRIGHT;
    else if( n0 >= n1 && n0 >= n2 && n0 >= n3 && n0 >= n4)
        stylestate = STYLE_NODOOR;

    if(stylestate == STYLE_WHOLEDOOR)
        AverageTmpStyleDoorRect(rect);
    else
        rect = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
}

/*===================================================================
 * 函数：AverageTmpDoorRect(RotatedRect &dst)
 *
 * 说明：求StyleDoorRect缓存向量中的旋转矩形的平均值；
 *  1、检查StyleDoorRect向量个数，个数小于等于0则退出函数
 *  2、遍历缓存矩形向量，若检测到当前矩形center为(-1,-1), Size为(-1,-1)，则删除该
 * 矩形；
 *  3、重新计算size，计算缓存矩形向量的平均值；
 *  4、将计算后的平均值传给RotatedRect dst；
 *
 * 参数：
 *  RotatedRect &dst:       输出计算后旋转矩形的平均值；
/*===================================================================
 */
void NCVision::AverageTmpStyleDoorRect(RotatedRect &dst)
{
    Point2f dstPoints[4];

    vector<RotatedRect> tmp_tmpRect;
    tmp_tmpRect = tmpStyleDoorRect;

    int size = tmp_tmpRect.size();

    // 检查StyleDoorRect向量个数，个数小于等于0则退出函数
    if(size <= 0)
    {
        cout<<"Average Tmp Style Door Rect Error: Vector Size is smaller than 0!"<<endl;
        return ;
    }

    for(int i = size - 1; i >= 0; i--)
    {
        if(tmp_tmpRect[i].center.x == -1
                && tmp_tmpRect[i].center.y == -1)
            tmp_tmpRect.erase(tmp_tmpRect.begin() + i);
    }

    size = tmp_tmpRect.size();

    if(size > 0)
    {
        // 计算平均值
        for(int i = 0; i < size; i++)
        {
            Point2f tmpPoints[4];
            tmp_tmpRect[i].points(tmpPoints);

            // RotatedRect四点累加；
            for(int j = 0; j < 4; j++)
            {
                dstPoints[j].x += tmpPoints[j].x;
                dstPoints[j].y += tmpPoints[j].y;
            }
        }

        // 求每点平均值
        for(int i = 0; i < 4; i++)
        {
            dstPoints[i].x /= size;
            dstPoints[i].y /= size;
        }

        // 拟合已求出的四个平均点的矩形；
        vector<Point2f> points;
        for(int i = 0; i < 4; i++)
        {
            points.push_back(dstPoints[i]);
        }
        dst = minAreaRect(points);
    }
    else
        dst = RotatedRect(Point2f(-1, -1), Size2f(-1, -1), 0);
//    cout<<"Style Door Average Rect:"<<endl;
//    cout<<"  Center: ("<<dst.center.x<<", "<<dst.center.y<<")"<<endl;
//    cout<<"  Angle:   "<<dst.angle<<endl;
}
