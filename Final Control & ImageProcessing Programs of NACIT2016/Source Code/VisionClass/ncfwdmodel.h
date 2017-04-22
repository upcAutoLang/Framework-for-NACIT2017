#ifndef NCFWDMODEL_H
#define NCFWDMODEL_H

#include <ncmodel.h>

class NCFwdModel : public NCModel
{
public:
    NCFwdModel();
    // 初始化参数，用给派生类NCFwdModel的构造函数；
    void InitFwdModel();
    // 计算在图像坐标系中航行器长、宽、高的映射大小
    void CalcuDimenInCam();
    // 估算摄像机与门之间的距离
    void CalcuEstDoorDis();
    // 估算摄像机与鱼雷切板之间的距离
    void CalcuEstTorpDis();
    // 设置图像坐标系中竖直杠的长度；
    void setLenDoorVerInCam(double src);
    // 设置图像坐标系中水平杠的长度；
    void setLenDoorLevelInCam(double src);
    // 设置图像坐标系中鱼雷切板大孔的长度；
    void setWidTorpBigHoleInCam(double src);
    // 设置图像坐标系中鱼雷切板小孔的长度；
    void setWidTorpSmallHoleInCam(double src);
    // 设置图像坐标系中鱼雷整体尺寸的长度；
    void setWidTorpWholeInCam(double src);

    // 获取估测的门与摄像头之间的距离
    void getEstDis(double &dst);

protected:
    // 过门任务，实际竖直、水平杠长度(v: vertical; l: level; r: real;)
    double WID_door_vr, LEN_door_lr;
    // 过门任务，图像坐标系中竖直、水平杠长度(v: vertical; l: level; v: Vision)
    double wid_door_vv, len_door_lv;
    // 过门任务，与门之间估测的距离(cd: camera, Door; est: estimate)
    double dis_cd_est;
    // 鱼雷任务，图像坐标系中大、小孔口径，整体尺寸；
    double wid_torp_big_v;
    double wid_torp_sma_v;
    double wid_torp_who_v;
    // 鱼雷任务，实际大、小孔口径，整体尺寸；
    double WID_torp_big_r;
    double WID_torp_sma_r;
    double WID_torp_who_r;
};

#endif // NCFWDMODEL_H
