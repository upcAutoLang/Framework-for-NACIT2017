#ifndef NCMODEL_H
#define NCMODEL_H

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

class NCModel
{
public:
    // 初始化参数，用给基类NCModel的构造函数；
    void InitBaseModel();
    NCModel();
    // 设置常量C的值
    void setConstC(double src);
    // 获取常量C的值
    void getConstC(double &dst);
    // 已知l, L, Z, 求C
    void CalcuC(double l, double L, double Z);
    // 已知C, L, Z, 求l，并传出
    void Calcul(double &dst, double L, double Z);
    // 已知C, L, Z, 求距离dst，并传出
    void CalcuZ(double &dst, double l, double L);


protected:
    // 池底到水面深度
    double H;
    // 深度传感器与池底距离（作为记录基准）
    double h_sen_bot;
//    // （这里需要改，俯视、前视摄像头距池底距离不同，应分属两个模型）
//    // 摄像头距池底距离(cb: camera, bottom)
//    double h_cb;
    // 视野角度
    double ViewAngle;
    // 航行器实际长、宽、高（vr: Vehicle Real）
    double LEN_vr, WID_vr, H_vr;
    // 航行器在图像坐标系中的长、宽、高（vv: Vehicle Vision）
    double len_vv, wid_vv, h_vv;
    // 摄像机焦距
    double f;
    // 深度传感器测值，此处为我们设定的深度
    double h_sen;
    // 深度传感器与俯视摄像头垂直方向距离(sd: sensor, Down Camera)
    double h_sen_down;
    // 深度传感器与前视摄像头前后方向距离(sf: sensor, Forward Camera)
    double l_sen_fwd;


    // 基本公式：l = C * L / Z;
    // C为距离估算公式的常量；
    double C;

};

#endif // NCMODEL_H
