#include <ncvision.h>

// cout过门任务信息；
void NCVision::PrintFirstDoorInfo(int num)
{
    switch(num)
    {
    case 5:
    {
        //***********************************
        //        输出Line ID State
        //***********************************

        cout<<"Line In Door state: ";
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
            cout<<"Found Line"<<endl;
        else
            cout<<"No Line"<<endl;

        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            //***********************************
            //        输出Line ID Far
            //***********************************

            cout<<"Line In Door Far: ";
            if(VInfo.lineIDFar == DOOR_FAR)
                cout<<"Far"<<endl;
            else
                cout<<"Near"<<endl;

            if(VInfo.lineIDFar == DOOR_NEAR)
            {
                //***********************************
                //        输出Line ID Offset X
                //***********************************

                cout<<"Line In Door Offset X: ";
                cout<<VInfo.LineIDOffsetX<<endl;

                //***********************************
                //        输出Door State
                //***********************************

                cout<<"Door State: ";
                switch(VInfo.doorstate)
                {
                case DOOR_NODOOR:       {cout<<"No"<<endl;break;}
                case DOOR_HALFLEFT:     {cout<<"Left"<<endl;break;}
                case DOOR_HALFRIGHT:    {cout<<"Right"<<endl;break;}
                case DOOR_WHOLEDOOR:    {cout<<"Whole"<<endl;break;}
                case DOOR_TOPONLY:      {cout<<"Top"<<endl;break;}
                default:                {cout<<"Other"<<endl;break;}
                }

                if(VInfo.doorstate == DOOR_WHOLEDOOR)
                {
                    //***********************************
                    //        输出Door Offset X
                    //***********************************

                    cout<<"Door Offset X: ";
                    cout<<VInfo.CenterOffsetX<<endl;
                }
            }
        }
        break;
    }
    case 6:
    {
        //***********************************
        //        输出Line ID State
        //***********************************

        cout<<"Line In Door state: ";
        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
            cout<<"Found Line"<<endl;
        else
            cout<<"No Line"<<endl;

        if(VInfo.lineIDstate == DOOR_FOUNDLINE)
        {
            //***********************************
            //        输出Line ID Far
            //***********************************

            cout<<"Line In Door Far: ";
            if(VInfo.lineIDFar == DOOR_FAR)
                cout<<"Far"<<endl;
            else
                cout<<"Near"<<endl;

            if(VInfo.lineIDFar == DOOR_NEAR)
            {
                //***********************************
                //        输出Line ID Offset X
                //***********************************

                cout<<"Line In Door Offset X: ";
                cout<<VInfo.LineIDOffsetX<<endl;
            }
        }
        break;
    }
    default:
        cout<<"Print First Door Info Error: Wrong Number!"<<endl;
        break;
    }


}

// cout寻线任务信息；
void NCVision::PrintLineInfo(int num)
{
    switch(num)
    {
    case 4:
    {
        //***********************************
        //        输出Line State
        //***********************************

        cout<<"Line State: ";
        if(VInfo.linestate == GUIDE_NOLINE)
            cout<<"No Line"<<endl;
        else
        {
            //***********************************
            //     GUIDE_POINT状态下的输出
            //***********************************
            if(VInfo.linestate == GUIDE_POINT)
            {
                cout<<"Point"<<endl;

                // 输出直线信息
                cout<<"Line Angle: ";
                cout<<VInfo.LineAngle<<endl;
            }

            //***********************************
            //     GUIDE_POINT状态下的输出
            //***********************************
            if(VInfo.linestate == GUIDE_LINE)
            {
                cout<<"Line"<<endl;

                // 输出直线信息
                cout<<"Line Angle: ";
                cout<<VInfo.LineAngle<<endl;
            }
        }
        break;
    }
    case 6:
    {
        //***********************************
        //        输出Line State
        //***********************************
        cout<<"Line State: ";
        switch(VInfo.linestate)
        {
        case GUIDE_NOLINE:
        {
            cout<<"No Line"<<endl;
            break;
        }
        case GUIDE_POINT:
        {
            cout<<"Point"<<endl;
            break;
        }
        case GUIDE_LINE:
        {
            cout<<"Line"<<endl;
            break;
        }
        default:
            cout<<"Print Line Info Error: Line State Num Wrong!"<<endl;
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

            cout<<"Line Angle: ";
            cout<<VInfo.LineAngle<<endl;

            //***********************************
            //        输出Line Offset
            //***********************************

            cout<<"Line ROI Offset (X, Y): ";
            cout<<"("<<VInfo.LineOffsetX<<", "<<VInfo.LineOffsetY<<")"<<endl;

            //***********************************
            //        输出IsAboveLine
            //***********************************

            cout<<"Is Above Line: ";
            if(VInfo.IsAboveLine)
                cout<<"YES"<<endl;
            else
                cout<<"NO"<<endl;
        }
        break;
    }
    default:
        cout<<"Print Line Info Error: Wrong Number!"<<endl;
        break;
    }


}

// cout撞球任务信息；
void NCVision::PrintBallsInfo()
{
    //***********************************
    //        输出Color Num
    //***********************************

    cout<<"Find Ball's Color: ";
    switch(VInfo.BallColor)
    {
    case BALL_RED:      {cout<<"Red"<<endl;break;}
    case BALL_YELLOW:   {cout<<"Yellow"<<endl;break;}
    case BALL_GREEN:    {cout<<"Green"<<endl;break;}
    default:            break;
    }

    //***********************************
    //        输出Attached Balls
    //***********************************
    if(VInfo.RedAttached || VInfo.GreenAttached)
    {
        cout<<"Attached Ball's Color: ";
        if(VInfo.RedAttached && VInfo.GreenAttached)
            cout<<"Red, Green"<<endl;
        else if(VInfo.RedAttached && !VInfo.GreenAttached)
            cout<<"Red"<<endl;
        else if(!VInfo.RedAttached && VInfo.GreenAttached)
            cout<<"Green"<<endl;
    }

    //***********************************
    //        输出ball state
    //***********************************

    cout<<"Ball State: ";
    switch(VInfo.ballsstate)
    {
    case BALL_FAR:      {cout<<"Far"<<endl;break;}
    case BALL_NEAR:     {cout<<"Near"<<endl;break;}
    case BALL_ATTACH:   {cout<<"Attach"<<endl;break;}
    default:            {cout<<"Attach"<<endl;break;}
    }

    //***********************************
    //  输出Red, Yellow, Green X,Y Offset
    //***********************************

//    if(VInfo.ballsstate != BALL_FAR)
//    {
        if(VInfo.RedHave == BALL_HAVE)
        {
            cout<<"Red (X, Y) Offset: ";
            cout<<"("<<VInfo.RedOffsetX<<", "<<VInfo.RedOffsetY<<")"<<endl;
        }

        if(VInfo.YellowHave == BALL_HAVE)
        {
            cout<<"Yellow (X, Y) Offset: ";
            cout<<"("<<VInfo.YellowOffsetX<<", "<<VInfo.YellowOffsetY<<")"<<endl;
        }

        if(VInfo.GreenHave == BALL_HAVE)
        {
            cout<<"Green (X, Y) Offset: ";
            cout<<"("<<VInfo.GreenOffsetX<<", "<<VInfo.GreenOffsetY<<")"<<endl;
        }
//    }
}

// cout风格过门任务信息(num = 2)；
void NCVision::PrintStyleDoorInfo()
{
    //***********************************
    //   输出Style Door State
    //***********************************

    cout<<"Style Door State: ";
    if(VInfo.StyleDoorState == STYLE_FAR)
        cout<<"Far"<<endl;
    else
        cout<<"Near"<<endl;

    //***********************************
    //   输出Style Door (X, Y) Offset
    //***********************************

    if(VInfo.StyleDoorState == STYLE_NEAR)
    {
        cout<<"Style Door (X, Y) Offset: ";
        cout<<"("<<VInfo.StyleCenterOffsetX<<", "<<VInfo.StyleCenterOffsetY<<")"<<endl;
    }

}

// cout风格过门任务信息(num = 3)；
void NCVision::PrintStyleSeenDoorInfo()
{
    //***********************************
    //   输出风格过门距离信息Style State
    //***********************************

    cout<<"Style Distance State:";
    switch(VInfo.StyleDoorState)
    {
    case STYLE_FAR:         {cout<<"Far"<<endl;break;}
    case STYLE_NEAR:        {cout<<"Near"<<endl;break;}
    default:                {break;}
    }

    if(VInfo.StyleDoorState == STYLE_NEAR)
    {
        //***********************************
        //   输出Style DoorState
        //***********************************

        cout<<"Style DoorState: ";
        switch(VInfo.StyleDoorSeenState)
        {
        case STYLE_NODOOR:       {cout<<"No"<<endl;break;}
        case STYLE_HALFLEFT:     {cout<<"Left"<<endl;break;}
        case STYLE_HALFRIGHT:    {cout<<"Right"<<endl;break;}
        case STYLE_WHOLEDOOR:    {cout<<"Whole"<<endl;break;}
        case STYLE_BOTTOMONLY:   {cout<<"Bottom"<<endl;break;}
        case STYLE_OTHER:        {cout<<"OTHER"<<endl;break;}
        default:                 {break;}
        }

        if(VInfo.StyleDoorSeenState == STYLE_WHOLEDOOR)
        {
            cout<<"Style Door (X, Y) Offset: ";
            cout<<"("<<VInfo.StyleCenterOffsetX<<", "<<VInfo.StyleCenterOffsetY<<")"<<endl;        

            //***********************************
            //   输出Style Door角度对准状态
            //***********************************

            cout<<"Style Door Angle State: ";
            switch(VInfo.StyleDoorAngleState)
            {
            case STYLE_CENTER:          {cout<<"Center"<<endl;break;}
            case STYLE_LEFT:            {cout<<"Left"<<endl;break;}
            case STYLE_RIGHT:           {cout<<"Right"<<endl;break;}
            case STYLE_UNKNOWNANGLE:    {cout<<"Unknown"<<endl;break;}
            default:                    {break;}
            }
        }
    }
}

// cout鱼雷切板任务信息
void NCVision::PrintTorBoardInfo()
{
    //***********************************
    //    输出TorBoardState
    //***********************************
    cout<<"Torpedo Board State: ";
    switch(VInfo.TorBoardState)
    {
    case TORPEDO_NOBOARD:           {cout<<"No Board"<<endl;break;}
    case TORPEDO_FAR:               {cout<<"Far"<<endl;break;}
    case TORPEDO_NEAR:              {cout<<"Near"<<endl;break;}
    case TORPEDO_ATTACH_NOHOLE:     {cout<<"Attach - No Hole"<<endl;break;}
    case TORPEDO_ATTACH_ONE:        {cout<<"Attach - One Hole"<<endl;break;}
    case TORPEDO_ATTACH_CROSS:      {cout<<"Attach - Cross"<<endl;break;}
    case TORPEDO_ATTACH_LEFT:       {cout<<"Attach - Left"<<endl;break;}
    case TORPEDO_ATTACH_RIGHT:      {cout<<"Attach - Right"<<endl;break;}
    case TORPEDO_ATTACH_TOP:        {cout<<"Attach - Top"<<endl;break;}
    default:                        {break;}
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
        cout<<"Torpedo Board Center: ("<<VInfo.TorBoardRect.center.x<<", "<<VInfo.TorBoardRect.center.y<<")"<<endl;
        cout<<"Torpedo Board Offset: ("<<VInfo.TorBoardOffsetX<<", "<<VInfo.TorBoardOffsetY<<")"<<endl;
        break;
    }
    case TORPEDO_NEAR:
    {
        cout<<"Torpedo Board Center: ("<<VInfo.TorBoardRect.center.x<<", "<<VInfo.TorBoardRect.center.y<<")"<<endl;
        cout<<"Torpedo Board Offset: ("<<VInfo.TorBoardOffsetX<<", "<<VInfo.TorBoardOffsetY<<")"<<endl;

        if(!VInfo.IsFacing)
        {
            cout<<"Is Facing: FALSE"<<endl;
        }
        else
        {
            cout<<"Is Facing: TRUE"<<endl;

            if(VInfo.ULHole.HoleHave)
            {
                cout<<"  UpLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
                cout<<"  UpLeft Hole Target State: ";
                switch(VInfo.ULHole.TargetState)
                {
                case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
                case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
                case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
                case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
                default:                    {cout<<"huh??"<<endl;break;}
                }
            }
            if(VInfo.URHole.HoleHave)
            {
                cout<<"  UpRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
                cout<<"  UpRight Hole Target State: ";
                switch(VInfo.URHole.TargetState)
                {
                case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
                case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
                case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
                case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
                default:                    {cout<<"huh??"<<endl;break;}
                }
            }
            if(VInfo.DLHole.HoleHave)
            {
                cout<<"  DownLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
                cout<<"  DownLeft Hole Target State: ";
                switch(VInfo.DLHole.TargetState)
                {
                case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
                case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
                case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
                case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
                default:                    {cout<<"huh??"<<endl;break;}
                }
            }
            if(VInfo.DRHole.HoleHave)
            {
                cout<<"  DownRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
                cout<<"  DownRight Hole Target State: ";
                switch(VInfo.DRHole.TargetState)
                {
                case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
                case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
                case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
                case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
                default:                    {cout<<"huh??"<<endl;break;}
                }
            }
        }
        break;
    }
    case TORPEDO_ATTACH_NOHOLE:
    {
        cout<<"Attach - No Hole"<<endl;
        break;
    }
    case TORPEDO_ATTACH_ONE:
    {
        cout<<"  Current Hole Center: ("<<VInfo.CurrentHole.Center.x<<", "<<VInfo.CurrentHole.Center.y<<")"<<endl;
        cout<<"  Current Hole Target State: ";
        switch(VInfo.CurrentHole.TargetState)
        {
        case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
        case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
        case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
        case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
        default:                    {cout<<"huh??"<<endl;break;}
        }


        cout<<"Current Hole Left Target Rect: ";
        cout<<VInfo.CurrentHole.LeftTarget.rect.x<<" "<<VInfo.CurrentHole.LeftTarget.rect.height<<endl;

        break;
    }
    case TORPEDO_ATTACH_CROSS:
    {
        if(VInfo.ULHole.HoleHave)
        {
            cout<<"  UpLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  UpLeft Hole Target State: ";
            switch(VInfo.ULHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.URHole.HoleHave)
        {
            cout<<"  UpRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  UpRight Hole Target State: ";
            switch(VInfo.URHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.DLHole.HoleHave)
        {
            cout<<"  DownLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  DownLeft Hole Target State: ";
            switch(VInfo.DLHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.DRHole.HoleHave)
        {
            cout<<"  DownRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  DownRight Hole Target State: ";
            switch(VInfo.DRHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        break;
    }
    case TORPEDO_ATTACH_LEFT:
    case TORPEDO_ATTACH_RIGHT:
    case TORPEDO_ATTACH_TOP:
    {
        if(VInfo.ULHole.HoleHave)
        {
            cout<<"  UpLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  UpLeft Hole Target State: ";
            switch(VInfo.ULHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.URHole.HoleHave)
        {
            cout<<"  UpRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  UpRight Hole Target State: ";
            switch(VInfo.URHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.DLHole.HoleHave)
        {
            cout<<"  DownLeft Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  DownLeft Hole Target State: ";
            switch(VInfo.DLHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        if(VInfo.DRHole.HoleHave)
        {
            cout<<"  DownRight Hole Center: ("<<VInfo.ULHole.Center.x<<", "<<VInfo.ULHole.Center.y<<")"<<endl;
            cout<<"  DownRight Hole Target State: ";
            switch(VInfo.DRHole.TargetState)
            {
            case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
            case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
            case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
            case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
            default:                    {cout<<"huh??"<<endl;break;}
            }
        }
        break;

        // 输出目标孔洞信息
        cout<<"  Current Hole Center: ("<<VInfo.CurrentHole.Center.x<<", "<<VInfo.CurrentHole.Center.y<<")"<<endl;
        cout<<"  Current Hole Target State: ";
        switch(VInfo.CurrentHole.TargetState)
        {
        case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
        case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
        case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
        case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
        default:                    {cout<<"huh??"<<endl;break;}
        }
    }

    default:
        cout<<"Wrong Torpedo Board State!"<<endl;
        break;
    }

    cout<<endl;

}

// cout投放标志物任务信息
void NCVision::PrintAnchorInfo()
{
    cout<<"Anchor Distance State: ";
    //***********************************
    //    输出AnchorDistState
    //***********************************
    switch(VInfo.AnchorDistState)
    {
    case ANCHOR_FAR:
    {
        cout<<"FAR"<<endl;
        break;
    }
    case ANCHOR_NEAR:
    {
        cout<<"NEAR"<<endl;
        break;
    }
    default:
        cout<<"ERROR"<<endl;
        cout<<"Wrong Anchor Distance State!"<<endl;
        break;
    }

    cout<<"Anchor State: ";
    //***********************************
    //    输出AnchorState
    //***********************************
    switch(VInfo.AnchorState)
    {
    case ANCHOR_NOANCHOR:
    {
        cout<<"NO ANCHOR"<<endl;
        break;
    }
    case ANCHOR_COVERONLY:
    {
        cout<<"COVER"<<endl;
        cout<<"  Cover's Center (X, Y): ("<<VInfo.CoverHole.Center.x<<", "<<VInfo.CoverHole.Center.y<<")"<<endl;
        break;
    }
    case ANCHOR_AIMBOX:
    {
        cout<<"AIM"<<endl;
        if(VInfo.CoverHole.HoleHave)
            cout<<"  Cover's Center (X, Y): ("<<VInfo.CoverHole.Center.x<<", "<<VInfo.CoverHole.Center.y<<")"<<endl;

        if(VInfo.PatternHole.HoleHave)
            cout<<"  Pattern's Center (X, Y): ("<<VInfo.PatternHole.Center.x<<", "<<VInfo.PatternHole.Center.y<<")"<<endl;

        cout<<"  Aim's Center (X, Y): ("<<VInfo.AimHole.Center.x<<", "<<VInfo.AimHole.Center.y<<")"<<endl;

        // 输出对准目标信息
        cout<<"  Aim's Target State: ";
        switch(VInfo.AimHole.TargetState)
        {
        case TARGET_NOTARGET:       {cout<<"No Target"<<endl;break;}
        case TARGET_LEFT:           {cout<<"Left Target"<<endl;break;}
        case TARGET_RIGHT:          {cout<<"Right Target"<<endl;break;}
        case TARGET_CENTER:         {cout<<"Center Target"<<endl;break;}
        default:                    {cout<<"huh??"<<endl;break;}
        }

        break;
    }
    default:
        cout<<"Wrong Anchor State!"<<endl;
        break;
    }

    cout<<endl;
}
