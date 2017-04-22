#include "ncdownmodel.h"

/*===================================================================
 * 构造函数：NCDownModel()
 *
 * 说明：
 *  将俯视摄像头中数据初始化；
/*===================================================================
 */
NCDownModel::NCDownModel()
{
    // 从基类NCModel中传值；
    InitBaseModel();
    // 对派生类NCDownModel的参数初始化
    InitDownModel();
}

/*===================================================================
 * 函数：CalcuDimenInCam()
 *
 * 说明：
 *  计算在图像坐标系中航行器长、宽、高的映射大小；
 *  基本公式：l = C * L / Z;
/*===================================================================
 */
void NCDownModel::CalcuDimenInCam()
{
    // 若C值没有传入，则返回
    if(C == 0)
    {
        cout<<"Down Model Have no const C !"<<endl;
        return ;
    }

    // 更新俯视摄像头深度；
    CalcuDownCamDepth();

    // 存在C值的前提下，求航行器图像坐标系中长、宽、高（高并没有什么作用）；
    Calcul(len_vv, LEN_vr, h_cam_line);
    Calcul(wid_vv, WID_vr, h_cam_line);
    Calcul(h_vv,  H_vr, h_cam_line);
}

/*===================================================================
 * 函数：CalcuLenLineInCam()
 *
 * 说明：
 *  计算图像坐标系中，导引路径原本应有的长、宽；
 *  已知l, L, Z, 求C;
 *  基本公式：l = C * L / Z;
 *
 * 注：
 *  在图像坐标系中，会实时求出导引路径的长、宽；
 *  此时我们已经设定深度H，已知
 *  导引路径实际长宽，以及俯视摄像头模型的C值，即可求出理论上原本应该有的长、宽；
 *  用这里的长宽值为以后的实时求出的长宽值做对比，可用于判断识别的线应该被当作点，或者是线；
 *
 * 参数：
 *  double l:       图像坐标系中的长度；
 *  double L:       物理坐标系中的长度；
 *  double Z:       物理坐标系中摄像头与物体之间的距离；
/*===================================================================
 */
void NCDownModel::CalcuLenLineInCam()
{
    if(C == 0)
    {
        cout<<"Down Model Have no const C !"<<endl;
        return ;
    }

    // 更新俯视摄像头深度；
    CalcuDownCamDepth();

    // 存在C值的前提下，求图像坐标系中导引路径原本应有的长、宽；
    Calcul(len_line_sh, LEN_line, h_cam_line);
    Calcul(wid_line_sh, WID_line, h_cam_line);
}

/*===================================================================
 * 函数：CalcuDownCamDepth()
 *
 * 说明：
 *  计算俯视摄像头深度，包括摄像头与导引路径距离、摄像头与池底距离；
/*===================================================================
 */
void NCDownModel::CalcuDownCamDepth()
{
    // 摄像头距离导引路径距离
    h_cam_line = H - h_sen - h_sen_down - h_line_bot;
    h_cam_bot  = h_cam_line + h_line_bot;
}

/*===================================================================
 * 函数：setSensorHeight(double src)
 *
 * 说明：
 *  改变设定工作深度（传感器深度值），并更新俯视摄像头相关深度信息；
 *
 * 参数：
 *  double src:     当前传感器深度值；
/*===================================================================
 */
void NCDownModel::setSensorHeight(double src)
{
    h_sen = src;
    // 更新俯视摄像头深度；
    CalcuDownCamDepth();
}

/*===================================================================
 * 函数：getLineWidthInCam(double &dst)
 *
 * 说明：
 *  获取图像坐标系中导引路径应有的宽度；
 *  基本公式：l = C * L / Z;
 *
 * 参数：
 *  double &dst:     图像坐标系中导引路径应有的宽度；
/*===================================================================
 */
void NCDownModel::getLineWidthInCam(double &dst)
{
    Calcul(wid_line_sh, WID_line, h_cam_line);
    dst = wid_line_sh;
}

/*===================================================================
 * 函数：getLineLengthInCam(double &dst)
 *
 * 说明：
 *  获取图像坐标系中导引路径应有的长度；
 *  基本公式：l = C * L / Z;
 *
 * 参数：
 *  double &dst:     图像坐标系中导引路径应有的长度
/*===================================================================
 */
void NCDownModel::getLineLengthInCam(double &dst)
{
    Calcul(len_line_sh, LEN_line, h_cam_line);
    dst = len_line_sh;
}
