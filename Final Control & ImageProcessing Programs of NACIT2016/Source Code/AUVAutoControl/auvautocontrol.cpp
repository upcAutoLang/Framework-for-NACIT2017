#include "auvautocontrol.h"
#include "ui_auvautocontrol.h"

#include <mission_param.h>

#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QString>
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

int state[4]={0,0,0,0};//存储AUV的当前状态
int te[4]={0,0,0,0};
int te2[4]={0,0,0,0};
int Task_Stop=0;
int mission_count=0;
Send_Data cmd;

bool Door_Find=false; // 是否找到了门,初始设定为没有找到门
bool MF=false;        // 主推关闭标志，true为开，false为关，默认为false
bool SF=false;        // 侧推关闭标志，true为开，false为关，默认为false
bool No_CenterOff=false;//门距离中线的偏移量是否满足要求
int ThroughDoor_Finish=0;//过门完成状态位，0表示未完成，1表示完成


AuvAutoControl::AuvAutoControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AuvAutoControl)
{
    ui->setupUi(this);
    ui->groupBox_6->setStyleSheet("color:Red");
    ui->groupBox_12->setStyleSheet("color:Red");
    setStyleSheet("QPushButton{color:blue;}""QLabel{color:red}");
    TimeCount=0;
    FileCount=0;
    SendCount=0;
    DoorTime=0;

    depth="0.5";
    pitch="0";
    roll="0";
    yaw="0";

    cmd.log="";
    cmd.log_Length=0;

    temp_exucte="";
    temp_UnderAc="";
    FUnderAcAngle1=0;
    UnderAcAngle1="";

    FUnderAcAngle2=0;
    UnderAcAngle2="";

    UnderAcDirection1="";
    UnderAcDirection2="";
    iamtask = START;
    qDebug()<<iamtask;
    //串口接收槽程序
    connect(&serialExecute,SIGNAL(readyRead()),this,SLOT(ReadFromExecute()));
    connect(&serialUnderAc1,SIGNAL(readyRead()),this,SLOT(ReadFromUnderAc1()));
    connect(&serialUnderAc2,SIGNAL(readyRead()),this,SLOT(ReadFromUnderAc2()));

    datafile=new QFile("data.txt");
    Elogfile=new QFile("Elog.txt");
    Ulogfile=new QFile("Ulog.txt");
    Debug=new QFile("Debug.txt");
    Camera=new QFile("Carema.txt");
    OpenFile();
    QTimer *time_clock = new QTimer(this);//            定时器500ms，管理任务切换和任务执行
    connect(time_clock,SIGNAL(timeout()),this,SLOT(AuvMainControl()));
    time_clock->start(CONTROLTIME);

//    QTimer *write_data_time = new QTimer(this);         //定时器1000ms，记录数据使用
//    connect(write_data_time,SIGNAL(timeout()),this,SLOT(Data_Write()));
//    write_data_time->start(FILEWTIME);

    VInfo.linestate=0;
    VInfo.LineAngle=0;
    VInfo.doorstate=0;
    VInfo.CenterOffsetX=0;
    VInfo.lineIDFar=0;
    VInfo.LineIDOffsetX=0;
    VInfo.lineIDstate=0;
    Vision.setSensorHeight(7);// 设置工作深度

    //**************************************
    //       调试，获取视频代码段
    //**************************************

//    Vision.setGrabFramesVideosPaths();
    //**************************************
    //    实测，获取摄像头实时捕获视频代码段
    //**************************************

    Vision.InitVision();
    Vision.InitCamerasParam();
    Vision.StartCamerasGrab();

    on_Open_Serial_clicked();//打开串口
}

AuvAutoControl::~AuvAutoControl()
{
     CloseFile();
     delete ui;
}
//AUV 主控程序
int AuvAutoControl::AuvMainControl()
{
    TimeCount++;
    MainTime=TimeCount*CONTROLTIME/1000.0;
    ui->lcdNumber->display(MainTime);

    //do something on outtime
    SwitchTask();
    TaskControl();
    FileCount++;
    SendCount++;
    Safety();
    if(FileCount >= 2)
    {
        Data_Write();
        FileCount=0;
    }
    if(SendCount>=1)
    {
        //System Time
        //**************************************
        //       调试，获取摄像头视频代码段
        //**************************************
//        Vision.GrabFramesFromVideos();

        //**************************************
        //    实测，获取摄像头实时捕获视频代码段
        //**************************************
        Vision.GrabFrames();

        Vision.sendDownFrame();
        Vision.sendFwdFrame();
        Vision.FramesPrePro(); // 预处理

        if(iamtask==THROUGHDOOR)
        {
            Vision.BinDoorPro();       // 过门二值化处理
            Vision.AnalyseFirstDoor(); // 分析过门二值图

            Vision.BinLinePro();       //寻线二值化处理
            Vision.AnalyseGuideLine(); //分析寻线二值图
        }
        if((iamtask==SCUTTLE_SHIP))
        {
            Vision.BinBallsPro(VInfo.ballsstate);
            Vision.AnalyseBalls(Ball_Color);

            Vision.BinLinePro();       //寻线二值化处理
            Vision.AnalyseGuideLine(); //分析寻线二值图
        }
        // 分析风格过门二值图
        // 参数为2时是初期调试使用的参数，意思是只有两个竖杠；
        // 参数为3时是按照比赛场地的设置，两个竖杠一个横杠；
        if(iamtask==NAVIGATE_CHANNEL)
        {
            Vision.BinStyleDoor(VInfo.StyleDoorState);
            Vision.AnalyseStyleDoor(3);
        }
        if(iamtask == WEIGHT_ANCHOR
                || iamtask == UNDERAC)
        {
            Vision.BinAnchor();
            Vision.AnalyseAnchor();
        }
//        if(iamtask == BURY_TREASURE)
//        {
//            Vision.BinOctCoins();
//            Vision.AnalyseOctCoins();
//        }

        if(image_show)
        {
            if(iamtask==THROUGHDOOR)
            {
                Vision.DrawFirstDoor();
                Vision.DrawGuideLine();
            }
            if(iamtask==SCUTTLE_SHIP)
            {
                Vision.DrawGuideLine();
                Vision.DrawBalls();
            }
            if(iamtask==NAVIGATE_CHANNEL)
            {
                Vision.DrawStyleSeenDoor();
            }
            if(iamtask == WEIGHT_ANCHOR || iamtask == UNDERAC)
            {
                Vision.DrawAnchor();
            }
//            if(iamtask == BURY_TREASURE)
//                Vision.DrawOctCoins();
        }
        // 获取图像信息
        Vision.getVisionInfo(VInfo);
        SendCount=0;
    }
    return 0;
}
void AuvAutoControl::OpenFile()
{
     if(!datafile->isOpen())
        {
            datafile->open(QIODevice::Append|QIODevice::Text);
        }
        if(datafile->isOpen())
        {
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date = current_date_time.toString("yyyy-MM-dd ddd hh:mm:ss ");
            QTextStream outdate(datafile);
            outdate<<current_date<<endl;
            outdate<<"Time "<<'\t';
            outdate << "yaw: " << '\t' << "pitch: " << '\t' << "roll: " << '\t'<< "depth: " << endl;  //记录文件头 to be added
        }
     if(!Elogfile->isOpen())
        {
            Elogfile->open(QIODevice::Append|QIODevice::Text);
        }
        if(Elogfile->isOpen())
        {
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date = current_date_time.toString("yyyy-MM-dd ddd hh:mm:ss ");
            QTextStream outdate(Elogfile);
            outdate<<current_date<<endl;
        }
     if(!Ulogfile->isOpen())
        {
            Ulogfile->open(QIODevice::Append|QIODevice::Text);
        }
        if(Ulogfile->isOpen())
        {
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_date = current_date_time.toString("yyyy-MM-dd ddd hh:mm:ss ");
            QTextStream outdate(Ulogfile);
            outdate<<current_date<<endl;
        }
     if(!Debug->isOpen())
        {
            Debug->open(QIODevice::Append|QIODevice::Text);
        }
      if(Debug->isOpen())
         {
           QDateTime current_date_time = QDateTime::currentDateTime();
           QString current_date = current_date_time.toString("yyyy-MM-dd ddd hh:mm:ss ");
           QTextStream outdate(Debug);
           outdate<<current_date<<endl;
         }
      if(!Camera->isOpen())
            {
                Camera->open(QIODevice::Append|QIODevice::Text);
            }
       if(Camera->isOpen())
          {
              QDateTime current_date_time = QDateTime::currentDateTime();
              QString current_date = current_date_time.toString("yyyy-MM-dd ddd hh:mm:ss ");
              QTextStream outdate(Camera);
              outdate<<current_date<<endl;
              outdate<<"Time "<<'\t';
              outdate<< "LineState" << '\t' << "LineAngle" << '\t'<<"LineOffsetX" << '\t';
//           outdate<< "lineIDstate: " << '\t'<< "lineIDFar: " << '\t' << "LineIDOffsetX: "<<'\t';
//           outdate<< "ballstate"<< '\t' <<"RedHave"<< '\t' <<"YellowHave"<<'\t'<<"GreenHave"<'\t'<<"RedAttached"<<'\t' <<"GreenAttached"<< '\t';
              outdate<< "StyleDoorState" << '\t' << "StyleDoorSeenState"<<'\t'<< "StyleCenterOffsetX" << '\t' << "StyleCenterOffsetY"<< endl;
       }
}

void AuvAutoControl::CloseFile()
{
    if(datafile->isOpen())
    {
       datafile->close();
    }
    if(Elogfile->isOpen())
    {
       Elogfile->close();
    }
    if(Ulogfile->isOpen())
    {
       Ulogfile->close();
    }
    if(Debug->isOpen())
    {
       Debug->close();
    }
    if(Camera->isOpen())
    {
       Camera->close();
    }
}
//打开水声模块串口
void AuvAutoControl::OpenComUnderAc1(QString portName)
{
    serialUnderAc1.setPortName(portName);
    if(serialUnderAc1.isOpen())
    {
        serialUnderAc1.close();
        ui->textBrowser->append("Close First!");
    }
    serialUnderAc1.open(QIODevice::ReadWrite);          //读写打开
    serialUnderAc1.setBaudRate(QSerialPort::Baud115200);  //波特率
    serialUnderAc1.setDataBits(QSerialPort::Data8);     //数据位
    serialUnderAc1.setParity(QSerialPort::NoParity);    //无奇偶校验
    serialUnderAc1.setStopBits(QSerialPort::OneStop);   //1个停止位
    serialUnderAc1.setFlowControl(QSerialPort::NoFlowControl);  //无控制
    if(serialUnderAc1.isOpen())
    {
        ui->textBrowser->setText("SerialU1:"+portName+" Open Succeed");
        ui->lineEdit_ACPortState1->setText("打开");
    }
    else
    {
         ui->textBrowser->append("Open Failed");
    }
}
void AuvAutoControl::OpenComUnderAc2(QString portName)
{
    serialUnderAc2.setPortName(portName);
    if(serialUnderAc2.isOpen())
    {
        serialUnderAc2.close();
        ui->textBrowser->append("Close First!");
    }
    serialUnderAc2.open(QIODevice::ReadWrite);          //读写打开
    serialUnderAc2.setBaudRate(QSerialPort::Baud115200);  //波特率
    serialUnderAc2.setDataBits(QSerialPort::Data8);     //数据位
    serialUnderAc2.setParity(QSerialPort::NoParity);    //无奇偶校验
    serialUnderAc2.setStopBits(QSerialPort::OneStop);   //1个停止位
    serialUnderAc2.setFlowControl(QSerialPort::NoFlowControl);  //无控制
    if(serialUnderAc2.isOpen())
    {
        ui->textBrowser->setText("SerialU2:"+portName+" Open Succeed");
        ui->lineEdit_ACPortState2->setText("打开");
    }
    else
    {
         ui->textBrowser->append("Open Failed");
    }
}
//打开下位机模块串口
void AuvAutoControl::OpenComExecute(QString portName)
{
    serialExecute.setPortName(portName);
    if(serialExecute.isOpen())
    {
        serialExecute.close();
        ui->textBrowser->append("Close First!");
    }
    serialExecute.open(QIODevice::ReadWrite);          //读写打开
    serialExecute.setBaudRate(QSerialPort::Baud9600);  //波特率
    serialExecute.setDataBits(QSerialPort::Data8);     //数据位
    serialExecute.setParity(QSerialPort::NoParity);    //无奇偶校验
    serialExecute.setStopBits(QSerialPort::OneStop);   //无停止位
    serialExecute.setFlowControl(QSerialPort::NoFlowControl);  //无控制
    if(serialExecute.isOpen())
    {
        ui->textBrowser->append("SerialE:"+portName+" Open Succeed");
        ui->lineEdit_ECPortState->setText("打开");
    }
    else
    {
         ui->textBrowser->append("Open Failed");
    }
}

//下位机串口发送程序
void AuvAutoControl::on_ExecuteSend(QString send2)
{
    int num = 0;
    if(send2 == M_S)
        MF = false;
    else if(send2 == M_F1 || send2 == M_F2 || send2 == M_F3
            || send2 == M_B1 || send2 == M_B2 || send2 == M_B3
            || send2 == M_FS1 || send2 == M_FS2 || send2 == M_FS3
                         || send2 == M_BS1 || send2 == M_BS2 || send2 == M_BS3)
        MF = true;
    else if(send2 == S_S)
        SF = false;
    else if(send2 == S_L1 || send2 == S_L2 || send2 == S_L3
            || send2 == S_R1 || send2 == S_R2 || send2 == S_R3
            || send2 == S_LS1 || send2 == S_LS2 || send2 == S_LS3
            || send2 == S_RS1 || send2 == S_RS2 || send2 == S_RS3)
        SF = true;

    cmd.log.clear();//使用之前先清理
    cmd.log_Length=0;
    cmd.log=send2;
    cmd.log_Length=cmd.log.length();
    ComSend2Execute(cmd.log, cmd.log_Length);
}

int AuvAutoControl::ComSend2Execute(QString str,int len)
{
    QByteArray temp_b=str.toLatin1();
    int SendNum=0;
    if(serialExecute.isOpen())
    {
        SendNum=serialExecute.write(temp_b);
        Log_Write("Execute",temp_b);        //指令记录
        if(DEBUGSWITCH)
        {
            if(!SUpdate)
            {
                if(SendNum==len)
                {
                    ui->textBrowser_2->append("SE: "+temp_b);
                    return SendNum;
                }
                else
                {
                    ui->textBrowser_2->append("Send Failure ");
                    ui->textBrowser_2->append("Wrong: "+temp_b);
                    qDebug()<<serialExecute.error();
                    qDebug()<<serialExecute.errorString();
                    return SendNum;
                }
            }
        }
    }
    else
        ui->textBrowser->append("串口未打开");
    return 0;
}

// 读取方向水声模块
void AuvAutoControl::ReadFromUnderAc1()
{
    QByteArray temp = serialUnderAc1.readAll();
    if(!temp.isEmpty())
    {
        temp_UnderAc.append(temp.toHex());
        if(temp_UnderAc.contains("bb"))
        {
            // temp_UnderAc将数据从bb处分开存入QStringList中，并读取第0个索引
            temp_UnderAc = temp_UnderAc.split("bb").at(0);
            // 从右取一定量的字符
            str_Ec = temp_UnderAc.right(EC_Length);
            // 判断指令缓存是否是一个完整接受的指令
            if((str_Ec.startsWith("aa"))&&(str_Ec.length()==(AC_Length)))
            {
                // 水声方向解算
                UnderAcDirection1 = str_Ec.mid(2,2);
                // 水声角度值十进制转换
                StringToDec(str_Ec.mid(4,4));
                UnderAcAngle1 = QString::number(te[0]*16+te[1])+'.'+QString::number((te[2]*16+te[3]));
                // 水声角度值转换为浮点数
                FUnderAcAngle1 = UnderAcAngle1.toFloat();
//                // 在水声任务中，则执行水声任务
//                if(iamtask == UNDERAC)
//                    M_Process_UnderAC();

            }
            ui->textBrowser->append(str_Ec);
            temp_UnderAc.clear();
        }
    }
}

// 读取深度水声模块
void AuvAutoControl::ReadFromUnderAc2()
{
    QByteArray temp = serialUnderAc2.readAll();
//    qDebug()<<"COM13: "<<temp;
    if(!temp.isEmpty())
    {
        temp_UnderAc2.append(temp.toHex());
        if(temp_UnderAc2.contains("bb"))
        {
            temp_UnderAc2=temp_UnderAc2.split("bb").at(0);
            str_Ec2 = temp_UnderAc2.right(EC_Length);
            if((str_Ec2.startsWith("aa"))&&(str_Ec2.length()==(AC_Length)))
            {
                UnderAcDirection2 = str_Ec2.mid(2,2);
                StringToDec2(str_Ec2.mid(4,4));
                UnderAcAngle2 = QString::number(te2[0]*16+te2[1])+'.'+QString::number((te2[2]*16+te2[3])*0.01);
                FUnderAcAngle2 = UnderAcAngle2.toFloat();

                // 水声角度，方向更新
                Oct_LastBeforeAngle = Oct_LastAngle;
                Oct_LastAngle = Oct_CurAngle;
                Oct_CurAngle = FUnderAcAngle2;

                Oct_VerLastAcDir = Oct_VerAcDir;
                if(UnderAcDirection2 == "44")
                    Oct_VerAcDir = 11;
                else
                    Oct_VerAcDir = 22;

                if(UnderAcDirection2 == "44")
                    ui->Oct_Dir_Line->setText("LEFT");
                else
                    ui->Oct_Dir_Line->setText("RIGHT");

                ui->Oct_Angle_Line->setText(QString::number(FUnderAcAngle2));

                // 八边形任务，数据个数的增加
                if(iamtask == BURY_TREASURE
                        || iamtask == UNDERAC
                        || iamtask == WEIGHT_ANCHOR)
                {
                    Oct_DataNum++;

                    // 竖直阵，缓存数据更新
                    for(int i = 0; i < 7 - 1; i++)
                    {
                        Oct_Ac_Angle[i] = Oct_Ac_Angle[i+1];
                    }
                    Oct_Ac_Angle[6] = FUnderAcAngle2;
                }
            }
            ui->textBrowser->append(str_Ec2);
            temp_UnderAc2.clear();
        }
    }
}

// 水声信号处理，QString转换为十进制数，用于导引到标志物盒子的方向水声信号处理
void AuvAutoControl::StringToDec(QString str)
{
    char *temp;
    QByteArray ba;
    ba=str.toLatin1();
    temp=ba.data();
    for(int i=0;i<4;i++)
    {
        if(temp[i]>='0' && temp[i]<='9')
            te[i]=temp[i]-'0';
        else
            if(temp[i]>='A' && temp[i]<='F')
            {
                te[i]=temp[i]-'A'+10;
            }
    }
}

// 水声信号处理，QString转换为十进制数，用于八边形深度水声信号处理
void AuvAutoControl::StringToDec2(QString str)
{
    char *temp;
    QByteArray ba;
    ba=str.toLatin1();
    temp=ba.data();
    for(int i=0;i<4;i++)
    {
        if(temp[i]>='0' && temp[i]<='9')
            te2[i]=temp[i]-'0';
        else
            if(temp[i]>='A' && temp[i]<='F')
            {
                te2[i]=temp[i]-'A'+10;
            }
    }
}

//读取下位机模块串口数据
void AuvAutoControl::ReadFromExecute()
{
    QByteArray temp = serialExecute.readAll();
    if(!temp.isEmpty())
      {
           temp_exucte.append(temp);
           if(temp_exucte.contains('&'))
           {
               temp_exucte = temp_exucte.split('&').at(0);
               str_Ec = temp_exucte.right(EC_Length);
               if((str_Ec.startsWith("YA"))&&(str_Ec.length()==(EC_Length)))
               {
                   yaw=str_Ec.mid(2,5);
                   pitch=str_Ec.mid(9,5);
                   roll=str_Ec.mid(16,6);
                   depth=str_Ec.mid(24,6);
                   depth=QString::number((depth.toFloat()-Atmos)*100);
                   state[0]=temp.mid(34,1).toInt();
                   state[1]=temp.mid(35,1).toInt();
                   state[2]=temp.mid(36,1).toInt();
                   state[3]=temp.mid(37,1).toInt();
               }
               //ui->textBrowser->append(str_Ec);
               temp_exucte.clear();
           }
    }
}

void AuvAutoControl::on_Open_Image_clicked()
{
    QPushButton *button=ui->Open_Image;
    if(!image_show)
    {
        button->setText("Close Image");
        image_show=true;
    }
    else
    {
        button->setText("Open Image");
        image_show=false;
        destroyWindow("Find Line");
        destroyWindow("First Door");
    }
}
void AuvAutoControl::on_pushButton_2_clicked()
{
    QPushButton *button=ui->pushButton_2;
    if(!GUpdate)
    {
        button->setText("更新");
        GUpdate=true;
    }
    else
    {
        button->setText("不更");
        GUpdate=false;
    }
}
void AuvAutoControl::on_pushButton_3_clicked()
{
    QPushButton *button=ui->pushButton_3;
    if(!SUpdate)
    {
        button->setText("更新");
        SUpdate=true;
    }
    else
    {
        button->setText("不更");
        SUpdate=false;
    }
}

void AuvAutoControl::on_Open_Serial_clicked()
{
    QPushButton *button=ui->Open_Serial;
    if(OSerial)
    {
        OpenComUnderAc1(COMUnderAc1);
        OpenComUnderAc2(COMUnderAc2);
        OpenComExecute(COMExecute);
        button->setText("关闭串口");
        OSerial=false;
    }
    else
    {
        serialUnderAc1.close();
        ui->textBrowser->setText("SerialE:com7 Close Succeed");
        serialUnderAc2.close();
        ui->textBrowser->append("SerialU:com4 Close Succeed");
        serialExecute.close();
        ui->textBrowser->append("SerialU:com4 Close Succeed");
        ui->lineEdit_ACPortState1->setText("关闭");
        ui->lineEdit_ACPortState2->setText("关闭");
        ui->lineEdit_ECPortState->setText("关闭");
        button->setText("打开串口");
        OSerial=true;
    }
}

void AuvAutoControl::on_Push_F2_clicked()
{
    on_ExecuteSend(M_F3);
}
void AuvAutoControl::on_Push_B2_clicked()
{
    on_ExecuteSend(M_B3);
}
void AuvAutoControl::on_Push_MS_clicked()
{
    on_ExecuteSend(M_S);
}
void AuvAutoControl::on_Push_L2_clicked()
{
    on_ExecuteSend(S_L3);
}
void AuvAutoControl::on_Push_R2_clicked()
{
    on_ExecuteSend(S_R3);
}
void AuvAutoControl::on_Push_SS_clicked()
{
    on_ExecuteSend(S_S);
}
void AuvAutoControl::on_Push_VS_clicked()
{
    on_ExecuteSend(V_S);
}
void AuvAutoControl::on_Push_UP_clicked()
{
    Send_Depth(0);
}
void AuvAutoControl::on_Push_AS_clicked()
{
    QString temp=ui->textEdit_SA->toPlainText();
    float ang=temp.toDouble();
    if((ang>=360)||(ang<0))
    {
        QMessageBox WarningBox;
        WarningBox.setText("输入角度有误,请输入：0~360");
        WarningBox.exec();
    }
    else
        Send_Angle(ang);
}
void AuvAutoControl::on_Push_DS_clicked()
{
    QString temp=ui->textEdit_SD->toPlainText();
    float dep=temp.toFloat();
    Send_Depth(dep);
}
void AuvAutoControl::on_Mission_Stop_clicked()
{
    iamtask=BODYBACK;
}
void AuvAutoControl::on_Clear_1_clicked()
{
    ui->textBrowser->clear();
}
void AuvAutoControl::on_Clear_2_clicked()
{
    ui->textBrowser_2->clear();
}
void AuvAutoControl::Log_Write(QString source,QString log)//命令记录
{

    QTime now= QTime::currentTime();
    if(source=="Execute")
    {
        QTextStream out(Elogfile);
        out<<(now.toString())<<'\t'<< log << endl;
    }
    if(source=="UnderAc")
    {
        QTextStream out(Ulogfile);
        out<<(now.toString())<<'\t'<< log << endl;
    }
}
void AuvAutoControl::Data_Write()//数据显示和记录
{
    if(DEBUGSWITCH)
    {
        ui->lineEdit_yaw->setText(yaw);
        ui->lineEdit_pitch->setText(pitch);
        ui->lineEdit_roll->setText(roll);
        ui->lineEdit_depth->setText(depth);

        //寻线信息
        if(VInfo.linestate==0)
            ui->Line_State->setText("无线");
        else
            ui->Line_State->setText("有线");
        ui->Line_Angle->setText(QString::number(VInfo.LineAngle));
        ui->Line_OffSet->setText(QString::number(VInfo.LineOffsetX));

        //过门信息
        ui->F_OffSet->setText(QString::number(VInfo.LineIDOffsetX));

        if(VInfo.lineIDstate==0)
        {
            ui->FLine_State->setText("无线");
            ui->F_Distance->setText("远距");
        }
        else
        {
            ui->FLine_State->setText("有线");
            if(VInfo.lineIDFar==0)
                ui->F_Distance->setText("近距");
            else
                ui->F_Distance->setText("远距");
        }
        ui->F_OffSet->setText(QString::number(VInfo.LineIDOffsetX));

        // 撞球信息
        switch(VInfo.ballsstate)
        {
        case 0:
            ui->lineEdit_roll_5->setText("远距");
            break;
        case 1:
            ui->lineEdit_roll_5->setText("近距");
            break;
        case 2:
            ui->lineEdit_roll_5->setText("将要撞球");
            break;
        case 3:
            ui->lineEdit_roll_5->setText("红球已撞");
        case 4:
            ui->lineEdit_roll_5->setText("绿球已撞");
            break;
        default:
            break;
        }

        if(VInfo.RedHave==0)
            ui->lineEdit_roll_2->setText("无球");
        else
            ui->lineEdit_roll_2->setText("有球");
        if(VInfo.GreenHave==0)
            ui->lineEdit_roll_6->setText("无球");
        else
            ui->lineEdit_roll_6->setText("有球");

        ui->lineEdit_roll_4->setText(QString::number(VInfo.RedOffsetX));
        ui->lineEdit_roll_9->setText(QString::number(VInfo.GreenOffsetX));

        ui->lineEdit_roll_7->setText(QString::number(VInfo.RedOffsetY));
        ui->lineEdit_roll_10->setText(QString::number(VInfo.GreenOffsetY));

        switch(Ball_Adjust)
        {
        case BALL_INIT:
        {
            ui->BallMisionEdit->setText("撞球初始");
            ui->lineEdit_roll_3->setText("未撞");
            ui->lineEdit_roll_8->setText("未撞");
            break;
        }
        case BALL_RED_NOADJ:
        {
            ui->BallMisionEdit->setText("红球正在调整");
            break;
        }
        case BALL_RED_ADJ:
        {
            ui->BallMisionEdit->setText("红球调整完毕");
            break;
        }
        case BALL_GREEN_NOADJ:
        {
            ui->BallMisionEdit->setText("绿球正在调整");
            ui->lineEdit_roll_3->setText("已撞");
            break;
        }
        case BALL_GREEN_ADJ:
        {
            ui->BallMisionEdit->setText("绿球调整完毕");
            break;
        }
        case BALL_FINISH:
        {
            ui->BallMisionEdit->setText("撞球结束寻线");
            ui->lineEdit_roll_8->setText("已撞");
            break;
        }
        default:
            ui->BallMisionEdit->setText("---");
            break;
        }
         //Navigate 信息
        if(VInfo.StyleDoorState==STYLE_FAR)
            ui->lineEdit->setText("远距");
        else
            ui->lineEdit->setText("近距");
        switch(VInfo.StyleDoorSeenState)
        {
        case STYLE_NODOOR:
        {
            ui->lineEdit_seenstate->setText("无门");
            break;
        }
        case STYLE_HALFLEFT:
        {
            ui->lineEdit_seenstate->setText("左半部分");
            break;
        }
        case STYLE_HALFRIGHT:
        {
            ui->lineEdit_seenstate->setText("右半部分");
            break;
        }
        case STYLE_WHOLEDOOR:
        {
            ui->lineEdit_seenstate->setText("全门");
            break;
        }
        case STYLE_BOTTOMONLY:
        {
            ui->lineEdit_seenstate->setText("底部");
            break;
        }
        case STYLE_OTHER:
        {
            ui->lineEdit_seenstate->setText("其它");
            break;
        }
        default:
           ui->lineEdit_seenstate->setText("出错");
            break;
        }
        ui->lineEdit_2->setText(QString::number(VInfo.StyleCenterOffsetX));
        ui->lineEdit_3->setText(QString::number(VInfo.StyleCenterOffsetY));

        switch(state[0])
        {
            case 0:
                ui->lineEdit_4->setText("深度未好");
            break;
            case 1:
                ui->lineEdit_4->setText("深度调好");
            break;
            default:
                ui->lineEdit_4->setText("出错");
            break;
        }
        switch(state[1])
        {
            case 0:
                ui->lineEdit_5->setText("侧推停止");
            break;
            case 1:
                ui->lineEdit_5->setText("左移1");
            break;
            case 2:
                ui->lineEdit_5->setText("左移2");
            break;
            case 3:
                ui->lineEdit_5->setText("左移3");
            break;
            case 4:
                ui->lineEdit_5->setText("右移1");
            break;
            case 5:
                ui->lineEdit_5->setText("右移2");
            break;
            case 6:
                ui->lineEdit_5->setText("右移3");
            break;
            default:
                ui->lineEdit_5->setText("出错");
            break;
        }
        switch(state[2])
        {
            case 0:
                ui->lineEdit_6->setText("转向停止");
            break;
            case 1:
                ui->lineEdit_6->setText("转向1");
            break;
            case 2:
                ui->lineEdit_6->setText("转向2");
            break;
            default:
                ui->lineEdit_6->setText("出错");
            break;
        }
        switch(state[3])
        {
            case 0:
                ui->lineEdit_7->setText("直推停止");
            break;
            case 1:
                ui->lineEdit_7->setText("前进1");
            break;
            case 2:
                ui->lineEdit_7->setText("前进2");
            break;
            case 3:
                ui->lineEdit_7->setText("前进3");
            break;
            case 4:
                ui->lineEdit_7->setText("后退1");
            break;
            case 5:
                ui->lineEdit_7->setText("后退2");
            break;
            case 6:
                ui->lineEdit_7->setText("后退3");
            break;
            default:
                ui->lineEdit_7->setText("出错");
            break;
        }
        switch( VInfo.StyleDoorSeenState)
        {
            case STYLE_NODOOR:
                ui->lineEdit_seenstate->setText("NODOOR");
            break;
            case STYLE_HALFLEFT:
                ui->lineEdit_seenstate->setText("LEFT");
            break;
            case STYLE_HALFRIGHT:
                ui->lineEdit_seenstate->setText("RIGHT");
            break;
            case STYLE_WHOLEDOOR:
                ui->lineEdit_seenstate->setText("WHOLEDOOR");
            break;
            case STYLE_BOTTOMONLY:
                ui->lineEdit_seenstate->setText("BOTTOM");
            break;
            case STYLE_OTHER:
                ui->lineEdit_seenstate->setText("OTHER");
            break;
            default:
                ui->lineEdit_7->setText("ERROR");
            break;
        }

        //*************************
        //    ANCHOR信息
        //*************************
        switch(Anchor_Adjust)
        {
        case ANCHOR_INIT:
        {
            ui->Anchor_Adjust_Line->setText("抛锚初始");
            break;
        }
        case ANCHOR_ADJ:
        {
            ui->Anchor_Adjust_Line->setText("下潜调整");
            break;
        }
        case ANCHOR_LEFT:
        {
            ui->Anchor_Adjust_Line->setText("左标志物");
            break;
        }
        case ANCHOR_RIGHT:
        {
            ui->Anchor_Adjust_Line->setText("右标志物");
            break;
        }
        case ANCHOR_FINISH:
        {
            ui->Anchor_Adjust_Line->setText("抛锚结束");
            break;
        }
        default:
            ui->Anchor_Adjust_Line->setText("逗我呢？");
            break;
        }

        switch(VInfo.AnchorDistState)
        {
        case ANCHOR_FAR:
        {
            ui->Anchor_DistState_Line->setText("远距");
            break;
        }
        case ANCHOR_NEAR:
        {
            ui->Anchor_DistState_Line->setText("近距");
            break;
        }
        default:
            ui->Anchor_DistState_Line->setText("Kidding me?");
            break;
        }

        switch(VInfo.AnchorState)
        {
        case ANCHOR_NOANCHOR:
        {
            ui->Anchor_State_Line->setText("没有目标");
            break;
        }
        case ANCHOR_COVERONLY:
        {
            ui->Anchor_State_Line->setText("盖子");
            break;
        }
        case ANCHOR_AIMBOX:
        {
            ui->Anchor_State_Line->setText("目标箱子");
            break;
        }
        default:
            ui->Anchor_State_Line->setText("Kidding me?");
            break;
        }

        if(Anchor_LEFT_Launched)
            ui->Anchor_Left_Line->setText("已投");
        else
            ui->Anchor_Left_Line->setText("未投");

        if(Anchor_RIGHT_Launched)
            ui->Anchor_Right_Line->setText("已投");
        else
            ui->Anchor_Right_Line->setText("未投");
    }

   QTextStream out(datafile);
   QTime now= QTime::currentTime();
   out<<now.toString()<<'\t' << yaw<<'\t' << pitch<<'\t' << roll<<'\t' << depth << endl;

   QTextStream out1(Camera);
     out1<<now.toString()<<'\t' <<VInfo.linestate<< '\t' <<VInfo.LineAngle << '\t' <<VInfo.LineOffsetX << '\t';
 //   out1<< VInfo.lineIDstate << '\t' << VInfo.lineIDFar <<'\t'<< VInfo.LineIDOffsetX <<'\t';
 //   out1<< VInfo.ballsstate<<'\t'<< VInfo.RedHave<<'\t'<< VInfo.YellowHave<<'\t'<< VInfo.GreenHave<<'\t'<< VInfo.RedAttached<<'\t'<<VInfo.GreenAttached<<endl;
      out1<< VInfo.StyleDoorState<<'\t'<< VInfo.StyleDoorSeenState<<'\t'<< VInfo.StyleCenterOffsetX<<'\t'<< VInfo.StyleCenterOffsetY<<endl;
  }

void AuvAutoControl::Send_Angle(float angle)
{
    QString Angle=QString::number(angle,'f',2);
    Angle=Angle.leftJustified(6,'0',true);
    QString send_Angle="*61";
    send_Angle.append(Angle);
    send_Angle.append("\r\n");
    on_ExecuteSend(send_Angle);  //给下位机发送角度值
}
void AuvAutoControl::Send_Depth(float depth)
{
    QString send_Depth="*62";
    QString Depth1=QString::number(depth/100+Atmos, 'd', 4);
    Depth1=Depth1.leftJustified(6,'0',true);
    send_Depth.append(Depth1);
    send_Depth.append("\r\n");
    on_ExecuteSend(send_Depth);  //给下位机发送深度值
}

//切换任务函数
int AuvAutoControl::SwitchTask()//任务切换
{
    // 开始任务完成后，切换到过门任务
    if(missionflag[0]==1)
    {
        iamtask=THROUGHDOOR;
        // 将任务完成标志置0，防止再次切换
        missionflag[0]=0;
    }
    if(missionflag[1]==1)
    {
        iamtask=SCUTTLE_SHIP;
        missionflag[1]=0;
    }
    if(missionflag[2]==1)
    {
        iamtask=NAVIGATE_CHANNEL;
        missionflag[2]=0;
    }
    if(missionflag[3]==1)//
    {
        iamtask=UNDERAC;
        missionflag[3]=0;
    }
    if(missionflag[4]==1)//
    {
        iamtask=WEIGHT_ANCHOR;
        missionflag[4]=0;
    }
    if(missionflag[5]==1)//
    {
        iamtask=BURY_TREASURE;
        missionflag[5]=0;
    }
    if(missionflag[6]==1)//
    {
        iamtask=BODYBACK;
        missionflag[6]=0;
    }
    return 0;
}
//各任务控制函数
void AuvAutoControl::TaskControl()
{
    if(MainTime>(MAXTIME*60))//回收任务，如果时间超过设定值，切换到回收任务（具体时间需要测量决定）
    {
        iamtask=BODYBACK;//执行上浮任务
    }
    switch (iamtask)
    {
    case START:
    {
        ui->lineEdit_task->setText("初始");
        M_Start();//初始任务的控制
        break;
    }
    case THROUGHDOOR:
    {
        ui->lineEdit_task->setText("过门任务");
        M_ThroughDoor();//过门任务的控制
        break;
    }
    case SCUTTLE_SHIP:
    {
        ui->lineEdit_task->setText("破坏任务");
        M_Scuttle_ship();
        break;
    }
    case NAVIGATE_CHANNEL:
    {
        ui->lineEdit_task->setText("过沟任务");
        M_Navigate_channel();
        break;
    }
    case UNDERAC:
    {
        ui->lineEdit_task->setText("水声任务");
        M_Process_UnderAC();
        break;
    }
    case WEIGHT_ANCHOR://SET_COURSE:
    {
        ui->lineEdit_task->setText("抛锚任务");
        M_Weight_anchor();
        break;
    }
    case BURY_TREASURE:
    {
        ui->lineEdit_task->setText("埋宝任务");
        M_Bury_treasure();
        break;
    }
    case BODYBACK:
    {
        ui->lineEdit_task->setText("回收任务");
        M_Bodyback();
        break;
    }
    case RANDOMAC:
    {
        ui->lineEdit_task->setText("任意水声");
        M_RandomPinger();
        break;
    }
    default:
        break;
    }

}

// 开始任务
void AuvAutoControl::M_Start()
{
    // 数据记录
    if(DEBUGSWITCH)
    {
        if(mission_count==0)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Start_Mission"<< endl;
            mission_count++;
            mission_time.Start_Time[0]=MainTime;
        }
    }
    // 记录初始十秒内航向角
    static float yaw_angle[10];
    float And=0;

    // 等待时间之内
    if(first_getangle)
    {
        static int count=0;
        if(MainTime > mission_time.Start_Time[0] + Start_Waittime)
        {
            yaw_angle[count]=yaw.toFloat();
            count++;

            if(count > 0)
                // 嘻嘻源程序
//            if(count > 5)
            {
                for(int i=0;i<5;i++)
                {
                    And=And+yaw_angle[i];
                }
//                // 嘻嘻源程序
//                Start_Angle = And / 5;      //确定航向角
                Start_Angle = And / 1;

                // 跳出该循环
                first_getangle = false;
                if(DEBUGSWITCH)
                {
                    qDebug()<<"Start_Angle:"<<Start_Angle;
                }
                count=0;
            }
        }
    }
    // 十秒后，切换到下一个过门任务，这样写可以保证已经取得了初始航向角
    else
    {
        //******************
        // 比赛时可能会用到……
        //******************
        if(Mission_Part == 'A' || Mission_Part == 'a')
            Start_Angle = PartA_Angle;
        else if(Mission_Part == 'B' || Mission_Part == 'b')
            Start_Angle = 88.01;
        else if(Mission_Part == 'C' || Mission_Part == 'c')
            Start_Angle = 315.01;
        else
            Start_Angle = 276.01;


//        Start_Angle = START_ANGLE;

        // 深度达到精度范围之内
        if((qAbs(depth.toFloat()- set_door_depth) < Proper_Depth))
        {
            // 给下位机发送角度
            Send_Angle(Start_Angle);

            // 如果航向角达到前十秒计算得到的角度的精度范围内
            if(qAbs(yaw.toFloat() - Start_Angle) < Proper_Angle)
            {
                // 初始任务[0]完成，置1
                missionflag[0] = 1;
                // 记录任务结束时间
                mission_time.End_Time[0] = MainTime;
            }
        }
        // 深度没有达到精度范围之内
        else
           // 给下位机发送深度值
           Send_Depth(set_door_depth);
    }
}

// 过门任务
void AuvAutoControl::M_ThroughDoor()
{
    // 任务流程记录
    if(DEBUGSWITCH)
    {
        if(mission_count==1)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"ThroughDoor_Mission"<< endl;
            ui->groupBox_2->setStyleSheet("color:Red;background:yellow");
            ui->groupBox_5->setStyleSheet("color:Red;background:yellow");
            mission_count++;
        }
    }


    if(VInfo.lineIDstate == DOOR_NOLINE)  // 前视摄像头中没有找到导引路径
    {
        if(VInfo.linestate == GUIDE_NOLINE)//俯视摄像头中没有找到导引路径
        {
            if(SF)
            {
                on_ExecuteSend(S_S);
                SF= false;
            }
            else
            {
                on_ExecuteSend(M_F3);
                MF= true;
            }
        }
    }
    // 前视摄像头中找到了导引路径
    else
    {
        // 找到了导引路径，并且比较近
        if(VInfo.lineIDFar == DOOR_FAR
                && ThroughDoor_Finish == 0)
        {
            // 导引路径图像中X方向偏移量较大时
            if(qAbs(VInfo.LineIDOffsetX > 60))
            {
                // 若主推未关闭，先将主推关闭
                if(MF)
                {
                    on_ExecuteSend(M_S);
                    MF = false;
                }
                // 主推已经关闭
                else
                {
                    // 左偏，应该右移（二挡）
                    if(VInfo.LineIDOffsetX < -60)
                        on_ExecuteSend(S_L2);
                    // 右偏，应该左移（二挡）
                    if(VInfo.LineIDOffsetX > 60)
                        on_ExecuteSend(S_R2);
                    SF = true;
                }
            }
            // 导引路径图像中X方向偏移量较小时
            else
            {
                // 先将侧推停止
                if(SF)
                {
                    on_ExecuteSend(S_S);
                    SF=false;
                }
                // 侧推停止后，主推开启（二挡）
                else
                {
                    on_ExecuteSend(M_F3);
                    MF=true;
                }
            }
        }
    }

    // 过门完成标志
    if(ThroughDoor_Finish == 1)
    {
        // 若深度没有调整好
        if(!DepthOK)
        {
            // 给下位机发送寻线深度值
            Send_Depth(GuidanceDepth);
            // 达到目标深度，深度调整完成标志位true
            if(qAbs(depth.toFloat() - GuidanceDepth) < Proper_Depth)
                DepthOK = true;
        }
        // 导引路径没有调整到中央
        else if(!OffCenterOK)
        {
            // 停止主推
            if(MF)
                on_ExecuteSend(M_S);

            // 调整导引路径在X方向的中央区域
            if(qAbs(VInfo.LineOffsetX) > Line_Proper_OffCenter)
            {
                // 右偏，则右移2档
                if(VInfo.LineOffsetX > Line_Proper_OffCenter)
                    on_ExecuteSend(S_R2);
                // 左偏，则左移2档
                else
                    on_ExecuteSend(S_L2);
            }
            // 导引路径到达X方向的精度范围内，则停止侧推
            else
            {
                if(SF)
                    on_ExecuteSend(S_S);
                // X方向调整到中央标志位置为真
                OffCenterOK = true;
            }
        }
        // 导引路径已经调整到了X方向中央
        else
        {
            // 角度没有调整好，开始调整角度
            if(!AngleOK)
            {
                // 第一次发送撞球角度
                if(count_time == 0)
                {
                    // 计算撞球角度
                    Scuttle_Angle = yaw.toFloat() - float(VInfo.LineAngle);
                    // 调整角度在0--360内
                    if(Scuttle_Angle < 0)
                        Scuttle_Angle = Scuttle_Angle + 360;
                    if(Scuttle_Angle > 360)
                        Scuttle_Angle = Scuttle_Angle - 360;

                    // 寻线时，识别到的线为直线状态，发送角度值
                    if(VInfo.linestate == GUIDE_LINE)
                        Send_Angle(Scuttle_Angle);
                    count_time++;
                }
                // 撞球角度到达角度精度范围内
                if(qAbs(Scuttle_Angle - yaw.toFloat()) < Proper_Angle)
                {
                    count_time++;
                    // 对准次数超过五次，则计算导引路径在摄像头中的角度
                    // 如果角度小于3度，则认为角度已经调整好
                    if(count_time >= 5)
                    {
                        if(qAbs(VInfo.LineAngle) < Proper_Angle)
                            AngleOK = true;
                        // 如果此时导引路径角度仍然较大
                        else
                        {
                            // 重新根据导引路径角度，计算并发送新角度值
                            Scuttle_Angle = yaw.toFloat() - float(VInfo.LineAngle);
                            if(Scuttle_Angle < 0)
                                Scuttle_Angle = Scuttle_Angle + 360;
                            if(Scuttle_Angle > 360)
                                Scuttle_Angle = Scuttle_Angle - 360;
                            if((VInfo.linestate == GUIDE_LINE))
                                Send_Angle(Scuttle_Angle);// 发送角度值
                            // 计数器更新
                            count_time = 1;
                        }
                    }
                }
            }
            // 角度调整完毕，则过门任务完成，相关计数器与标志位重置
            else
            {
                // 过门任务[1]变为完成状态
                missionflag[1] = 1;

                count_time = 0;
                // 标志位重置
                DepthOK = false;
                AngleOK = false;
                OffCenterOK = false;
            }
        }
    }
    // 过门没有完成的时候
    else
    {
        // 视觉分析看到了线，且判断导引路径在俯视摄像头中X方向偏离过大，则调节
        if(VInfo.linestate == GUIDE_LINE
            && qAbs(VInfo.LineOffsetX) > Line_Proper_OffCenter)
        {
            // 主推停止
            if(MF)
            {
                on_ExecuteSend(M_S);
                MF = false;
            }
            else
            {
                // 右偏，则右移2档
                if(VInfo.LineOffsetX > Line_Proper_OffCenter)
                    on_ExecuteSend(S_R2);
                // 左偏，则左移2档
                else
                    on_ExecuteSend(S_L2);
                SF = true;
            }
        }
        else
        {
            // 视觉分析看到了线，而且判断航行器已经在线的上方
            if(VInfo.linestate == GUIDE_LINE
                    && VInfo.IsAboveLine == true)
            {
                // 停止侧推
                if(SF)
                    on_ExecuteSend(S_S);
                if(ThroughDoor_Finish == 0)
                {
                    on_ExecuteSend(M_S);
                    MF = false;
                }

                // 过门结束标志位为1
                ThroughDoor_Finish = 1;
            }
            else
            {
                // 关闭侧推
                if(SF)
                    on_ExecuteSend(S_S);// 侧推关闭完毕，则启动主推
                else
                    on_ExecuteSend(M_F3);
            }
        }
    }
}


// 撞球任务
void AuvAutoControl::M_Scuttle_ship()
{
    // 任务数据记录
    if(DEBUGSWITCH)
    {
        if(mission_count == 2)
        {
            // 红球指令
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Scuttle_ship_Mission"<< endl;
            ui->groupBox_4->setStyleSheet("color:Red;background:yellow");
            mission_count++;
        }
    }
    switch(Ball_Adjust)
    {
    //********************
    //    刚进入撞球任务
    //********************
    case BALL_INIT:
    {
        // 判断Find_Ball
        if(VInfo.RedHave == BALL_NOBALL
                && VInfo.YellowHave == BALL_NOBALL
                && VInfo.GreenHave == BALL_NOBALL)
            Find_Ball = false;
        else
            Find_Ball = true;

        // 没有找到球
        if(Find_Ball == false)
        {
            // 关闭侧推
            if(SF)
            {
                on_ExecuteSend(S_S);
                SF = false;
            }
            else
            {
                // 主推开启，前进3档
                MF = true;
                on_ExecuteSend(M_F3);
            }
        }
        // 找到球
        else
        {
            // 若找到球且距离等级为FAR，则前进2档
            if(VInfo.ballsstate == BALL_FAR)
                on_ExecuteSend(M_F2);

            // 若找到球，且距离等级不为FAR，则切换任务，开始寻找红球
            else
            {
                if(Ball_Adjust == BALL_INIT)
                {
                    Ball_Color = BALL_RED;
                    Ball_Adjust = BALL_RED_NOADJ;
                }
            }
        }
        break;
    }

    //********************
    //    刚进入撞球任务
    //********************
    case BALL_RED_NOADJ:
    {
        //
        Ball_Adjusted_OK = false;

        // 判断视野中是否存在红球

        // 视野中没有红球
        if(VInfo.RedHave == BALL_NOBALL)
        {
            // 没有红球，则判断绿球与黄球

            // 如果没有红黄绿球，则后退2档
            //***************
            //  此处有bug。。。
            //***************
            if(VInfo.YellowHave == BALL_NOBALL && VInfo.GreenHave == BALL_NOBALL)
            {
                if(SF)
                    on_ExecuteSend(S_S);
                else
                    on_ExecuteSend(M_B2);
            }
            // 如果找到了黄或绿球，没有找到红球，则左移3档
            else
            {
                // 先将主推关闭
                if(MF)
                {
                    on_ExecuteSend(M_S);
                    MF = false;
                }
                // 主推已经关闭
                else
                {
                    // A区域，左至右颜色依次为RGY
                    // 此时应该左移
                    if(Mission_Part == 'A')
                    {
                        SF = true;
                        on_ExecuteSend(S_L3);
                    }
                    // BCD区域，左至右颜色依次为YGR
                    // 此时应该左移
                    else
                    {
                        SF = true;
                        on_ExecuteSend(S_R3);
                    }
                }
            }
        }

        // 视野中有红球，根据不同距离等级进行动作
        else
        {
            switch(VInfo.ballsstate)
            {
            case BALL_FAR:
            {
                // 红球X方向偏移量过大
                if(qAbs(VInfo.RedOffsetX) > LargeOffset_BALL)
                {
                    // 先将主推关闭
                    if(MF)
                    {
                        on_ExecuteSend(M_S);
                        MF = false;
                    }
                    // 主推已经关闭
                    else
                    {
                        // 右偏，则右移3档
                        if(VInfo.RedOffsetX > LargeOffset_BALL)
                        {
                            on_ExecuteSend(S_R3);
                            qDebug()<<"Adjusting Red X: Large Right.";
                        }
                        // 左偏，则左移3档
                        else if(VInfo.RedOffsetX < -LargeOffset_BALL)
                        {
                            on_ExecuteSend(S_L3);
                            qDebug()<<"Adjusting Red X: Large Left.";
                        }
                    }
                }
                // 红球X方向基本在视野范围中央
                else
                {
                    qDebug()<<"Adjusting Far Red X OK.";
                    // 先将侧推停止
                    if(SF)
                    {
                        SF = false;
                        on_ExecuteSend(S_S);
                    }
                    // 侧推停止后，二档前进
                    else
                    {
                        MF = true;
                        on_ExecuteSend(M_F2);
                    }
                }
                break;
            }
            case BALL_NEAR:
//            case BALL_ATTACH:
            {
                // 先将主推停止
                if(MF)
                {
                    MF = false;
                    on_ExecuteSend(M_S);
                }
                // 主推停止后，侧推调节
                else
                {
                    // X方向没有调节完毕，则调节X方向
                    if(!x_Adjust)
                    {
                        // 先调节红球X方向
                        if(qAbs(VInfo.RedOffsetX) > Proper_BALL)
                        {
                            if(VInfo.RedOffsetX > LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Red X: Right With Gear 2.";
                                on_ExecuteSend(S_R2);
                            }
                            else if(VInfo.RedOffsetX > Proper_BALL && VInfo.RedOffsetX <=LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Red X: Right With Gear 1.";
                                on_ExecuteSend(S_R1);
                            }
                            else if(VInfo.RedOffsetX < -Proper_BALL && VInfo.RedOffsetX >= -LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Red X: Left With Gear 1.";
                                on_ExecuteSend(S_L1);
                            }
                            else if(VInfo.RedOffsetX < -LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Red X: Left With Gear 2.";
                                on_ExecuteSend(S_L2);
                            }
                        }
                        // X方向达到精度范围内
                        else
                            x_Adjust=true;
                    }
                    // 红球X方向在精度范围内，开始调节Y方向
                    else
                    {
                        // Y方向没有调节完毕，则调节Y方向
                        if(!y_Adjust)
                        {
                            qDebug()<<"Adjusting Red Y.";
                            // 先将侧推关闭
                            if(SF)
                            {
                                SF = false;
                                on_ExecuteSend(S_S);
                            }
                            // 侧推关闭后，调节深度
                            else
                            {
//                                if(qAbs(VInfo.RedOffsetY) > 1.5 * Proper_BALL)
                                if(VInfo.RedOffsetY < - 1 * Proper_BALL
                                        || VInfo.RedOffsetY > 2.5 * Proper_BALL)
                                {
                                    double deltaDepth = 0.2;

                                    qDebug()<<"Red Offset Y:"<<VInfo.RedOffsetY;
                                    qDebug()<<"Set Depth: "<<(deltaDepth + depth.toFloat());

                                    // 红球偏下，航行器向下潜
                                    if(VInfo.RedOffsetY > 2.5 * Proper_BALL)
                                    {
                                        if(Ball_Com_Send_Num >= 2)
                                        {
                                            Send_Depth(depth.toFloat() + deltaDepth);
                                            Ball_Com_Send_Num = 0;
                                            qDebug()<<"Adjusting Red Y: Down.";
                                        }
                                        else
                                            Ball_Com_Send_Num ++;
                                    }
                                    // 红球偏上，航行器向上浮
                                    else
                                    {
                                        if(Ball_Com_Send_Num >= 2)
                                        {
                                            Send_Depth(depth.toFloat() - deltaDepth);
                                            Ball_Com_Send_Num = 0;
                                            qDebug()<<"Adjusting Red Y: Up.";
                                        }
                                        else
                                            Ball_Com_Send_Num ++;
                                    }
                                }
                                // Y方向达到精度范围内
                                else
                                    y_Adjust = true;
                            }
                        }
                        // X, Y都达到了精度，切换该任务执行阶段，且记录下阶段任务开始时间，以及俯仰角
                        else
                        {
                            Ball_Adjust = BALL_RED_ADJ;
                            Ball_Time.Start_Time[1] = MainTime;
                            // 为下一个计时任务准备，此处时间置0
                            Ball_Adjusted_Time = 0;

                            // X, Y调整完毕标志位归为false
                            x_Adjust = false;
                            y_Adjust = false;
                            StartPitch = pitch.toDouble();
                        }
                    }
                }
                break;
            }
            // 其余情况ATTACH，后退
            default:
                // 先将侧推停止
                if(SF)
                {
                    SF = false;
                    on_ExecuteSend(S_S);
                }
                // 侧推停止后，后退2档
                else
                {
                    MF = true;
                    on_ExecuteSend(M_B2);
                }
                break;
            }
        }
        break;
    }
    case BALL_RED_ADJ:
    {
        // 撞球未结束阶段
        if(!Ball_Crashed)
        {
            // 判断视野中是否存在红球

            // 如果在没有调整好的情况下，没有找到红球；或者红球精度不足(Y方向偏移量可适当调大)
            // 则返回红球没有调整好的阶段，BALL_RED_NOADJ
            if( (VInfo.RedHave == BALL_NOBALL && !Ball_Adjusted_OK)
                    || (qAbs(VInfo.RedOffsetX) > Proper_BALL)
                    || (VInfo.RedOffsetY < - 1 * Proper_BALL
                        || VInfo.RedOffsetY > 2.5 * Proper_BALL)
                    )
            {
                Ball_Adjust = BALL_RED_NOADJ;
            }
            // 如果找到红球
            else
            {
                // 先侧推停止
                if(SF)
                {
                    SF = false;
                    on_ExecuteSend(S_S);
                }
                else
                {
                    // 红球没有调整好的时候
                    if(!Ball_Adjusted_OK)
                    {
                        // 判断是否已经稳在了屏幕中心处，且时间没有到，则计时
                        if(qAbs(VInfo.RedOffsetX) < Proper_BALL
//                                && qAbs(VInfo.RedOffsetY) < Proper_BALL * 1.5
                                && (VInfo.RedOffsetY > - 1 * Proper_BALL
                                    && VInfo.RedOffsetY < 2.5 * Proper_BALL)
                                )
                        {
                            double time = MainTime - Ball_Time.Start_Time[1];

                            // 对准时间长于2s，且之前的撞球对准标志位为false
                            // 则将撞球对准标志位置为true，且记录当前时间
                            if(time >= 2 && Ball_Adjusted_OK == false)
                            {
                                Ball_Adjusted_OK = true;
                                Ball_Adjusted_Time = MainTime;
                            }
                        }
                        // 计时过程中没有满足X, Y精度
                        // 则说明需要调整，返回到BALL_RED_NOADJ阶段重新调整
                        else
                        {
                            Ball_Adjust = BALL_RED_NOADJ;
                        }
                    }
                    // 计时完毕，红球已经稳定在屏幕中心一段时间
                    else
                    {
                        Ball_Crashed = true;

                        // 记录对准的该时刻，识别红球的尺寸
                        Ball_Adjusted_RectSize = VInfo.RedRect.size.height > VInfo.RedRect.size.width
                                ? VInfo.RedRect.size.height : VInfo.RedRect.size.width;

                        // 根据NEAR, ATTACH的上下限构造线性函数，计算得到撞球时间
                        // NEAR_LIMIT:   最少撞2秒;
                        // ATTACH_LIMIT: 最多撞7秒;
                        double time;
                        time = 4 + (9 - 4) * (Ball_Adjusted_RectSize - BALL_ATTACH_LIMIT) / (BALL_NEAR_LIMIT - BALL_ATTACH_LIMIT);
                        Ball_Crash_Time = (int)(time + 1);

                        qDebug()<<"Ball Crash Time is: "<<Ball_Crash_Time;
                    }
                }
            }
        }
        // 撞球结束
        else
        {
            // 进入撞球结束阶段，记录该阶段的初始时间
            if(!Ball_Crashed_IsStarted)
            {
                Ball_Crashed_StartTime = MainTime;
                Ball_Crashed_IsStarted = true;
            }
            Ball_Crashed_EndTime = MainTime;

//            // 视觉上判断已经撞球，再令航行器以前进2档前进6秒，确保航行器完全撞上球
//            if(Ball_Crashed_EndTime - Ball_Crashed_StartTime <= 9)

            if(Ball_Crashed_EndTime - Ball_Crashed_StartTime <= Ball_Crash_Time)
            // 视觉上判断已经撞球，再令航行器以前进2档前进若干秒，确保航行器完全撞上球
            // 撞球时间由线性函数计算得到
            {
                qDebug()<<"Red Ball Crashing with Gear 3";
                // 此时主推前进撞球
                if(!MF)
                {
                    MF = true;
                }
                on_ExecuteSend(M_F3);
            }
            // 前进7秒已经结束，认为航行器已经撞上红球，则切换任务
            else
            {
                // 主推停止
                if(MF)
                {
                    MF = false;
                    on_ExecuteSend(M_S);
                }

                // 准备撞绿球
                if(GreenBall_ReadyOK)
                {
                    // 任务阶段切换，切换为寻找绿球
                    Ball_Adjust = BALL_GREEN_NOADJ;
                    Ball_Color = BALL_GREEN;
                }
                // 只撞红球，红球之后直接寻找导引路径
                else
                {
                    // 直接切换为撞球结束寻线
                    Ball_Adjust = BALL_FINISH;
                }


//                // 大水池调试模式，任务阶段直接切换为撞球结束寻线
//                Ball_Adjust = BALL_FINISH;

                // 该任务标志位回归，重新初始化
                Ball_Crashed = false;
                Ball_Crashed_IsStarted = false;
                Ball_Com_Send_Num = 0;
//                Ball_Adjusted_OK = false;
                Ball_Adjusted_RectSize = BALL_NEAR_LIMIT;
                Ball_Crash_Time = 9;
            }
        }
        break;
    }
    case BALL_GREEN_NOADJ:
    {
        // 对准标志位置false
        Ball_Adjusted_OK = false;

        // 判断视野中是否存在绿球

        // 视野中没有绿球
        if(VInfo.GreenHave == BALL_NOBALL)
        {
            // 如果没有绿球，则寻找红球

            // 如果视野中没有红球，则后退3档
            if(VInfo.RedHave == BALL_NOBALL)
            {
                if(SF)
                    on_ExecuteSend(S_S);
                else
                    on_ExecuteSend(M_B3);
            }
            // 找到了红球，根据距离状态作出相应动作
            else
            {
//                // 若当前距离状态为NEAR, ATTACH，则后退2档
//                if(VInfo.ballsstate != BALL_FAR)
//                    on_ExecuteSend(M_B2);
                // 若当前距离状态为 ATTACH，则后退3档
                if(VInfo.ballsstate == BALL_ATTACH)
                    on_ExecuteSend(M_B3);
                // 当前距离状态为FAR
                else
                {
                    // 先关闭主推
                    if(MF)
                    {
                        MF = false;
                        on_ExecuteSend(M_S);
                    }
                    else
                    {
                        // A区顺序RGY，此时应该向右移动
                        if(Mission_Part == 'A'
                                || Mission_Part == 'a')
                        {
                            SF = true;
                            // 右移2档
                            on_ExecuteSend(S_R2);
                        }
                        // 其他区(BCD)顺序RGY，此时应该向左移动
                        else
                        {
                            SF = true;
                            // 右移2档
                            on_ExecuteSend(S_L2);
                        }
                    }
                }
            }
        }
        // 视野中有绿球，根据不同距离等级进行动作
        else
        {
            switch(VInfo.ballsstate)
            {
            case BALL_FAR:
            {
                // 绿球X方向偏移量过大
                if(qAbs(VInfo.GreenOffsetX) > LargeOffset_BALL)
                {
                    // 先将主推关闭
                    if(MF)
                    {
                        on_ExecuteSend(M_S);
                        MF = false;
                    }
                    // 若主推已经关闭
                    else
                    {
                        // 右偏，则右移3档
                        if(VInfo.GreenOffsetX > LargeOffset_BALL)
                        {
                            on_ExecuteSend(S_R3);
                            qDebug()<<"Adjusting Green X: Large Right.";
                        }
                        // 左偏，则左移3档
                        else if(VInfo.GreenOffsetX < -LargeOffset_BALL)
                        {
                            on_ExecuteSend(S_L3);
                            qDebug()<<"Adjusting Green X: Large Left.";
                        }
                    }
                }
                // 绿球X方向基本在视野范围中央
                else
                {
                    qDebug()<<"Adjusting Far Green X OK.";
                    // 先将侧推停止
                    if(SF)
                    {
                        SF = false;
                        on_ExecuteSend(S_S);
                    }
                    // 侧推停止后，前进2档
                    else
                    {
                        MF = true;
                        on_ExecuteSend(M_F2);
                    }
                }
                break;
            }
            case BALL_NEAR:
//            case BALL_ATTACH:
            {
                // 先将主推停止
                if(MF)
                {
                    MF = false;
                    on_ExecuteSend(M_S);
                }
                // 主推停止后，侧推调节
                else
                {
                    // X方向没有调节完毕，则调节X方向
                    if(!x_Adjust)
                    {
                        // 先调节绿球X方向
                        if(qAbs(VInfo.GreenOffsetX) > Proper_BALL)
                        {
                            if(VInfo.GreenOffsetX > LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Green X: Right With Gear 2.";
                                on_ExecuteSend(S_R2);
                            }
                            else if(VInfo.GreenOffsetX > Proper_BALL && VInfo.GreenOffsetX <=LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Green X: Right With Gear 1.";
                                on_ExecuteSend(S_R1);
                            }
                            else if(VInfo.GreenOffsetX < -Proper_BALL && VInfo.GreenOffsetX >= -LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Green X: Left With Gear 1.";
                                on_ExecuteSend(S_L1);
                            }
                            else if(VInfo.GreenOffsetX < -LargeOffset_BALL)
                            {
                                qDebug()<<"Adjusting Green X: Left With Gear 2.";
                                on_ExecuteSend(S_L2);
                            }
                        }
                        // X方向达到精度范围内
                        else
                            x_Adjust = true;
                    }

                    // 绿球X方向在精度范围内，开始调节Y方向
                    else
                    {
                        // Y方向没有调节完毕，则调节Y方向
                        if(!y_Adjust)
                        {
                            qDebug()<<"Adjusting Green Y.";
                            // 先将侧推关闭
                            if(SF)
                            {
                                SF = false;
                                on_ExecuteSend(S_S);
                            }
                            // 侧推关闭后，调节深度
                            else
                            {
                                if(VInfo.GreenOffsetY < -1 * Proper_BALL
                                        || VInfo.GreenOffsetY > 2.5 * Proper_BALL)
                                {
                                    double deltaDepth = 0.2;

                                    qDebug()<<"Green Offset Y:"<<VInfo.GreenOffsetY;
                                    qDebug()<<"Set Depth: "<<(deltaDepth + depth.toFloat());

                                    // 绿球偏下，航行器向下潜
                                    if(VInfo.GreenOffsetY > 2.5 * Proper_BALL)
                                    {
                                        if(Ball_Com_Send_Num >= 2)
                                        {
                                            Send_Depth(depth.toFloat() + deltaDepth);
                                            Ball_Com_Send_Num = 0;
                                            qDebug()<<"Adjusting Green Y: Down.";
                                        }
                                        else
                                            Ball_Com_Send_Num++;
                                    }
                                    // 绿球偏下，航行器向上浮
                                    else
                                    {
                                        if(Ball_Com_Send_Num >= 2)
                                        {
                                            Send_Depth(depth.toFloat() - deltaDepth);
                                            Ball_Com_Send_Num = 0;
                                            qDebug()<<"Adjusting Green Y: Up.";
                                        }
                                        else
                                            Ball_Com_Send_Num++;
                                    }
                                }
                                // Y方向达到精度范围内
                                else
                                    y_Adjust = true;
                            }
                        }
                        // X, Y都达到了精度，切换该任务执行阶段，且记录下阶段任务开始时间，以及俯仰角
                        else
                        {
                            Ball_Adjust = BALL_GREEN_ADJ;
                            Ball_Time.Start_Time[2] = MainTime;

                            // 为下一个计时任务准备，此处时间置0
                            Ball_Adjusted_Time = 0;

                            // X, Y调整完毕标志位归为false
                            x_Adjust = false;
                            y_Adjust = false;
                            StartPitch = pitch.toDouble();

                        }
                    }
                }
                break;
            }
            // 其余情况ATTACHED，后退
            default:
                // 先将侧推停止
                if(SF)
                {
                    SF = false;
                    on_ExecuteSend(S_S);
                }
                // 侧推停止后，后退2档
                else
                {
                    MF = false;
                    on_ExecuteSend(M_B2);
                }
                break;
            }

        }
        break;
    }
    case BALL_GREEN_ADJ:
    {
        // 撞球未结束阶段
        if(!Ball_Crashed)
        {
            // 判断视野中是否存在绿球

            // 如果在没有调整好的情况下，没有找到绿球；或者绿球精度不足(Y方向偏移量可适当调大)
            // 则返回绿球没有调整好的阶段，BALL_GREEN_NOADJ
            if( (VInfo.GreenHave == BALL_NOBALL && !Ball_Adjusted_OK)
                    || (qAbs(VInfo.GreenOffsetX) > Proper_BALL)
//                    && qAbs(VInfo.GreenOffsetY) > Proper_BALL * 1.5
                    || (VInfo.GreenOffsetY < -1 * Proper_BALL
                        || VInfo.GreenOffsetY > 2.5 * Proper_BALL)
                    )
            {
                Ball_Adjust = BALL_GREEN_NOADJ;
            }
            // 如果找到绿球
            else
            {
                // 先侧推停止
                if(SF)
                {
                    SF = false;
                    on_ExecuteSend(S_S);
                }
                else
                {
                    // 绿球没有调整好的时候
                    if(!Ball_Adjusted_OK)
                    {
                        // 判断是否已经稳在了屏幕中心处，且时间没有到，则计时
                        if(qAbs(VInfo.GreenOffsetX) < Proper_BALL
//                                && qAbs(VInfo.GreenOffsetY) < Proper_BALL * 1.5
                                && (VInfo.GreenOffsetY > -1 * Proper_BALL
                                    && VInfo.GreenOffsetY < 2.5 * Proper_BALL)
                                )
                        {
                            double time = MainTime - Ball_Time.Start_Time[2];

                            // 对准时间长于2s，且之前的撞球对准标志位为false
                            // 则将撞球对准标志位置为true，且记录当前时间
                            if(time >= 2 && Ball_Adjusted_OK == false)
                            {
                                Ball_Adjusted_OK = true;
                                Ball_Adjusted_Time = MainTime;
                            }
                        }
                        // 计时过程中没有满足X, Y精度
                        // 则说明需要调整，返回到BALL_GREEN_NOADJ阶段重新调整
                        else
                        {
                            Ball_Adjust = BALL_GREEN_NOADJ;
                        }
                    }
                    // 计时完毕，绿球已经稳定在屏幕中心一段时间
                    else
                    {
                        Ball_Crashed = true;

                        // 记录对准的该时刻，识别绿球的尺寸
                        Ball_Adjusted_RectSize = VInfo.GreenRect.size.height > VInfo.GreenRect.size.width
                                ? VInfo.GreenRect.size.height : VInfo.GreenRect.size.width;

                        // 根据NEAR, ATTACH的上下限构造线性函数，计算得到撞球时间
                        // NEAR_LIMIT:   最少撞4秒;
                        // ATTACH_LIMIT: 最多撞9秒;
                        double time;
                        time = 4 + (9 - 4) * (Ball_Adjusted_RectSize - BALL_ATTACH_LIMIT) / (BALL_NEAR_LIMIT - BALL_ATTACH_LIMIT);
                        Ball_Crash_Time = (int)(time + 1);

                        qDebug()<<"Ball Crash Time is: "<<Ball_Crash_Time;
                    }
                }
            }
        }
        else
        {
            // 进入撞球结束阶段，记录该阶段的初始时间
            if(!Ball_Crashed_IsStarted)
            {
                Ball_Crashed_StartTime = MainTime;
                Ball_Crashed_IsStarted = true;
            }
            Ball_Crashed_EndTime = MainTime;

//            // 视觉上判断已经撞球，再令航行器以前进2档前进6秒，确保航行器完全撞上球
//            if(Ball_Crashed_EndTime - Ball_Crashed_StartTime <= 9)

            if(Ball_Crashed_EndTime - Ball_Crashed_StartTime <= Ball_Crash_Time)
            // 视觉上判断已经撞球，再令航行器以前进3档前进若干秒，确保航行器完全撞上球
            // 撞球时间由线性函数计算得到
            {
                qDebug()<<"Green Ball Crashing with Gear 3";
                // 此时主推前进撞球
                if(!MF)
                {
                    MF = true;
                }
                on_ExecuteSend(M_F3);
            }
            // 前进2秒已经结束，认为航行器已经撞上绿球，则切换任务
            else
            {
                // 主推停止
                if(MF)
                {
                    MF = false;
                    on_ExecuteSend(M_S);
                }
                // 任务阶段切换，切换为撞球结束寻线阶段
                Ball_Adjust = BALL_FINISH;

                // 该任务标志位回归，重新初始化
                Ball_Crashed = false;
                Ball_Crashed_IsStarted = false;
                Ball_Com_Send_Num = 0;
                Ball_Adjusted_OK = false;

                Ball_Adjusted_RectSize = BALL_NEAR_LIMIT;
                Ball_Crash_Time = 9;
            }
        }

        break;
    }
    case BALL_FINISH:
    {
        // 没有发现线的情况
        if(!Ball_FindLine_OK)
        {
            // 没有找到线的时候，先停止主推
            if(MF && !Ball_Finish_FindingLine)
            {
                MF = false;
                on_ExecuteSend(M_S);
            }
            // 主推停止后
            else
            {
                // 深度调整完毕标志位为false的时候，调整深度
                if(!DepthOK)
                {
                    // 如果当前深度没有到达航行器整体能够越过绿球的深度，则发送该深度
                    if(qAbs(depth.toDouble() - BallFinishedDepth) > Proper_Depth)
                    {
                        // 主推侧推停止
                        if(MF)
                            on_ExecuteSend(M_S);
                        else if(SF)
                            on_ExecuteSend(S_S);
                        else
                            // 航行器上浮至绿球上方的高度
                            Send_Depth(BallFinishedDepth);
                    }
                    // 如果当前深度到达了该深度的精度范围之内，则调整角度
                    else
                    {
                        DepthOK = true;
                        qDebug()<<"Above Ball Depth OK.";
                    }
                }
                // 深度调整完毕，调整角度
                else
                {
                    if(!AngleOK)
                    {
                        // 经过绿球撞完才进入结束阶段
                        if(GreenBall_ReadyOK)
                        {
                            // 绿球撞完，必在中间
                            Angle_Green_Line = 0;
                        }
                        // 红球撞完之后，进入结束阶段
                        else
                        {
                            // A区顺序RGY
                            if(Mission_Part == 'A' || Mission_Part == 'a')
                                Angle_Green_Line = 25;
                            // BCD区顺序YGR
                            else
                                Angle_Green_Line = -25;
                        }

                        double angle = Scuttle_Angle + Angle_Green_Line;


//                        // 正常模式，绿球撞完之后，减角度
//                        double angle = Scuttle_Angle - Angle_Green_Line;

////                        // 大水池调试模式，红球撞完之后，加角度
////                        double angle = Scuttle_Angle + Angle_Green_Line;

                        if(angle > 360) angle = angle - 360;
                        if(angle < 0)   angle = 360 - qAbs(angle);

                        // 如果当前角度没有到达导引路径--绿球的角度，则发送该角度
                        if(qAbs(yaw.toDouble() - angle) > Proper_Angle)
                        {
                            Ball_BallLineAngle_Time = 0;
                            if(!Ball_SendBallLineAngle_OK)
                            {
                                // 发送绿球到导引路径的角度
                                Send_Angle(angle);
                                Ball_SendBallLineAngle_OK = true;
                                qDebug()<<"Have Send Ball - Line Angle.";
                            }
                            else
                                qDebug()<<"Ball - Line Angle is Adjusting...";
                        }
                        else
                        {
                            Ball_BallLineAngle_Time++;
                            if(Ball_BallLineAngle_Time >= 2)
                            {
                                AngleOK = true;
                                qDebug()<<"Ball - Line Angle Adjusting Finished.";
                            }
                            else
                                qDebug()<<"Ball - Line Angle Adjust Time: "<<Ball_BallLineAngle_Time;
                        }
                    }
                    // 如果到达了该角度的精度范围，则前进
                    else
                    {
                        // 停止侧推
                        if(SF)
                        {
                            SF = false;
                            on_ExecuteSend(S_S);
                            qDebug()<<"Above Ball Angle OK.";
                        }
                        else
                        {
                            // 俯视摄像头没有看到线，则2档前进，且标志位Ball_Finish_FindingLine置为true
                            if(VInfo.linestate == GUIDE_NOLINE || VInfo.linestate == GUIDE_POINT)
                            {
                                on_ExecuteSend(M_F2);
                                qDebug()<<"Gear 2 GO";
                                Ball_Finish_FindingLine = true;
                            }
                            else
                            {
                                on_ExecuteSend(M_S);
                                AngleOK=false;
                                DepthOK=false;
                                Ball_FindLine_OK = true;
                                qDebug()<<"Above Ball Finding Line OK.";
                            }
                        }
                    }
                }
            }
        }
        else
        {
            //*******************************
            //       几乎照搬嘻嘻程序。。。
            //*******************************

            // 已经调到了正上方
            if(Ball_AboveLine_OK)
            {
                // 若深度没有调整好
                if(!DepthOK)
                {
                    // 给下位机发送风格过门深度值
                    Send_Depth(Navigate_Depth);
                    // 达到目标深度，深度调整完成标志位true
                    if(qAbs(depth.toFloat() - Navigate_Depth) < Proper_Depth)
                        DepthOK = true;
                }
                // 深度已经调整好
                else
                {
                    if(!OffCenterOK)
                    {
                        // 主推停止
                        if(MF)
                            on_ExecuteSend(M_S);
                        if(qAbs(VInfo.LineOffsetX > Line_Proper_OffCenter-5))
                        {
                            // 右偏，则右移2档
                            if(VInfo.LineOffsetX > Line_Proper_OffCenter-5)
                                on_ExecuteSend(S_R2);
                            // 左偏，则左移2档
                            else
                                on_ExecuteSend(S_L2);
                        }
                        else
                        {
                            if(SF)
                                on_ExecuteSend(S_S);
                            OffCenterOK=true;
                        }
                    } // 角度已经调整好了
                    // 角度没有调整好
                    else
                    {
                        if(!AngleOK)
                        {
                            if(count_time==0)
                            {
                                Navigate_Angle = yaw.toFloat() - float(VInfo.LineAngle);
                                if(Navigate_Angle<0)
                                    Navigate_Angle=Navigate_Angle+360;
                                if(Navigate_Angle>360)
                                    Navigate_Angle=Navigate_Angle-360;
                                if((VInfo.linestate == GUIDE_LINE))
                                    Send_Angle(Navigate_Angle);// 发送角度值
                                count_time++;
                            }
                            if(qAbs(Navigate_Angle - yaw.toFloat())<Proper_Angle)
                            {
                                count_time++;
                                if(count_time>=5)
                                {
                                    if(qAbs(VInfo.LineAngle) < Proper_Angle)
                                        AngleOK = true;
                                    else
                                    {
                                        Navigate_Angle = yaw.toFloat() - float(VInfo.LineAngle);
                                        if(Navigate_Angle<0)
                                            Navigate_Angle=Navigate_Angle+360;
                                        if(Navigate_Angle>360)
                                            Navigate_Angle=Navigate_Angle-360;
                                        if((VInfo.linestate == GUIDE_LINE))
                                            Send_Angle(Navigate_Angle);// 发送角度值
                                        count_time=1;
                                    }
                                }

                            }
                        }
                        else
                        {
                            // 是否过风格门
                            if(StyleDoor_ReadyOK)
                            {
                                // 撞球任务[2]变为完成状态
                                missionflag[2] = 1;
                                // 标志位重置
                                DepthOK = false;
                                AngleOK = false;
                                OffCenterOK = false;
                                count_time=0;
                                on_ExecuteSend(M_F3);
                                Ball_FindLine_OK = false;
                                Ball_AboveLine_OK = false;

                                // 记录时间门开始时间
                                Navigate_WholeTime = MainTime;
                            }
                            else
                            {
                                // 直接找水声
                                iamtask = BURY_TREASURE;
                                Oct_StartTime = MainTime;

                                // 标志位重置
                                DepthOK = false;
                                AngleOK = false;
                                OffCenterOK = false;
                                count_time=0;
                                on_ExecuteSend(M_F3);
                                Ball_FindLine_OK = false;
                                Ball_AboveLine_OK = false;
                            }
                        }
                    }
                }
            }
            // 还没有调到正上方
            else
            {
                // 视觉分析看到了线，而且判断航行器已经在线的上方
                if((VInfo.linestate == GUIDE_LINE )&& qAbs(VInfo.LineOffsetX) > Line_Proper_OffCenter)
                {
                    // 主推停止
                    if(MF)
                    {
                        on_ExecuteSend(M_S);
                        MF=false;
                    }
                    else
                    {
                        // 右偏，则右移2档
                        if(VInfo.LineOffsetX > Line_Proper_OffCenter)
                            on_ExecuteSend(S_R2);
                        // 左偏，则左移2档
                        else
                            on_ExecuteSend(S_L2);
                        SF=true;
                    }
                }
                // 视觉分析看到了线，且判断导引路径在俯视摄像头中X方向偏离过大，则调节
                else
                {
                    if((VInfo.linestate == GUIDE_LINE) && (VInfo.IsAboveLine==true))
                    {
                        if(SF)
                            on_ExecuteSend(S_S);// 此时推停止
                        if(!Ball_AboveLine_OK)
                        {
                            on_ExecuteSend(M_S);
                            MF = false;
                        }

                        // 正上方标志位置为true
                        Ball_AboveLine_OK = true;
                    }
                    else
                    {
                        // 关闭侧推
                        if(SF)
                            on_ExecuteSend(S_S);// 侧推关闭完毕，则启动主推
                        else
                            on_ExecuteSend(M_F3);
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

void AuvAutoControl::M_Navigate_channel()
{
    if(DEBUGSWITCH)
    {
        if(mission_count==3)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Navigate_channel_Mission"<< endl;
            mission_count++;
            mission_time.Start_Time[3]=MainTime;
            ui->groupBox_3->setStyleSheet("color:Red;background:yellow");
        }
    }

    int NavigateTime = MainTime - Navigate_WholeTime;

    // 程序识别到NEAR，用于判断程序用十几秒的时间，能否看到时间门
    if(VInfo.StyleDoorState == STYLE_NEAR
            && !Navigate_NEAROK)
        Navigate_NEAROK = true;

    // 程序运行25s，还没有到达过NEAR状态，则基本已经找不到时间门，直接切换为水声任务
    if(NavigateTime > 25 && !Navigate_NEAROK)
    {
        Navigate_EndEarly = true;
    }

    // 程序运行超过120s，强制退出该任务，切换到水声任务
    if(NavigateTime > 120)
    {
        Navigate_EndEarly = true;
    }

    // 强制跳出该任务
    if(Navigate_EndEarly)
    {
        if(MF)
            on_ExecuteSend(M_S);
        else if(SF)
            on_ExecuteSend(S_S);
        else
        {
            if(qAbs(depth.toDouble() - Oct_Depth) > Proper_Depth)
                Send_Depth(Oct_Depth);
            else
            {
                double DoublePingerAngle;

                if(Mission_Part == 'A' || Mission_Part == 'a'
                        || Mission_Part == 'C' || Mission_Part == 'c')
                    DoublePingerAngle = Navigate_Angle + 30;
                else
                    DoublePingerAngle = Navigate_Angle - 30;

                if(DoublePingerAngle > 360)
                    DoublePingerAngle = DoublePingerAngle - 360;
                else if(DoublePingerAngle < 0)
                    DoublePingerAngle = DoublePingerAngle + 360;

                if(qAbs(yaw.toDouble() - DoublePingerAngle) > Proper_Angle)
                    Send_Angle(DoublePingerAngle);
                else
                    iamtask = RANDOMAC;
            }

//            else
//            {
//                Oct_StartTime = MainTime;
//                iamtask = BURY_TREASURE;
//            }
        }
    }
    // 正常执行程序
    else
    {
        // 门距离状态为远，且没有稳定在WHOLEDOOR的时候，3档前进
        if(VInfo.StyleDoorState == STYLE_FAR
                && !OffCenterOK)
            on_ExecuteSend(M_F3);
        else
        {
            // 主推前进，且180度没有调整好，且没有用2档接近风格门，则主推停止
            if((MF)&&(!DoorAD)&&(!MF2GO))
            {
                on_ExecuteSend(M_S);
            }

            // 风格门没有稳定在WHOLEDOOR的时候
            if(!OffCenterOK)
            {
                // 如果门状态为WHOLEDOOR，开始计时
                if(VInfo.StyleDoorSeenState == STYLE_WHOLEDOOR)
                {
                    hold_time++;
                    // 侧推停止
                    if(SF)
                        on_ExecuteSend(S_S);
                    // WHOLEDOOR状态保持3秒以上，标志位置为真
                    if(hold_time >= 3)
                    {
                        OffCenterOK = true;
                    }
                }
                // 门状态不为WHOLEDOOR
                else
                {
                    // 保持时间清零
                    hold_time=0;
                    if(MF)
                        on_ExecuteSend(M_S);
                    else
                    {
                        // 左状态，右移
                        if(VInfo.StyleDoorSeenState==STYLE_HALFLEFT)
                        {
                            on_ExecuteSend(S_R2);
                        }
                        else
                        {
                            // 右状态，左移
                            if(VInfo.StyleDoorSeenState==STYLE_HALFRIGHT)
                            {
                                on_ExecuteSend(S_L2);
                            }
                            else
                            {
                                // 底部状态，向上升20cm
                                if(VInfo.StyleDoorSeenState==STYLE_BOTTOMONLY)
                                {
                                    if(SF)
                                        on_ExecuteSend(S_S);
                                    float temp=depth.toFloat() - 0.2;
                                    Send_Depth(temp);
                                }
                            }
                        }
                    }
                }
            }
            // 已经在WHOLEDOOR稳定了3秒
            else
            {
                // 调整X方向偏移量
                if(!Navigate_channel_X)
                {
                    // X方向偏移量过大
                    if(qAbs(VInfo.StyleCenterOffsetX) > Proper_OffCenter)
                    {
                        // 右偏，右移二挡
                        if(VInfo.StyleCenterOffsetX > Proper_OffCenter)
                            on_ExecuteSend(S_R2); //侧推右移2挡
                        // 左偏，左移二挡
                        else
                            on_ExecuteSend(S_L2); //侧推左移2挡
                    }
                    // X方向偏移量在准许范围内
                    else
                    {
                        // 停止侧推
                        if(SF)
                            on_ExecuteSend(S_S);
                        keep_time++;
                        // 保持1秒以上，则认为X方向调整完毕
                        if(keep_time >= 1)
                            Navigate_channel_X = true;
                        qDebug()<<"Navigate Offset X OK.";
                    }
                }
                else
                {
                    // X方向偏移量调整完毕，开始调整Y方向
                    if(!Navigate_channel_Y)
                    {
                        // Y方向的偏移量大于精度值，则进行调整
                        if(qAbs(VInfo.StyleCenterOffsetY) > Proper_OffCenter)
                        {
                            // 风格门偏下，则航行器下沉0.2m
                            if(VInfo.StyleCenterOffsetY > Proper_OffCenter)
                            {
                                float temp=depth.toFloat() + 0.2;
                                Send_Depth(temp);
                            }
                            // 风格门偏上，则航行器上浮0.2m
                            else
                            {
                                float temp=depth.toFloat() - 0.2;
                                Send_Depth(temp);
                            }
                        }
                        // Y方向偏移量在准许范围内，则认为Y方向调整完毕
                        else
                        {
                            Navigate_channel_Y=true;
                        }
                    }
                    // X, Y方向都调整完毕
                    else
                    {
                        // 门状态为WHOLEDOOR，且没有做180度转向动作
                        if(VInfo.StyleDoorSeenState == STYLE_WHOLEDOOR
                                && Door_adjust == 0)
                        {
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                on_ExecuteSend(M_F3);
                                Door_adjust = 1;
                                DoorAD = true;
                                Navigate_GoAheadStartTime = MainTime;
                            }
                        }
                    }
                }
            }
        }

        // 平常调试：已经执行了180度转向命令
        // 半决赛阶段更改：已经基本对准，马上前进3档，15s之后切换任务
        if(Door_adjust==1)
        {
            int GoAheadTime = MainTime - Navigate_GoAheadStartTime;

            // 直行前进15s，拼运气过去吧……
            if(GoAheadTime < 15)
            {
                if(SF)
                    on_ExecuteSend(S_S);
                else
                    on_ExecuteSend(M_F3);
            }
            else
            {
                if(MF)
                    on_ExecuteSend(M_S);
                else if(SF)
                    on_ExecuteSend(S_S);
                else
                {
                    if(qAbs(depth.toDouble() - Oct_Depth) > Proper_Depth)
                        Send_Depth(Oct_Depth);
                    else
                    {
                        double DoublePingerAngle;

                        if(Mission_Part == 'A' || Mission_Part == 'a'
                                || Mission_Part == 'C' || Mission_Part == 'c')
                            DoublePingerAngle = Navigate_Angle + 30;
                        else
                            DoublePingerAngle = Navigate_Angle - 30;

                        if(DoublePingerAngle > 360)
                            DoublePingerAngle = DoublePingerAngle - 360;
                        else if(DoublePingerAngle < 0)
                            DoublePingerAngle = DoublePingerAngle + 360;

                        if(qAbs(yaw.toDouble() - DoublePingerAngle) > Proper_Angle)
                            Send_Angle(DoublePingerAngle);
                        else
                            iamtask = RANDOMAC;
                    }
                }
            }
        }
    }
}


void AuvAutoControl::M_Weight_anchor()
{
    if(DEBUGSWITCH)
    {
        if(mission_count==5)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Weight_anchor_Mission"<< endl;
            mission_count++;
        }
    }

    // 抛锚任务时间限制
    int Anchor_Time = MainTime - Anchor_StartTime;

    if(Anchor_Time > 150)
    {
        if(!Anchor_LEFT_Launched)
        {
            qDebug()<<"Anchor Time is UP & Launch LEFT Object!";

            // 投放左标志物
            on_ExecuteSend(ANCHOR_LA);
            Anchor_LEFT_Launched = true;
        }
        else
        {
            if(!Anchor_RIGHT_Launched)
            {
                qDebug()<<"Anchor Time is UP & Launch RIGHT Object!";

                // 投放右标志物
                on_ExecuteSend(ANCHOR_RA);
                Anchor_RIGHT_Launched = true;
            }
            // 切换任务
            else
            {
                // 初始水声任务为八边形，则此时已经结束所有程序
                if(Random_Oct)
                {
                    if(MF)
                        on_ExecuteSend(M_S);
                    else if(SF)
                        on_ExecuteSend(S_S);
                    else
                        iamtask = BODYBACK;
                }
                // 初始水声任务为投放标志物，则此时应该导向八边形
                else
                {
                    if(qAbs(depth.toDouble() - Oct_Depth) > Proper_Depth)
                        Send_Depth(Oct_Depth);
                    else
                    {
                        if(qAbs(yaw.toDouble() - Oct_StartAngle) > Proper_Angle)
                            Send_Angle(Oct_StartAngle);
                        else
                        {
                            // 所有标志位置false
                            Anchor_Init_AdjustX_OK = false;
                            Anchor_Init_AdjustY_OK = false;
                            Anchor_Adj_AdjustX_OK = false;
                            Anchor_Adj_AdjustY_OK = false;
                            Anchor_Adj_DepthOK = false;
                            Anchor_Adj_AngleOK = false;
                            Anchor_LEFT_AdjustX_OK = false;
                            Anchor_LEFT_AdjustY_OK = false;
                            Anchor_RIGHT_AdjustX_OK = false;
                            Anchor_RIGHT_AdjustY_OK = false;
                            Anchor_LEFT_Launched = false;
                            Anchor_RIGHT_Launched = false;

                            Oct_StartTime = MainTime;
                            iamtask = BURY_TREASURE;
                        }
                    }
                }
            }
        }
    }
    else
    {
        switch(Anchor_Adjust)
        {
        // 初始阶段
        case ANCHOR_INIT:
        {
            // 如果找到了目标箱子，或者找到了盖子
            if(VInfo.AimHole.HoleHave || VInfo.CoverHole.HoleHave)
            {
                // 找到了目标箱子
                if(VInfo.AimHole.HoleHave)
                {
                    // 计算目标箱子与屏幕中心之间的X, Y偏移量
                    Anchor_Init_OffsetX = VInfo.AimHole.CenterTarget.TarOffsetX;
                    Anchor_Init_OffsetY = VInfo.AimHole.CenterTarget.TarOffsetY;
                }
                // 没有找到目标箱子，只找到了目标盒子
                else if(VInfo.CoverHole.HoleHave)
                {
                    // 计算盖子与屏幕中心之间的X, Y偏移量
                    Anchor_Init_OffsetX = VInfo.CoverHole.CenterTarget.TarOffsetX;
                    Anchor_Init_OffsetY = VInfo.CoverHole.CenterTarget.TarOffsetY;
                }
                // 两者都没有找到（异常状况）
                else
                {
                    ;
    //                Anchor_Init_OffsetX = Anchor_Offset_Default;
    //                Anchor_Init_OffsetY = Anchor_Offset_Default;
                }

                // X方向没有调节完毕，则调节X方向
                if(!Anchor_Init_AdjustX_OK)
                {
                    qDebug()<<"Adjusting Anchor Init X...";
                    // 调整X方向时，先将主推关闭
                    if(MF)
                        on_ExecuteSend(M_S);
                    else
                    {
                        // （异常状况输出）
                        if(Anchor_Init_OffsetX == Anchor_Offset_Default)
                        {
                            qDebug()<<"Exception of Init OffsetX...";
                        }
                        // （正常状况）
                        else
                        {
                            if(qAbs(Anchor_Init_OffsetX) > ProperOffset_ANCHOR)
                            {
                                // 右偏，则右移2档
                                if(Anchor_Init_OffsetX > ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor X: Right With Gear 2";
                                    on_ExecuteSend(S_R2);
                                }
                                // 左偏，则左移2档
                                else if(Anchor_Init_OffsetX < -ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor X: Left With Gear 2";
                                    on_ExecuteSend(S_L2);
                                }
                            }
                            // X方向达到精度范围内
                            else
                                Anchor_Init_AdjustX_OK = true;
                        }
                    }
                }
                // X方向已经在精度范围内，开始调节Y方向
                else
                {
                    qDebug()<<"Adjusting Anchor Init Y.";

                    if(!Anchor_Init_AdjustY_OK)
                    {
                        // 调整Y方向时，先将侧推关闭
                        if(SF)
                            on_ExecuteSend(S_S);
                        else
                        {
                            // （异常状况的处理）
                            if(Anchor_Init_OffsetY == Anchor_Offset_Default)
                            {
                                qDebug()<<"Exception of Init OffsetY...";
                            }
                            // （正常状况）
                            else
                            {
                                if(qAbs(Anchor_Init_OffsetY) > LargeOffset_ANCHOR)
                                {
                                    // 下偏，则后退2档
                                    if(Anchor_Init_OffsetY > LargeOffset_ANCHOR)
                                    {
                                        qDebug()<<"Init Adjusting Anchor Y: Back With Gear 2";
                                        on_ExecuteSend(M_B2);
                                    }
                                    // 上偏，则前进2档
                                    else if(Anchor_Init_OffsetY < -LargeOffset_ANCHOR)
                                    {
                                        qDebug()<<"Init Adjusting Anchor Y: Forward With Gear 2";
                                        on_ExecuteSend(M_F2);
                                    }
                                }
                                // Y方向达到精度范围内
                                else
                                    Anchor_Init_AdjustY_OK = true;
                            }
                        }
                    }
                    // X, Y都达到了精度，切换该任务执行阶段，记录该阶段结束时间，并将标志位重置
                    else
                    {
                        Anchor_Adjust = ANCHOR_ADJ;
                        Anchor_Init_OK_Time = MainTime;
                        Anchor_Depth_ComSendNum = 0;

                        // X, Y调整完毕标志位归为false
                        Anchor_Init_AdjustX_OK = false;
                        Anchor_Init_AdjustY_OK = false;
                    }
                }
            }
            // 如果目标箱子和盖子都没有找到
            else
            {
                // 进入该阶段的初始阶段丢失了箱子或盖子，或者出现了异常状况
                // 则上浮扩大视野范围，进而寻找目标
                if(Anchor_Init_OffsetX == Anchor_Offset_Default
                        && Anchor_Init_OffsetY == Anchor_Offset_Default)
                {
                    // 上浮之前，先停止主推，停止侧推
                    if(MF)
                        on_ExecuteSend(M_S);
                    else if(SF)
                        on_ExecuteSend(S_S);
                    // 停止主推与侧推，开始上浮
                    else
                    {
                        double deltaDepth = 0.2;

                        if(Anchor_Depth_ComSendNum >= 2)
                        {
                            Send_Depth(depth.toFloat() - deltaDepth);
                            Anchor_Depth_ComSendNum = 0;
                            qDebug()<<"Adjusting Anchor Depth: Up.";
                        }
                        else
                            Anchor_Depth_ComSendNum++;
                    }
                }
                // 在任务阶段中丢失箱子或盖子，但之前的偏移量数据有记录
                // 则根据记录下来的OffsetX, OffsetY调整
                else
                {
                    // X, Y方向偏移量都过大，则上浮寻找目标
                    if(qAbs(Anchor_Init_OffsetX) > LargeOffset_ANCHOR
                            && qAbs(Anchor_Init_OffsetY) > LargeOffset_ANCHOR)
                    {
                        double deltaDepth = 0.1;

                        if(Anchor_Depth_ComSendNum >= 2)
                        {
                            Send_Depth(depth.toFloat() + deltaDepth);
                            Anchor_Depth_ComSendNum = 0;
                            qDebug()<<"Adjusting Anchor Depth: Up.";
                        }
                        else
                            Anchor_Depth_ComSendNum++;
                    }
                    // X, Y方向中某方向偏移量较大，则主要调整该方向
                    else
                    {
                        // X方向偏移量大，主要调整X方向
                        if(qAbs(Anchor_Init_OffsetX) > qAbs(Anchor_Init_OffsetY))
                        {
                            qDebug()<<"Adjusting Anchor Init X With No Aim.";
                            // 调整X方向时，先将主推关闭
                            if(MF)
                                on_ExecuteSend(M_S);
                            else
                            {
                                // 右偏，则右移2档
                                if(Anchor_Init_OffsetX > LargeOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor X With No Aim: Right With Gear 2";
                                    on_ExecuteSend(S_R2);
                                }
                                // 左偏，则左移2档
                                else if(Anchor_Init_OffsetX < -LargeOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor X With No Aim: Left With Gear 2";
                                    on_ExecuteSend(S_L2);
                                }
                            }
                        }
                        // Y方向偏移量大，主要调整Y方向
                        else
                        {
                            qDebug()<<"Adjusting Anchor Init Y With No Aim.";
                            // 调整Y方向时，先将侧推关闭
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                // 下偏，则后退2档
                                if(Anchor_Init_OffsetY > LargeOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor Y With No Aim: Back With Gear 2";
                                    on_ExecuteSend(M_B2);
                                }
                                // 上偏，则前进2档
                                else if(Anchor_Init_OffsetY < -LargeOffset_ANCHOR)
                                {
                                    qDebug()<<"Init Adjusting Anchor Y With No Aim: Forward With Gear 2";
                                    on_ExecuteSend(M_F2);
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        case ANCHOR_ADJ:
        {
            // 整个过程中记录目标的偏移量
            // 如果找到了目标箱子，或者找到了盖子
            if(VInfo.AimHole.HoleHave || VInfo.CoverHole.HoleHave)
            {
                // 找到了目标箱子
                if(VInfo.AimHole.HoleHave)
                {
                    // 计算目标箱子与屏幕中心之间的X, Y偏移量
                    Anchor_Adj_OffsetX = VInfo.AimHole.CenterTarget.TarOffsetX;
                    Anchor_Adj_OffsetY = VInfo.AimHole.CenterTarget.TarOffsetY;
                }
                // 没有找到目标箱子，只找到了目标盒子
                else if(VInfo.CoverHole.HoleHave)
                {
                    // 计算盖子与屏幕中心之间的X, Y偏移量
                    Anchor_Adj_OffsetX = VInfo.CoverHole.CenterTarget.TarOffsetX;
                    Anchor_Adj_OffsetY = VInfo.CoverHole.CenterTarget.TarOffsetY;
                }
            }
            // 如果该任务过程中丢失了目标箱子或盖子，则记录丢失前瞬间的偏移量
            else
                ;

            // 深度没有调整完毕，则调整深度
            if(!Anchor_Adj_DepthOK)
            {
                // 发送深度前，停止主推侧推
                if(MF)
                    on_ExecuteSend(M_S);
                else if(SF)
                    on_ExecuteSend(S_S);
                // 主推侧推已经停止，调整深度
                else
                {
                    // 给下位机发送投放标志物的深度值
                    Send_Depth(Anchor_Adj_LaunchDepth);
                    // 达到目标深度，深度调整完毕标志位true
                    if(qAbs(depth.toFloat() - Anchor_Adj_LaunchDepth) < Proper_Depth)
                        Anchor_Adj_DepthOK = true;
                }
            }
            // 深度已经调整完毕，判断视野中是否已经丢失了目标箱子或盒子
            else
            {
                // 下潜过程中，视野中没有丢失目标箱子或盖子，则调整X, Y方向偏移量
                if(VInfo.AimHole.HoleHave || VInfo.CoverHole.HoleHave)
                {
                    if(!Anchor_Adj_AdjustX_OK)
                    {
                        qDebug()<<"Adjusting Anchor Adj X To Center.";

                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            qDebug()<<"Adjusting Anchor Adj X To Center.";

                            // 右偏，则右移1档
                            if(Anchor_Adj_OffsetX > LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Adj Adjusting Anchor X To Center: Right With Gear 1";
                                on_ExecuteSend(S_R1);
                            }
                            // 左偏，则左移1档
                            else if(Anchor_Adj_OffsetX < -LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Init Adjusting Anchor X To Center: Left With Gear 1";
                                on_ExecuteSend(S_L1);
                            }
                            // X方向达到精度范围内
                            else
                                Anchor_Adj_AdjustX_OK = true;
                        }
                    }
                    // X方向已经调整好，开始调整Y方向
                    else
                    {
                        qDebug()<<"Adjusting Anchor Adj Y To Center.";

                        if(!Anchor_Adj_AdjustY_OK)
                        {
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(qAbs(Anchor_Adj_OffsetY) > LargeOffset_ANCHOR)
                                {
                                    // 下偏，则后退1档
                                    if(Anchor_Adj_OffsetY > LargeOffset_ANCHOR)
                                    {
                                        qDebug()<<"Adj Adjusting Anchor Y To Center: Back With Gear 1";
                                        on_ExecuteSend(M_B1);
                                    }
                                    // 上偏，则前进1档
                                    else if(Anchor_Adj_OffsetY < -LargeOffset_ANCHOR)
                                    {
                                        qDebug()<<"Adj Adjusting Anchor Y To Center: Forward With Gear 1";
                                        on_ExecuteSend(M_F1);
                                    }
                                }
                                // Y方向达到精度范围内，并记录此时的角度值，作为调角度的基准值
                                else
                                {
                                    Anchor_Adj_AdjustY_OK = true;
                                    Anchor_Adj_OffsetXY_InitAngle = yaw.toDouble();
                                }
                            }
                        }

                        // X, Y都达到了精度，则调整角度，使箱子长边平行于图像X方向
                        else
                        {
                            // 角度没有调整好，调整角度
                            if(!Anchor_Adj_AngleOK)
                            {
                                // 调整角度前，关闭主推侧推
                                if(MF)
                                    on_ExecuteSend(M_S);
                                else if(SF)
                                    on_ExecuteSend(S_S);
                                // 主推侧推全部关闭，开始调整角度
                                else
                                {
                                    double DeltaAngle;

                                    // 视野中有目标箱子，则计算目标箱子的角度
                                    if(VInfo.AimHole.HoleHave)
                                    {
                                        DeltaAngle = getRcDegree(VInfo.AimHole.HoleRect);
                                        DeltaAngle = DeltaAngle - 180;

                                        // 计算短边角度，并将角度固定在-90---90度之间
                                        // \为正值，/为负值
                                        if(DeltaAngle >= 90)
                                            DeltaAngle = DeltaAngle - 180;
                                        else if(DeltaAngle <= -90)
                                            DeltaAngle = DeltaAngle + 180;
                                    }
                                    // 视野中没有目标箱子但有盖子，则计算盖子的角度
                                    else if(VInfo.CoverHole.HoleHave)
                                    {
                                        DeltaAngle = getRcDegree(VInfo.CoverHole.HoleRect);
                                        DeltaAngle = DeltaAngle - 180;

                                        // 计算短边角度，并将角度固定在-90---90度之间
                                        // \为正值，/为负值
                                        if(DeltaAngle >= 90)
                                            DeltaAngle = DeltaAngle - 180;
                                        else if(DeltaAngle <= -90)
                                            DeltaAngle = DeltaAngle + 180;
                                    }

                                    // 第一次发送角度值
                                    if(Anchor_Adj_AngleCount == 0)
                                    {
                                        // 计算应该偏移的角度值
                                        Anchor_Adj_LaunchAngle = yaw.toDouble() - DeltaAngle;

                                        if(Anchor_Adj_LaunchAngle < 0)
                                            Anchor_Adj_LaunchAngle = Anchor_Adj_LaunchAngle + 360;
                                        if(Anchor_Adj_LaunchAngle > 360)
                                            Anchor_Adj_LaunchAngle = Anchor_Adj_LaunchAngle - 360;
                                        // 发送角度值
                                        Send_Angle(Anchor_Adj_LaunchAngle);
                                        Anchor_Adj_AngleCount++;
                                    }

                                    // 发送第一次角度值后，持续检查角度值
                                    if(qAbs(Anchor_Adj_LaunchAngle - yaw.toDouble()) < Proper_Angle * 2)
                                    {
                                        Anchor_Adj_AngleCount++;
                                        if(Anchor_Adj_AngleCount >= 4)
                                        {
                                            // 目标箱子或盖子已经稳定到了水平角度
                                            if(qAbs(DeltaAngle) < Proper_Angle * 2)
                                            {
                                                Anchor_Adj_AngleOK = true;
                                            }
                                            // 还没有完全稳定
                                            else
                                            {
                                                Anchor_Adj_LaunchAngle = yaw.toDouble() - DeltaAngle;
                                                if(Anchor_Adj_LaunchAngle < 0)
                                                    Anchor_Adj_LaunchAngle = Anchor_Adj_LaunchAngle + 360;
                                                if(Anchor_Adj_LaunchAngle > 360)
                                                    Anchor_Adj_LaunchAngle = Anchor_Adj_LaunchAngle - 360;
                                                // 发送角度值
                                                Send_Angle(Anchor_Adj_LaunchAngle);
                                                // 角度发送计数器重置
                                                Anchor_Adj_AngleCount = 1;
                                            }
                                        }
                                    }
                                }
                            }
                            // 如果深度，X, Y偏移量，角度都调整完毕，则进入下一任务阶段
                            else
                            {
                                if(SF)
                                    on_ExecuteSend(S_S);
                                else if(MF)
                                    on_ExecuteSend(M_S);

                                // 左标志物尚未投放，则切换到投放左标志物任务
                                if(!Anchor_LEFT_Launched)
                                    Anchor_Adjust = ANCHOR_LEFT;
                                // 右标志物尚未投放，则切换到投放右标志物任务
                                else if(!Anchor_RIGHT_Launched)
                                    Anchor_Adjust = ANCHOR_RIGHT;
                                // 左右标志物都投放完毕，则切换到任务结束阶段
                                else
                                    Anchor_Adjust = ANCHOR_FINISH;

                                Anchor_Adj_OK_Time = MainTime;

                                // X, Y调整完毕标志位归为false
                                Anchor_Adj_AdjustX_OK = false;
                                Anchor_Adj_AdjustY_OK = false;
                            }
                        }
                    }
                }
                // 下潜过程中，视野中丢失了目标箱子或盖子
                // 则根据记录下来的OffsetX, OffsetY调整
                else
                {
                    // X方向偏移量大，主要调整X方向
                    if(qAbs(Anchor_Adj_OffsetX) > qAbs(Anchor_Adj_OffsetY))
                    {
                        qDebug()<<"Adjusting Anchor Adj X With No Aim.";
                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            // 右偏，则右移2档
                            if(Anchor_Adj_OffsetX > LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Adj Adjusting Anchor X With No Aim: Right With Gear 2";
                                on_ExecuteSend(S_R2);
                            }
                            // 左偏，则左移2档
                            else if(Anchor_Adj_OffsetX < -LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Adj Adjusting Anchor X With No Aim: Left With Gear 2";
                                on_ExecuteSend(S_L2);
                            }
                        }
                    }
                    // Y方向偏移量大，主要调整Y方向
                    else
                    {
                        qDebug()<<"Adjusting Anchor Adj Y With No Aim.";
                        // 调整Y方向时，先将侧推关闭
                        if(SF)
                            on_ExecuteSend(S_S);
                        else
                        {
                            // 下偏，则后退2档
                            if(Anchor_Adj_OffsetY > LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Adj Adjusting Anchor Y With No Aim: Back With Gear 2";
                                on_ExecuteSend(M_B2);
                            }
                            // 上偏，则前进2档
                            else if(Anchor_Adj_OffsetY < -LargeOffset_ANCHOR)
                            {
                                qDebug()<<"Adj Adjusting Anchor Y With No Aim: Forward With Gear 2";
                                on_ExecuteSend(M_F2);
                            }
                        }
                    }
                }
            }

            break;
        }
        case ANCHOR_LEFT:
        {
            // 如果没有找到目标箱子和盖子，则回到上个阶段
            if(!VInfo.AimHole.HoleHave && !VInfo.CoverHole.HoleHave)
            {
                Anchor_Adjust = ANCHOR_ADJ;
            }
            // 找到了目标箱子或盖子，则优先寻找箱子
            else
            {
                // 找到了目标箱子
                if(VInfo.AimHole.HoleHave)
                {
    //                qDebug()<<"Left Adjusting: Adjusting Aim Box.";

                    // X方向没有调整完毕
                    if(!Anchor_LEFT_AdjustX_OK)
                    {
    //                    qDebug()<<"Left X Adjusting: Adjusting Aim Box.";

                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            if(qAbs(VInfo.AimHole.LeftTarget.TarOffsetX) > VInfo.AimHole.LeftTarget.width / 2)
                            {
                                // 右偏，则右移1档
                                if(VInfo.AimHole.LeftTarget.TarOffsetX > VInfo.AimHole.LeftTarget.width / 2)
                                {
                                    qDebug()<<"LEFT Target X Adjusting: Right with Gear 1";
                                    on_ExecuteSend(S_R1);
                                }
                                // 左偏，则左移1档
                                else if(VInfo.AimHole.LeftTarget.TarOffsetX < -VInfo.AimHole.LeftTarget.width / 2)
                                {
                                    qDebug()<<"LEFT Target X Adjusting: Left with Gear 1";
                                    on_ExecuteSend(S_L1);
                                }
                            }
                            else
                                Anchor_LEFT_AdjustX_OK = true;
                        }
                    }
                    // X方向调整完毕，开始调节Y方向
                    else
                    {
    //                    qDebug()<<"Left Y Adjusting: Adjusting Aim Box.";

                        if(!Anchor_LEFT_AdjustY_OK)
                        {
                            // 调整Y方向时，先将侧推关闭
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(qAbs(VInfo.AimHole.LeftTarget.TarOffsetY) > VInfo.AimHole.LeftTarget.width / 2)
                                {
                                    // 下偏，则后退1档
                                    if(VInfo.AimHole.LeftTarget.TarOffsetY > VInfo.AimHole.LeftTarget.width / 2)
                                    {
                                        qDebug()<<"LEFT Target Y Adjusting: Back with Gear 1";
                                        on_ExecuteSend(M_B1);
                                    }
                                    // 上偏，则前进1档
                                    else if(VInfo.AimHole.LeftTarget.TarOffsetY < -VInfo.AimHole.LeftTarget.width / 2)
                                    {
                                        qDebug()<<"LEFT Target Y Adjusting: Forward with Gear 1";
                                        on_ExecuteSend(M_F1);
                                    }
                                }
                                else
                                {
                                    Anchor_LEFT_AdjustY_OK = true;
                                    Anchor_LEFT_OK_Time = MainTime;
                                }
                            }
                        }
                        // X, Y都达到了精度，投放标志物
                        else
                        {
                            // 发射前，停止主推侧推
                            if(MF)
                                on_ExecuteSend(M_S);
                            else if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                // 对准2秒后，再发射
                                int time = MainTime - Anchor_LEFT_OK_Time;

                                if(time <= 3)
                                {
                                    if(qAbs(VInfo.AimHole.LeftTarget.TarOffsetX) > VInfo.AimHole.LeftTarget.width / 2
                                            || qAbs(VInfo.AimHole.LeftTarget.TarOffsetY) > VInfo.AimHole.LeftTarget.width / 2)
                                    {
                                        Anchor_LEFT_AdjustX_OK = false;
                                        Anchor_LEFT_AdjustY_OK = false;

                                        Anchor_Adjust = ANCHOR_ADJ;
                                    }
                                    else
                                    {
                                        qDebug()<<"Anchor Left Object is Moving to Left Target Area Time: "<<time;
                                    }
                                }
                                else
                                {
                                    if(!Anchor_LEFT_Launched)
                                    {
                                        qDebug()<<"Launch LEFT Object!";

                                        // 投放左标志物
                                        on_ExecuteSend(ANCHOR_LA);
                                        Anchor_LEFT_Launched = true;
                                    }
                                    else
                                    {
                                        Anchor_Adjust = ANCHOR_RIGHT;

                                        // 标志位置为false
                                        Anchor_LEFT_AdjustX_OK = false;
                                        Anchor_LEFT_AdjustY_OK = false;
                                    }
                                }
                            }
                        }
                    }
                }
                // 没有找到目标箱子，但找到了盖子
                // 则将盖子移动到视野中央，则目标箱子会出现在视野中
                else if(VInfo.CoverHole.HoleHave)
                {
                    qDebug()<<"Left Adjusting: Adjusting Cover.";

                    int CoverOffsetX = VInfo.CoverHole.CenterTarget.TarOffsetX;
                    int CoverOffsetY = VInfo.CoverHole.CenterTarget.TarOffsetY;

                    // X方向没有调整完毕
                    if(!Anchor_LEFT_AdjustX_OK)
                    {
                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            // 调整X方向
                            if(qAbs(CoverOffsetX) > ProperOffset_ANCHOR)
                            {
                                // 右偏，则右移1档
                                if(CoverOffsetX > ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"LEFT Target X Cover Adjusting: Right with Gear 1";
                                    on_ExecuteSend(S_R1);
                                }
                                // 左偏，则左移1档
                                else if(CoverOffsetX < -ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"LEFT Target X Cover Adjusting: Left with Gear 1";
                                    on_ExecuteSend(S_L1);
                                }
                            }
                            else
                                Anchor_LEFT_AdjustX_OK = true;
                        }
                    }
                    // X方向调整完毕，开始调节Y方向
                    else
                    {
                        if(!Anchor_LEFT_AdjustY_OK)
                        {
                            // 调整Y方向时，先将侧推关闭
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(qAbs(CoverOffsetY) > ProperOffset_ANCHOR)
                                {
                                    // 下偏，则后退1档
                                    if(CoverOffsetY > ProperOffset_ANCHOR)
                                    {
                                        qDebug()<<"LEFT Target Y Cover Adjusting: Back with Gear 1";
                                        on_ExecuteSend(M_B1);
                                    }
                                    // 上偏，则前进1档
                                    else if(CoverOffsetY < -ProperOffset_ANCHOR)
                                    {
                                        qDebug()<<"LEFT Target Y Cover Adjusting: Forward with Gear 1";
                                        on_ExecuteSend(M_F1);
                                    }
                                }
                                else
                                    Anchor_LEFT_AdjustY_OK = true;
                            }
                        }
                        // X, Y都达到了精度，投放标志物
                        else
                        {
                            // 发射前，停止主推侧推
                            if(MF)
                                on_ExecuteSend(M_S);
                            else if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(!Anchor_LEFT_Launched)
                                {
                                    qDebug()<<"Launch LEFT Object!";

                                    // 投放左标志物
                                    on_ExecuteSend(ANCHOR_LA);
                                    Anchor_LEFT_Launched = true;
                                }
                                else
                                {
                                    Anchor_Adjust = ANCHOR_RIGHT;

                                    // 标志位置为false
                                    Anchor_LEFT_AdjustX_OK = false;
                                    Anchor_LEFT_AdjustY_OK = false;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case ANCHOR_RIGHT:
        {
            // 如果没有找到目标箱子和盖子，则回到上个阶段
            if(!VInfo.AimHole.HoleHave && VInfo.CoverHole.HoleHave)
            {
                Anchor_Adjust = ANCHOR_ADJ;
            }
            // 找到了目标箱子或盖子，则优先寻找箱子
            else
            {
                if(VInfo.AimHole.HoleHave)
                {
                    qDebug()<<"Right Adjusting: Adjusting Aim Box.";

                    // X方向没有调整完毕
                    if(!Anchor_RIGHT_AdjustX_OK)
                    {
                        qDebug()<<"Right X Adjusting: Adjusting Aim Box.";

                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            if(qAbs(VInfo.AimHole.CenterTarget.TarOffsetX) > VInfo.AimHole.CenterTarget.width / 2)
                            {
                                // 右偏，则右移1档
                                if(VInfo.AimHole.CenterTarget.TarOffsetX > VInfo.AimHole.CenterTarget.width / 2)
                                {
                                    qDebug()<<"RIGHT Target X Adjusting: Right with Gear 1";
                                    on_ExecuteSend(S_R1);
                                }
                                // 左偏，则左移1档
                                else if(VInfo.AimHole.CenterTarget.TarOffsetX < -VInfo.AimHole.CenterTarget.width / 2)
                                {
                                    qDebug()<<"RIGHT Target X Adjusting: Left with Gear 1";
                                    on_ExecuteSend(S_L1);
                                }
                            }
                            else
                                Anchor_RIGHT_AdjustX_OK = true;
                        }
                    }
                    // X方向调整完毕，开始调节Y方向
                    else
                    {
                        qDebug()<<"Right Y Adjusting: Adjusting Aim Box.";

                        if(!Anchor_RIGHT_AdjustY_OK)
                        {
                            // 调整Y方向时，先将侧推关闭
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(qAbs(VInfo.AimHole.CenterTarget.TarOffsetY) > VInfo.AimHole.CenterTarget.width / 2)
                                {
                                    // 下偏，则后退1档
                                    if(VInfo.AimHole.CenterTarget.TarOffsetY > VInfo.AimHole.CenterTarget.width / 2)
                                    {
                                        qDebug()<<"RIGHT Target Y Cover Adjusting: Back with Gear 1";
                                        on_ExecuteSend(M_B1);
                                    }
                                    // 上偏，则前进1档
                                    else if(VInfo.AimHole.CenterTarget.TarOffsetY < VInfo.AimHole.CenterTarget.width / 2)
                                    {
                                        qDebug()<<"RIGHT Target Y Cover Adjusting: Forward with Gear 1";
                                        on_ExecuteSend(M_F1);
                                    }
                                }
                                else
                                {
                                    Anchor_RIGHT_AdjustY_OK = true;
                                    Anchor_RIGHT_OK_Time = MainTime;
                                }
                            }
                        }
                        // X, Y都达到了精度，投放标志物
                        else
                        {
                            // 发射前，停止主推侧推
                            if(MF)
                                on_ExecuteSend(M_S);
                            else if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                // 对准2秒后，再发射
                                int time = MainTime - Anchor_RIGHT_OK_Time;

                                if(time <= 3)
                                {
                                    if(qAbs(VInfo.AimHole.CenterTarget.TarOffsetX) > VInfo.AimHole.CenterTarget.width / 2
                                            || qAbs(VInfo.AimHole.CenterTarget.TarOffsetY) > VInfo.AimHole.CenterTarget.width / 2)
                                    {
                                        Anchor_LEFT_AdjustX_OK = false;
                                        Anchor_LEFT_AdjustY_OK = false;

                                        Anchor_Adjust = ANCHOR_ADJ;
                                    }
                                    else
                                    {
                                        qDebug()<<"Anchor Right Object is Moving to Right Target Area Time: "<<time;
                                    }
                                }
                                else
                                {
                                    if(!Anchor_RIGHT_Launched)
                                    {
                                        qDebug()<<"Launch RIGHT Object!";

                                        // 投放左标志物
                                        on_ExecuteSend(ANCHOR_RA);
                                        Anchor_RIGHT_Launched = true;
                                    }
                                    else
                                    {
                                        Anchor_Adjust = ANCHOR_FINISH;

                                        // 标志位置为false
                                        Anchor_RIGHT_AdjustX_OK = false;
                                        Anchor_RIGHT_AdjustY_OK = false;
                                    }
                                }
                            }
                        }
                    }
                }
                // 没有找到目标箱子，但找到了盖子
                // 则将盖子移动到视野中央，则目标箱子会出现在视野中
                else if(VInfo.CoverHole.HoleHave)
                {
                    qDebug()<<"Right Adjusting: Adjusting Cover.";

                    int CoverOffsetX = VInfo.CoverHole.CenterTarget.TarOffsetX;
                    int CoverOffsetY = VInfo.CoverHole.CenterTarget.TarOffsetY;

                    // X方向没有调整完毕
                    if(!Anchor_RIGHT_AdjustX_OK)
                    {
                        // 调整X方向时，先将主推关闭
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            // 调整X方向
                            if(qAbs(CoverOffsetX) > ProperOffset_ANCHOR)
                            {
                                // 右偏，则右移1档
                                if(CoverOffsetX > ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"RIGHT Target X Cover Adjusting: Right with Gear 1";
                                    on_ExecuteSend(S_R1);
                                }
                                // 左偏，则左移1档
                                else if(CoverOffsetX < -ProperOffset_ANCHOR)
                                {
                                    qDebug()<<"RIGHT Target X Cover Adjusting: Left with Gear 1";
                                    on_ExecuteSend(S_L1);
                                }
                            }
                            else
                                Anchor_RIGHT_AdjustX_OK = true;
                        }
                    }
                    // X方向调整完毕，开始调节Y方向
                    else
                    {
                        if(!Anchor_RIGHT_AdjustY_OK)
                        {
                            // 调整Y方向时，先将侧推关闭
                            if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(qAbs(CoverOffsetY) > ProperOffset_ANCHOR)
                                {
                                    // 下偏，则后退1档
                                    if(CoverOffsetY > ProperOffset_ANCHOR)
                                    {
                                        qDebug()<<"RIGHT Target Y Cover Adjusting: Back with Gear 1";
                                        on_ExecuteSend(M_B1);
                                    }
                                    // 上偏，则前进1档
                                    else if(CoverOffsetY < -ProperOffset_ANCHOR)
                                    {
                                        qDebug()<<"RIGHT Target Y Cover Adjusting: Forward with Gear 1";
                                        on_ExecuteSend(M_F1);
                                    }
                                }
                                else
                                    Anchor_RIGHT_AdjustY_OK = true;
                            }
                        }
                        // X, Y都达到了精度，投放标志物
                        else
                        {
                            // 发射前，停止主推侧推
                            if(MF)
                                on_ExecuteSend(M_S);
                            else if(SF)
                                on_ExecuteSend(S_S);
                            else
                            {
                                if(!Anchor_RIGHT_Launched)
                                {
                                    qDebug()<<"Launch RIGHT Object!";

                                    // 投放左标志物
                                    on_ExecuteSend(ANCHOR_RA);
                                    Anchor_RIGHT_Launched = true;
                                }
                                else
                                {
                                    Anchor_Adjust = ANCHOR_FINISH;

                                    // 标志位置为false
                                    Anchor_RIGHT_AdjustX_OK = false;
                                    Anchor_RIGHT_AdjustY_OK = false;
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        case ANCHOR_FINISH:
        {
            // 投放标志物气动收回
            if(Anchor_LEFT_Launched || Anchor_RIGHT_Launched)
                on_ExecuteSend(ANCHOR_LB);
            else
            {
                // 关闭主推侧推
                if(MF)
                    on_ExecuteSend(M_S);
                else if(SF)
                    on_ExecuteSend(S_S);
                else
                {
                    //************************
                    // 转向下一个任务，八边形浮起
                    //************************

                    // 调整为八边形水声深度
                    if(qAbs(depth.toDouble() - Oct_Depth) > Proper_Depth)
                        Send_Depth(Oct_Depth);
                    // 调整为八边形水声初始角度，初始角度被设定为风格过门角度
                    else
                    {
                        if(qAbs(yaw.toDouble() - Oct_StartAngle) > Proper_Angle)
                            Send_Angle(Oct_StartAngle);
                        else
                        {
                            // 初始水声任务为八边形，则此时已经结束所有程序
                            if(Random_Oct)
                            {
                                if(MF)
                                    on_ExecuteSend(M_S);
                                else if(SF)
                                    on_ExecuteSend(S_S);
                                else
                                {
                                    // 所有标志位置false
                                    Anchor_Init_AdjustX_OK = false;
                                    Anchor_Init_AdjustY_OK = false;
                                    Anchor_Adj_AdjustX_OK = false;
                                    Anchor_Adj_AdjustY_OK = false;
                                    Anchor_Adj_DepthOK = false;
                                    Anchor_Adj_AngleOK = false;
                                    Anchor_LEFT_AdjustX_OK = false;
                                    Anchor_LEFT_AdjustY_OK = false;
                                    Anchor_RIGHT_AdjustX_OK = false;
                                    Anchor_RIGHT_AdjustY_OK = false;
                                    Anchor_LEFT_Launched = false;
                                    Anchor_RIGHT_Launched = false;

                                    iamtask = BODYBACK;
                                }
                            }
                            // 初始水声任务为投放标志物，则此时应该导向八边形
                            else
                            {
                                if(qAbs(depth.toDouble() - Oct_Depth) > Proper_Depth)
                                    Send_Depth(Oct_Depth);
                                else
                                {
                                    if(qAbs(yaw.toDouble() - Oct_StartAngle) > Proper_Angle)
                                        Send_Angle(Oct_StartAngle);
                                    else
                                    {
                                        // 所有标志位置false
                                        Anchor_Init_AdjustX_OK = false;
                                        Anchor_Init_AdjustY_OK = false;
                                        Anchor_Adj_AdjustX_OK = false;
                                        Anchor_Adj_AdjustY_OK = false;
                                        Anchor_Adj_DepthOK = false;
                                        Anchor_Adj_AngleOK = false;
                                        Anchor_LEFT_AdjustX_OK = false;
                                        Anchor_LEFT_AdjustY_OK = false;
                                        Anchor_RIGHT_AdjustX_OK = false;
                                        Anchor_RIGHT_AdjustY_OK = false;
                                        Anchor_LEFT_Launched = false;
                                        Anchor_RIGHT_Launched = false;

                                        Oct_StartTime = MainTime;
                                        iamtask = BURY_TREASURE;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        default:
            break;
        }
    }


}

void AuvAutoControl::M_Bodyback()
{

    if(DEBUGSWITCH)
    {
        if(mission_count==6)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Bodyback_Mission"<< endl;
            mission_count++;
        }
    }
    if(MF)
        on_ExecuteSend(M_S);
    else if(SF)
        on_ExecuteSend(S_S);
    else
        Send_Depth(0.1);
    mission_count = 0;
}

// 安保程序
void AuvAutoControl::Safety()
{
    // 对深度值与程序运行时间，设定安全保障
    if(!safety_flag)
    {
        // 深度限定值
        if((depth.toFloat()) > 4.7)
        {
           safety_flag = true;
           iamtask = BODYBACK;
           QTextStream out(Debug);
           QTime now= QTime::currentTime();
           out<<now.toString()<<'\t'<<"深度超过"<< endl;

        }
        // 最大时间限定
        if(MainTime > MAX_TIME)
        {
           safety_flag = true;
           iamtask = BODYBACK;
           QTextStream out(Debug);
           QTime now= QTime::currentTime();
           out<<now.toString()<<'\t'<<"时间到了"<< endl;
        }
    }
}

// 水声定位任务
void AuvAutoControl::M_Process_UnderAC()
{
    if(DEBUGSWITCH)
    {
        if(mission_count==4)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"M_Process_UnderAC"<< endl;
            mission_count++;
        }
    }

    //*****************************
    //     切换到投放标志位任务
    //*****************************

    int UnderAc_Time = MainTime - UnderAc_StartTime;

    Oct_Fun_Angle.clear();

    for(int i = 0; i < 7; i++)
    {
        if(Oct_Ac_Angle[i] != 0)
            Oct_Fun_Angle.push_back(Oct_Ac_Angle[i]);
    }

    if(Oct_Fun_Angle.size() >= 4)
    {
        // 竖直阵检测到较多数据，且俯视摄像头中有目标盒子出现
        if(UnderAc_Time > 30
                && VInfo.AimHole.HoleHave)
        {
            // 切换为投放标志物任务
            missionflag[4] = 1;
            if(MF)
                on_ExecuteSend(M_S);

            // 记录投放标志物开始时间
            Anchor_StartTime = MainTime;

            // 记录八边形水声初始角度，记为时间门角度
            Oct_StartAngle = Navigate_Angle;

            // 所有标志位重置
            AngleOK = false;
            GoAngleOk = false;
            SendAngleOK = false;
            FirstTurn = false;
            Straight = false;
            AcAngleFinishb = false;
        }
    }


    //*****************************
    //     数据分析处理部分
    //*****************************

    // 角度没有采集完毕
    if(!AngleOK)
    {
        // 当前接收到水声信号没有超过ACTimes(7次)的时候，继续接受水声信号并计数
        if(count_time < ACtimes)
        {
            // 水声角度44，在航行器左边
            if(UnderAcDirection1 == "44")
            {
                 Dir[count_time] = Left;
                 Ac_Angle[count_time] = FUnderAcAngle1;
                 count_time++;
                 Count_amount++;
            }
            // 水声角度66，在航行器右边
            else if(UnderAcDirection1=="66")
            {
                Dir[count_time] = Right;
                Ac_Angle[count_time] = FUnderAcAngle1;
                count_time++;
                Count_amount = Count_amount+2;
            }
        }
        // 当前接收到水声信号到达超过ACTimes(7次)的时候，处理数据并发送
        else
        {
            // 当前时刻的水声方向，更新给上一时刻水声方向
            LastAcDir = AcDir;
            // 计算计数范围之内，左右次数出现的频率
            double temp = Count_amount / ACtimes;
            // 左右次数信息清零
            Count_amount = 0;
            // 解析出的方向，在水声方向缓冲区的个数
            int u1 = 0;
            // 水声信号在左边出现次数较多
            if(temp < 1.5 )
            {
                for(int i = 0; i < ACtimes; i++)
                {
                    if(Dir[i] == Left)
                    {
                        u1++;
                        GoAngle = GoAngle + Ac_Angle[i];
                    }
                }
                // 11代表左边
                AcDir = 11;
                ui->lineEdit_acd->setText("LEFT");
            }
            // 水声信号在右边出现次数较多
            else
            {
                for(int j = 0; j < ACtimes; j++)
                {
                    if(Dir[j] == Right)
                    {
                        GoAngle = GoAngle + Ac_Angle[j];
                        u1++;
                    }
                }
                // 22代表右边
                AcDir = 22;
                ui->lineEdit_acd->setText("RIGHT");
            }

            // 计算在解算方向的角度平均值
            GoAngle = GoAngle / u1;
            ui->lineEdit_acd2->setText(QString::number(GoAngle));

            // 角度解算完毕标志位置为真
            GoAngleOk = true;
            // 重新计数
            count_time = 0;
        }
    }

    //*****************************
    //   角度解算完毕，做出对应动作
    //*****************************

    // 计算出一个大周期内的角度的情况下
    if(GoAngleOk)
    {
        // 角度较小，可以认为在前方
        // 此时角度采集完毕，不再解算角度，并将直行标志位Straight置为真
        if(GoAngle <= 15)
        {
            AngleOK = true;
            GoAngleOk = false;
            // 此后的角度调整，变为在Straight判断中的角度微调
            Straight = true;
            Straight_Times++;
            qDebug()<<"Go Angle Ok.";
        }
        // 角度较大，角度需要根据水声信号的测量值调整
        else
        {
            // 先停止主推
            if(MF)
                on_ExecuteSend(M_S);
            else
            {
                // 角度采集完毕
                AngleOK = true;
                GoAngleOk = false;
                AcAngleFinish = false;
                Straight_Times = 0;
                // 水声方向在左边，GoAngle减运算
                if(AcDir == 11)
                {
                    Send_AcAngle = yaw.toFloat() + GoAngle;
                }
                // 水声方向在右边，GoAngle减运算
                else if(AcDir == 22)
                {
                    Send_AcAngle = yaw.toFloat() - GoAngle;
                }
                // 角度控制在0--360度内
                if(Send_AcAngle > 360)
                    Send_AcAngle = Send_AcAngle - 360;
                if(Send_AcAngle < 0)
                    Send_AcAngle = 360 + Send_AcAngle;

                // 发送结算完毕的角度，调整角度
                Send_Angle(Send_AcAngle);
                qDebug()<<"Send Ac Angle and Adjust.";
                // 角度发送完毕
                SendAngleOK = true;
            }
        }
    }

    //*****************************
    //   角度发送完毕，开始判断是否直行
    //*****************************

    if(SendAngleOK)
    {
        qDebug()<<"Send Angle: "<<Send_AcAngle;
        qDebug()<<"Current Angle: "<<yaw.toFloat();

        // 当前角度到达角度准许精度范围内
        if(qAbs(Send_AcAngle - yaw.toFloat()) < Proper_Angle)
        {
            qDebug()<<"Current Angle's Error is Less Than 3 degree.";
            count_time1++;
            // 超过五次以上，角度到达了精度误差范围内
            if(count_time1 >= 5)
            {
                // 重置角度发送完毕标志位
                SendAngleOK = false;
                // 重置角度采集完毕标志位，再次采水声角度值
                AngleOK = false;
                // 如果认为到达了正对方向（两次水声方向不同），则直行3档
                if(AcAngleFinish)
                {
                    FirstTurn = false;
                    on_ExecuteSend(M_F3);
                    qDebug()<<"Ac Angle OK & Go Straight with Gear 3.";
                }
                count_time1 = 0;
            }
        }
        // 当前角度没有到达角度准许精度范围内，重新发送新周期解算出的角度
        else
        {
            Send_Angle(Send_AcAngle);
            count_time1 = 0;
        }

        qDebug()<<count_time1;
    }

    //*****************************
    //     直行情况的判断
    //*****************************

    // 在Straight判断中，角度的调整为逐步渐进调整
    if(Straight)
    {
        // 第一次
        if(!FirstTurn)
            FirstTurn = true;
        // 二次后的直行
        else
        {
            // 在该大周期内，发送微调后的水声角度，并在最后令Straight标志位置为假，跳出该次大周期
            if(!MF)
            {
                // 如果该周期的水声方向与上一周期的方向相同，则做较大角度的处理
                if(LastAcDir == AcDir)
                {
                    if(AcDir == 11)
                        Send_AcAngle = yaw.toFloat() + 10;
                    if(AcDir == 22)
                        Send_AcAngle = yaw.toFloat() - 10;
                    qDebug()<<"Last & Current Direction are SAME.";
                }
                // 如果该周期的水声方向与上一周期的方向不同，则做较小角度的处理
                else
                {
                    if(AcDir == 11)
                        Send_AcAngle = yaw.toFloat() + 5;
                    if(AcDir == 22)
                        Send_AcAngle = yaw.toFloat() - 5;

                    // 直行状态下，两次采到的水声方向不同，可认为到达了正对方向
                    // 判读直行，需要令两个周期分别采到一左一右的数据，且两次数据值都小于15度
                    AcAngleFinish = true;
                    qDebug()<<"Last & Current Direction are DIFFERENT && GO STRAIGHT.";
                }

                // 角度控制在0--360度内
                if(Send_AcAngle > 360)
                    Send_AcAngle = Send_AcAngle - 360;
                if(Send_AcAngle < 0)
                    Send_AcAngle = 360 + Send_AcAngle;

                Send_Angle(Send_AcAngle);
                qDebug()<<"as1";
                SendAngleOK = true;
                //
                Straight = false;
            }
            // 如果此时主推正在直行，则AngleOK置为假，即直行过程中一直在更新水声当前角度
            else
                AngleOK = false;
        }
        Straight_Times = 0;
    }
}

// 最后一天调试水声出状况，临时改的程序，有效果，但一些注释项没有更改。。。
void AuvAutoControl::M_Bury_treasure()
{
    if(DEBUGSWITCH)
    {
        if(mission_count==6)
        {
            QTextStream out(Debug);
            QTime now= QTime::currentTime();
            out<<now.toString()<<'\t'<<"Set_course_Mission"<< endl;
            mission_count++;
        }
    }


    int Oct_Time = MainTime - Oct_StartTime;
    if(Oct_Time > 180)
    {
        if(SF)
            on_ExecuteSend(S_S);
        else if(MF)
            on_ExecuteSend(M_S);
        // 初始水声任务为八边形，则还要做投放标志物任务
        else if(Random_Oct)
        {
            if(!Oct_FloatUP_Done)
            {
                if(qAbs(depth.toDouble() - 0.1) > Proper_Depth)
                {
                    Send_Depth(0.1);
                }
                else
                    Oct_FloatUP_Done = true;
            }
            else
            {
                // 投放标志物，先下沉到2.5m左右
                if(qAbs(depth.toDouble() - 2.5) > Proper_Depth)
                    Send_Depth(2.5);
                else
                {
                    double AnchorAngle = Navigate_Angle - 30;
                    if(AnchorAngle > 360)       AnchorAngle = AnchorAngle - 360;
                    else if(AnchorAngle < 0)    AnchorAngle = AnchorAngle + 360;

                    if(qAbs(yaw.toDouble() - AnchorAngle) > Proper_Angle)
                        Send_Angle(AnchorAngle);
                    else
                    {
                        iamtask = UNDERAC;
                        UnderAc_StartTime = MainTime;
                    }
                }
            }
        }
        // 初始水声任务为投放标志物，则此时已经结束所有程序
        else
        {
            if(MF)
                on_ExecuteSend(M_S);
            else if(SF)
                on_ExecuteSend(S_S);
            else
                iamtask = BODYBACK;
        }
    }
    else
    {

        if(Oct_VerWork_Work)
        {
            Oct_Fun_Angle.clear();

            for(int i = 0; i < 7; i++)
            {
                if(Oct_Ac_Angle[i] != 0)
                    Oct_Fun_Angle.push_back(Oct_Ac_Angle[i]);
            }

            for(int i = 0; i < Oct_Fun_Angle.size(); i++)
            {
                if(Oct_Fun_Angle[i] > 0 && Oct_Fun_Angle[i] < 15)
                {
                    Oct_VerWork = true;
                    Oct_VerWork_Work = false;
                }
                else
                {
                    Oct_VerWork = false;
                    break;
                }
            }
        }

        //**********************************************************
        //                 角度处理部分
        //**********************************************************

        if(!Oct_SwitchAC)
        {
            if(Oct_BeforeSwitchTime < 5)
            {
                if(SF)
                    on_ExecuteSend(S_S);
                else
                    on_ExecuteSend(M_F3);

                Oct_BeforeSwitchTime++;
            }
            else
            {
                if(MF)
                    on_ExecuteSend(M_S);
                else
                    Oct_SwitchAC = true;
            }
        }
        else
        {
            // 竖直阵不起作用
            if(!Oct_VerWork)
            {
                //*****************************
                //     数据分析处理部分
                //*****************************

                // 角度没有采集完毕
                if(!AngleOK)
                {
                    // 当前接收到水声信号没有超过ACTimes(7次)的时候，继续接受水声信号并计数
                    if(count_time < ACtimes)
                    {
                        // 水声角度44，在航行器左边
                        if(UnderAcDirection1 == "44")
                        {
                             Dir[count_time] = Left;
                             Ac_Angle[count_time] = FUnderAcAngle1;
                             count_time++;
                             Count_amount++;
                        }
                        // 水声角度66，在航行器右边
                        else if(UnderAcDirection1=="66")
                        {
                            Dir[count_time] = Right;
                            Ac_Angle[count_time] = FUnderAcAngle1;
                            count_time++;
                            Count_amount = Count_amount+2;
                        }
                    }
                    // 当前接收到水声信号到达超过ACTimes(7次)的时候，处理数据并发送
                    else
                    {
                        // 当前时刻的水声方向，更新给上一时刻水声方向
                        LastAcDir = AcDir;
                        // 计算计数范围之内，左右次数出现的频率
                        double temp = Count_amount / ACtimes;
                        // 左右次数信息清零
                        Count_amount = 0;
                        // 解析出的方向，在水声方向缓冲区的个数
                        int u1 = 0;
                        // 水声信号在左边出现次数较多
                        if(temp < 1.5 )
                        {
                            for(int i = 0; i < ACtimes; i++)
                            {
                                if(Dir[i] == Left)
                                {
                                    u1++;
                                    GoAngle = GoAngle + Ac_Angle[i];
                                }
                            }
                            // 11代表左边
                            AcDir = 11;
                            ui->lineEdit_acd->setText("LEFT");
                        }
                        // 水声信号在右边出现次数较多
                        else
                        {
                            for(int j = 0; j < ACtimes; j++)
                            {
                                if(Dir[j] == Right)
                                {
                                    GoAngle = GoAngle + Ac_Angle[j];
                                    u1++;
                                }
                            }
                            // 22代表右边
                            AcDir = 22;
                            ui->lineEdit_acd->setText("RIGHT");
                        }

                        // 计算在解算方向的角度平均值
                        GoAngle = GoAngle / u1;
                        ui->lineEdit_acd2->setText(QString::number(GoAngle));

                        // 角度解算完毕标志位置为真
                        GoAngleOk = true;
                        // 重新计数
                        count_time = 0;
                    }
                }

                //*****************************
                //   角度解算完毕，做出对应动作
                //*****************************

                // 计算出一个大周期内的角度的情况下
                if(GoAngleOk)
                {
                    // 角度较小，可以认为在前方
                    // 此时角度采集完毕，不再解算角度，并将直行标志位Straight置为真
                    if(GoAngle <= 15)
                    {
                        AngleOK = true;
                        GoAngleOk = false;
                        // 此后的角度调整，变为在Straight判断中的角度微调
                        Straight = true;
                        Straight_Times++;
                        qDebug()<<"Go Angle Ok.";
                    }
                    // 角度较大，角度需要根据水声信号的测量值调整
                    else
                    {
                        // 先停止主推
                        if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            // 角度采集完毕
                            AngleOK = true;
                            GoAngleOk = false;
                            AcAngleFinish = false;
                            Straight_Times = 0;
                            // 水声方向在左边，GoAngle减运算
                            if(AcDir == 11)
                            {
                                Send_AcAngle = yaw.toFloat() + GoAngle;
                            }
                            // 水声方向在右边，GoAngle减运算
                            else if(AcDir == 22)
                            {
                                Send_AcAngle = yaw.toFloat() - GoAngle;
                            }
                            // 角度控制在0--360度内
                            if(Send_AcAngle > 360)
                                Send_AcAngle = Send_AcAngle - 360;
                            if(Send_AcAngle < 0)
                                Send_AcAngle = 360 + Send_AcAngle;

                            // 发送结算完毕的角度，调整角度
                            Send_Angle(Send_AcAngle);
                            qDebug()<<"Send Ac Angle and Adjust.";
                            // 角度发送完毕
                            SendAngleOK = true;
                        }
                    }
                }

                //*****************************
                //   角度发送完毕，开始判断是否直行
                //*****************************

                if(SendAngleOK)
                {
                    qDebug()<<"Send Angle: "<<Send_AcAngle;
                    qDebug()<<"Current Angle: "<<yaw.toFloat();

                    // 当前角度到达角度准许精度范围内
                    if(qAbs(Send_AcAngle - yaw.toFloat()) < Proper_Angle)
                    {
                        qDebug()<<"Current Angle's Error is Less Than 3 degree.";
                        count_time1++;
                        // 超过五次以上，角度到达了精度误差范围内
                        if(count_time1 >= 5)
                        {
                            // 重置角度发送完毕标志位
                            SendAngleOK = false;
                            // 重置角度采集完毕标志位，再次采水声角度值
                            AngleOK = false;
                            // 如果认为到达了正对方向（两次水声方向不同），则直行3档
                            if(AcAngleFinish)
                            {
                                FirstTurn = false;
                                on_ExecuteSend(M_F3);
                                qDebug()<<"Ac Angle OK & Go Straight with Gear 3.";
                            }
                            count_time1 = 0;
                        }
                    }
                    // 当前角度没有到达角度准许精度范围内，重新发送新周期解算出的角度
                    else
                    {
                        Send_Angle(Send_AcAngle);
                        count_time1 = 0;
                    }

                    qDebug()<<count_time1;
                }

                //*****************************
                //     直行情况的判断
                //*****************************

                // 在Straight判断中，角度的调整为逐步渐进调整
                if(Straight)
                {
                    // 第一次
                    if(!FirstTurn)
                        FirstTurn = true;
                    // 二次后的直行
                    else
                    {
                        // 在该大周期内，发送微调后的水声角度，并在最后令Straight标志位置为假，跳出该次大周期
                        if(!MF)
                        {
                            // 如果该周期的水声方向与上一周期的方向相同，则做较大角度的处理
                            if(LastAcDir == AcDir)
                            {
                                if(AcDir == 11)
                                    Send_AcAngle = yaw.toFloat() + 10;
                                if(AcDir == 22)
                                    Send_AcAngle = yaw.toFloat() - 10;
                                qDebug()<<"Last & Current Direction are SAME.";
                            }
                            // 如果该周期的水声方向与上一周期的方向不同，则做较小角度的处理
                            else
                            {
                                if(AcDir == 11)
                                    Send_AcAngle = yaw.toFloat() + 5;
                                if(AcDir == 22)
                                    Send_AcAngle = yaw.toFloat() - 5;

                                // 直行状态下，两次采到的水声方向不同，可认为到达了正对方向
                                // 判读直行，需要令两个周期分别采到一左一右的数据，且两次数据值都小于15度
                                AcAngleFinish = true;
                                qDebug()<<"Last & Current Direction are DIFFERENT && GO STRAIGHT.";
                            }

                            // 角度控制在0--360度内
                            if(Send_AcAngle > 360)
                                Send_AcAngle = Send_AcAngle - 360;
                            if(Send_AcAngle < 0)
                                Send_AcAngle = 360 + Send_AcAngle;

                            Send_Angle(Send_AcAngle);
                            qDebug()<<"as1";
                            SendAngleOK = true;
                            //
                            Straight = false;
                        }
                        // 如果此时主推正在直行，则AngleOK置为假，即直行过程中一直在更新水声当前角度
                        else
                            AngleOK = false;
                    }
                    Straight_Times = 0;
                }
            }
            else
            {
                //**********************************************************
                //                 深度处理部分
                //**********************************************************

                if(!Oct_FwdDone)
                {
                    if(FUnderAcAngle2 > 10)
                    {
                        if(SF)
                            on_ExecuteSend(S_S);
                        else
                            on_ExecuteSend(M_F2);
                    }
                    else
                        Oct_FwdDone = true;
                }
                else
                {
                    if(Oct_VerLastAcDir != Oct_VerAcDir)
                    {
                        if(SF)
                            on_ExecuteSend(S_S);
                        else if(MF)
                            on_ExecuteSend(M_S);
                        else
                        {
                            // 初始水声任务为八边形，则此时应该导向投放标志物
                            if(Random_Oct)
                            {
                                if(!Oct_FloatUP_Done)
                                {
                                    if(qAbs(depth.toDouble() - 0.1) > Proper_Depth)
                                    {
                                        Send_Depth(0.1);
                                    }
                                    else
                                        Oct_FloatUP_Done = true;
                                }
                                else
                                {
                                    // 投放标志物，先下沉到2.5m左右
                                    if(qAbs(depth.toDouble() - 2.5) > Proper_Depth)
                                        Send_Depth(2.5);
                                    else
                                    {
                                        double AnchorAngle = Navigate_Angle - 30;
                                        if(AnchorAngle > 360)       AnchorAngle = AnchorAngle - 360;
                                        else if(AnchorAngle < 0)    AnchorAngle = AnchorAngle + 360;

                                        if(qAbs(yaw.toDouble() - AnchorAngle) > Proper_Angle)
                                            Send_Angle(AnchorAngle);
                                        else
                                        {
                                            // 所有标志位重置
                                            AngleOK = false;
                                            GoAngleOk = false;
                                            SendAngleOK = false;
                                            FirstTurn = false;
                                            Straight = false;
                                            AcAngleFinishb = false;

                                            Oct_FloatUP_Done = false;
                                            Oct_VerWork_Work = false;
                                            Oct_VerWork = false;

                                            iamtask = UNDERAC;
                                            UnderAc_StartTime = MainTime;
                                        }
                                    }
                                }
                            }
                            // 初始水声任务为投放标志物，则此时已经结束所有程序
                            else
                            {
                                if(MF)
                                    on_ExecuteSend(M_S);
                                else if(SF)
                                    on_ExecuteSend(S_S);
                                else
                                    iamtask = BODYBACK;
                            }
                        }
                    }
                    else
                    {
                        if(Oct_VerAcDir == 11)
                        {
                            if(MF)
                                on_ExecuteSend(M_S);
                            else
                                on_ExecuteSend(S_L2);
                        }
                        else
                        {
                            if(MF)
                                on_ExecuteSend(M_S);
                            else
                                on_ExecuteSend(S_R2);
                        }
                    }
                }
            }
        }
    }
}

void AuvAutoControl::M_RandomPinger()
{

    //*****************************
    //     数据分析处理部分
    //*****************************

    // 角度没有采集完毕
    if(!AngleOK)
    {
        // 当前接收到水声信号没有超过ACTimes(7次)的时候，继续接受水声信号并计数
        if(count_time < ACtimes)
        {
            // 水声角度44，在航行器左边
            if(UnderAcDirection1 == "44")
            {
                 Dir[count_time] = Left;
                 Ac_Angle[count_time] = FUnderAcAngle1;
                 count_time++;
                 Count_amount++;
            }
            // 水声角度66，在航行器右边
            else if(UnderAcDirection1=="66")
            {
                Dir[count_time] = Right;
                Ac_Angle[count_time] = FUnderAcAngle1;
                count_time++;
                Count_amount = Count_amount+2;
            }
        }
        // 当前接收到水声信号到达超过ACTimes(7次)的时候，处理数据并发送
        else
        {
            // 当前时刻的水声方向，更新给上一时刻水声方向
            LastAcDir = AcDir;
            // 计算计数范围之内，左右次数出现的频率
            double temp = Count_amount / ACtimes;
            // 左右次数信息清零
            Count_amount = 0;
            // 解析出的方向，在水声方向缓冲区的个数
            int u1 = 0;
            // 水声信号在左边出现次数较多
            if(temp < 1.5 )
            {
                for(int i = 0; i < ACtimes; i++)
                {
                    if(Dir[i] == Left)
                    {
                        u1++;
                        GoAngle = GoAngle + Ac_Angle[i];
                    }
                }
                // 11代表左边
                AcDir = 11;
                ui->lineEdit_acd->setText("LEFT");
            }
            // 水声信号在右边出现次数较多
            else
            {
                for(int j = 0; j < ACtimes; j++)
                {
                    if(Dir[j] == Right)
                    {
                        GoAngle = GoAngle + Ac_Angle[j];
                        u1++;
                    }
                }
                // 22代表右边
                AcDir = 22;
                ui->lineEdit_acd->setText("RIGHT");
            }

            // 计算在解算方向的角度平均值
            GoAngle = GoAngle / u1;
            ui->lineEdit_acd2->setText(QString::number(GoAngle));

            // 角度解算完毕标志位置为真
            GoAngleOk = true;
            // 重新计数
            count_time = 0;
        }
    }

    //*****************************
    //   角度解算完毕，做出对应动作
    //*****************************

    // 计算出一个大周期内的角度的情况下
    if(GoAngleOk)
    {
        // 角度较小，可以认为在前方
        // 此时角度采集完毕，不再解算角度，并将直行标志位Straight置为真
        if(GoAngle <= 15)
        {
            AngleOK = true;
            GoAngleOk = false;
            // 此后的角度调整，变为在Straight判断中的角度微调
            Straight = true;
            Straight_Times++;
            qDebug()<<"Go Angle Ok.";
        }
        // 角度较大，角度需要根据水声信号的测量值调整
        else
        {
            // 先停止主推
            if(MF)
                on_ExecuteSend(M_S);
            else
            {
                // 角度采集完毕
                AngleOK = true;
                GoAngleOk = false;
                AcAngleFinish = false;
                Straight_Times = 0;
                // 水声方向在左边，GoAngle减运算
                if(AcDir == 11)
                {
                    Send_AcAngle = yaw.toFloat() + GoAngle;
                }
                // 水声方向在右边，GoAngle减运算
                else if(AcDir == 22)
                {
                    Send_AcAngle = yaw.toFloat() - GoAngle;
                }
                // 角度控制在0--360度内
                if(Send_AcAngle > 360)
                    Send_AcAngle = Send_AcAngle - 360;
                if(Send_AcAngle < 0)
                    Send_AcAngle = 360 + Send_AcAngle;

                // 发送结算完毕的角度，调整角度
                Send_Angle(Send_AcAngle);
                qDebug()<<"Send Ac Angle and Adjust.";
                // 角度发送完毕
                SendAngleOK = true;
            }
        }
    }

    //*****************************
    //   角度发送完毕，开始判断是否直行
    //*****************************

    if(SendAngleOK)
    {
        qDebug()<<"Send Angle: "<<Send_AcAngle;
        qDebug()<<"Current Angle: "<<yaw.toFloat();

        // 当前角度到达角度准许精度范围内
        if(qAbs(Send_AcAngle - yaw.toFloat()) < Proper_Angle)
        {
            qDebug()<<"Current Angle's Error is Less Than 3 degree.";
            count_time1++;
            // 超过五次以上，角度到达了精度误差范围内
            if(count_time1 >= 5)
            {
                // 重置角度发送完毕标志位
                SendAngleOK = false;
                // 重置角度采集完毕标志位，再次采水声角度值
                AngleOK = false;
                // 如果认为到达了正对方向（两次水声方向不同），则开始判断下个任务
                if(AcAngleFinish)
                {
                    FirstTurn = false;
//                    on_ExecuteSend(M_F3);
//                    qDebug()<<"Ac Angle OK & Go Straight with Gear 3.";

                    IsAimToMission = true;

                }
                count_time1 = 0;
            }
        }
        // 当前角度没有到达角度准许精度范围内，重新发送新周期解算出的角度
        else
        {
            Send_Angle(Send_AcAngle);
            count_time1 = 0;
        }

        qDebug()<<count_time1;
    }

    //*****************************
    //     直行情况的判断
    //*****************************

    // 在Straight判断中，角度的调整为逐步渐进调整
    if(Straight)
    {
        // 第一次
        if(!FirstTurn)
            FirstTurn = true;
        // 二次后的直行
        else
        {
            // 在该大周期内，发送微调后的水声角度，并在最后令Straight标志位置为假，跳出该次大周期
            if(!MF)
            {
                // 如果该周期的水声方向与上一周期的方向相同，则做较大角度的处理
                if(LastAcDir == AcDir)
                {
                    if(AcDir == 11)
                        Send_AcAngle = yaw.toFloat() + 10;
                    if(AcDir == 22)
                        Send_AcAngle = yaw.toFloat() - 10;
                    qDebug()<<"Last & Current Direction are SAME.";
                }
                // 如果该周期的水声方向与上一周期的方向不同，则做较小角度的处理
                else
                {
                    if(AcDir == 11)
                        Send_AcAngle = yaw.toFloat() + 5;
                    if(AcDir == 22)
                        Send_AcAngle = yaw.toFloat() - 5;

                    // 直行状态下，两次采到的水声方向不同，可认为到达了正对方向
                    // 判读直行，需要令两个周期分别采到一左一右的数据，且两次数据值都小于15度
                    AcAngleFinish = true;
                    qDebug()<<"Last & Current Direction are DIFFERENT && GO STRAIGHT.";
                }

                // 角度控制在0--360度内
                if(Send_AcAngle > 360)
                    Send_AcAngle = Send_AcAngle - 360;
                if(Send_AcAngle < 0)
                    Send_AcAngle = 360 + Send_AcAngle;

                Send_Angle(Send_AcAngle);
                qDebug()<<"as1";
                SendAngleOK = true;
                //
                Straight = false;
            }
            // 如果此时主推正在直行，则AngleOK置为假，即直行过程中一直在更新水声当前角度
            else
                AngleOK = false;
        }
        Straight_Times = 0;
    }

    //*****************************
    //     任意水声任务判断
    //*****************************

    if(IsAimToMission)
    {
        double YawAngle = yaw.toDouble();
        // 判断是否为投放标志物任务
        if(YawAngle < Navigate_Angle
                && YawAngle > 220)
        {
            iamtask = UNDERAC;
            Random_Oct = false;
            UnderAc_StartTime = MainTime;

            // 所有标志位重置
            AngleOK = false;
            GoAngleOk = false;
            SendAngleOK = false;
            FirstTurn = false;
            Straight = false;
            AcAngleFinishb = false;
        }

        // 判断是否为八边形任务
        else if((YawAngle > Navigate_Angle && YawAngle < 360)
                || (YawAngle > 0 && YawAngle < 40))
        {
            iamtask = BURY_TREASURE;
            Random_Oct = true;
            Oct_StartTime = MainTime;

            // 所有标志位重置
            AngleOK = false;
            GoAngleOk = false;
            SendAngleOK = false;
            FirstTurn = false;
            Straight = false;
            AcAngleFinishb = false;
        }
        else
            IsAimToMission = false;

    }
}





