#include "ncfwdmodel.h"

/*===================================================================
 * 构造函数：NCFwdModel()
 *
 * 说明：
 *  将前视摄像头中数据初始化；
/*===================================================================
 */
NCFwdModel::NCFwdModel()
{
    // 从基类NCModel中传值；
    InitBaseModel();
    // 对派生类NCFwdModel的参数初始化
    InitFwdModel();
}



/*===================================================================
 * 函数：CalcuDimenInCam()
 *
 * 说明：
 *  计算在图像坐标系中航行器长、宽、高的映射大小；
 *  基本公式：l = C * L / Z;
/*===================================================================
 */
void NCFwdModel::CalcuDimenInCam()
{
    // 若C值没有传入，则返回
    if(C == 0)
    {
        cout<<"Down Model Have no const C !"<<endl;
        return ;
    }

    // 存在C值的前提下，求航行器图像坐标系中长、宽、高（高并没有什么作用）；
//    Calcul(len_vc, LEN_vr, h_cl);
//    Calcul(wid_vc, WID_vr, h_cl);
//    Calcul(h_vc,  H_vr, h_cl);
    Calcul(wid_door_vv, WID_door_vr, dis_cd_est);
    Calcul(len_door_lv, LEN_door_lr, dis_cd_est);
}

/*===================================================================
 * 函数：setLenDoorVerInCam(double src)
 *
 * 说明：
 *  设置图像坐标系中竖直杠的长度；
/*===================================================================
 */
void NCFwdModel::setLenDoorVerInCam(double src)
{
    wid_door_vv = src;
}

/*===================================================================
 * 函数：setLenDoorLevelInCam(double src)
 *
 * 说明：
 *  设置图像坐标系中水平杠的长度；
/*===================================================================
 */
void NCFwdModel::setLenDoorLevelInCam(double src)
{
    len_door_lv = src;
}

/*===================================================================
 * 函数：setWidTorpBigHoleInCam(double src)
 *
 * 说明：
 *  设置图像坐标系中鱼雷切板大孔的长度；
/*===================================================================
 */
void NCFwdModel::setWidTorpBigHoleInCam(double src)
{
    wid_torp_big_v = src;
}

/*===================================================================
 * 函数：setWidTorpSmallHoleInCam(double src)
 *
 * 说明：
 *  设置图像坐标系中鱼雷切板小孔的长度；
/*===================================================================
 */
void NCFwdModel::setWidTorpSmallHoleInCam(double src)
{
    wid_torp_sma_v = src;
}

/*===================================================================
 * 函数：setWidTorpWholeInCam(double src)
 *
 * 说明：
 *  设置图像坐标系中鱼雷整体尺寸的长度；
/*===================================================================
 */
void NCFwdModel::setWidTorpWholeInCam(double src)
{
    wid_torp_who_v = src;
}

/*===================================================================
 * 函数：CalcuEstDoorDis()
 *
 * 说明：
 *  估算摄像机与门之间的距离；
 *  基本公式：l = C * L / Z;
/*===================================================================
 */
void NCFwdModel::CalcuEstDoorDis()
{
    // 若C值没有传入，则返回
    if(C == 0)
    {
        cout<<"Down Model Have no const C !"<<endl;
        return ;
    }
    // 若没有图像坐标系中竖直、水平杠的长度，则返回
    if(wid_door_vv == 0 || len_door_lv == 0)
    {
        cout<<"Length of Vertical or Level Door is NULL!"<<endl;
        return ;
    }

    // 估测的与门之间的距离，由竖直杠相关数据算出；
    CalcuZ(dis_cd_est, wid_door_vv, WID_door_vr);
}

/*===================================================================
 * 函数：CalcuEstTorpDis()
 *
 * 说明：
 *  估算摄像机与鱼雷切板之间的距离；
 *  基本公式：l = C * L / Z;
/*===================================================================
 */
void NCFwdModel::CalcuEstTorpDis()
{
    // 若C值没有传入，则返回
    if(C == 0)
    {
        cout<<"Down Model Have no const C !"<<endl;
        return ;
    }
    // 若没有图像坐标系中竖直、水平杠的长度，则返回
    if(wid_torp_who_v == 0)
    {
        cout<<" is NULL!"<<endl;
        return ;
    }
    CalcuZ(dis_cd_est, wid_torp_who_v, WID_torp_who_r);
}

/*===================================================================
 * 函数：getEstDis(double &dst)
 *
 * 说明：
 *  获取被测目标与摄像头之间的距离；
/*===================================================================
 */
void NCFwdModel::getEstDis(double &dst)
{
    dst = dis_cd_est;
}
