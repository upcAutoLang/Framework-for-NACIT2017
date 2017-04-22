#ifndef AUVAUTOCONTROL_H
#define AUVAUTOCONTROL_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class AuvAutoControl;
}

class AuvAutoControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuvAutoControl(QWidget *parent = 0);
    ~AuvAutoControl();

    void OpenFile();
    void WriteToFile();
    void CloseFile();

    void OpenComUnderAc1(QString namePort);
    void OpenComUnderAc2(QString namePort);
    void OpenComExecute(QString namePort);
    void Safety();

   // void on_UnderAcSend1(QString send1);
   // void on_UnderAcSend2(QString send1);
    void on_ExecuteSend(QString send2);

  //  int ComSend2UnderAc(QString str,int len);
    int ComSend2Execute(QString str,int len);

    int SwitchTask();
    void TaskControl();
    void Log_Write(QString source,QString log);//命令记录
    void Send_Angle(float angle);
    void Send_Depth(float depth);

    void M_Start();//开始任务
    void M_ThroughDoor();//过门任务
    void M_Scuttle_ship();

    void M_Navigate_channel();
    void M_Weight_anchor();
//    void M_Set_course();
//    void M_Bury_treasure();

    void M_Bodyback();//回收任务
    // 任意水声任务
    void M_RandomPinger();

    void StringToDec(QString str);
    void StringToDec2(QString str);

    void M_Process_UnderAC();
    void M_Bury_treasure();

    void Data_Write();//文件记录开始


private:
    Ui::AuvAutoControl *ui;
    int TimeCount;
    double MainTime;
    double DoorTime;
    int FileCount;
    int SendCount;
    QSerialPort serialUnderAc1;
    QSerialPort serialUnderAc2;
    QSerialPort serialExecute;

    QFile *datafile;
    QFile *Elogfile;
    QFile *Ulogfile;
    QFile *Debug;    //调试使用
    QFile *Camera;   //存储视频信息

        //AUV parameter
    /*double depth;
    double pitch;
    double roll;
    double yaw;*/

    QString depth;   //将变量定义为QString，方便读取和显示，如果用到计算需要重新定了类型。
    QString pitch;
    QString roll;
    QString yaw;

    // 接受下位机数据缓存区
    QString temp_exucte;
    // 接受水声数据缓存区
    QString temp_UnderAc;
    // 接受水声数据缓存区
    QString temp_UnderAc2;
    // 数据截取存储
    QString str_Ec;
    // 数据截取存储
    QString str_Ec2;
    // 水声角度值
    QString UnderAcAngle1;
    // 水声角度的方向
    QString UnderAcDirection1;
    // 解算出来的水声角度值
    float FUnderAcAngle1;

    // 水声纵向角度值
    QString UnderAcAngle2;
    // 水声纵向的方向
    QString UnderAcDirection2;
    float FUnderAcAngle2;

private slots:
    int AuvMainControl();

    void ReadFromUnderAc1();
    void ReadFromUnderAc2();
    void ReadFromExecute();

    void on_Open_Serial_clicked();
    void on_Open_Image_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_Push_F2_clicked();
    void on_Push_B2_clicked();
    void on_Push_MS_clicked();
    void on_Push_L2_clicked();
    void on_Push_R2_clicked();
    void on_Push_SS_clicked();
    void on_Push_VS_clicked();
    void on_Push_UP_clicked();
    void on_Push_AS_clicked();
    void on_Push_DS_clicked();
    void on_Mission_Stop_clicked();
    void on_Clear_1_clicked();
    void on_Clear_2_clicked();
};

#endif // AUVAUTOCONTROL_H
