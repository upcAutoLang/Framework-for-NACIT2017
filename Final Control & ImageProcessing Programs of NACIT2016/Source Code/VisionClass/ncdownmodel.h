#ifndef NCDOWNMODEL_H
#define NCDOWNMODEL_H

#include <ncmodel.h>

class NCDownModel : public NCModel
{
public:
    NCDownModel();
    // 初始化参数，用给派生类NCDownModel的构造函数；
    void InitDownModel();
    // 计算在图像坐标系中航行器长、宽、高的映射大小
    void CalcuDimenInCam();
    // 计算图像坐标系中，导引路径原本应有的长、宽（用于往后的比较）
    void CalcuLenLineInCam();
    // 计算俯视摄像头深度；
    void CalcuDownCamDepth();

    //*************************************
    //          set, get
    //*************************************

    // 改变设定工作深度（传感器深度值）
    void setSensorHeight(double src);

    // 获取图像坐标系中导引路径应有的宽度
    void getLineWidthInCam(double &dst);
    // 获取图像坐标系中导引路径应有的长度
    void getLineLengthInCam(double &dst);


protected:
    // 俯视摄像头距离导引路径距离(cl: camera, line)
    double h_cam_line;
    // 俯视摄像头距池底距离
    double h_cam_bot;
    // 路径距池底高度(lb: line, bottom)
    double h_line_bot;
    // 导引路径实际长度、宽度(l: line)
    double LEN_line, WID_line;
    // 导引路径在图像坐标系中原本应有的长度、宽度(l_sh: line, should)
    double len_line_sh, wid_line_sh;
    // 图像坐标系下视角范围半径大小
    double r;

};

#endif // NCDOWNMODEL_H
