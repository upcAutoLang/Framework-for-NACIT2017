#include "ncmodel.h"

/*===================================================================
 * 构造函数：NCModel()
 *
 * 说明：
 *  将俯视、前视摄像头中公用的数据初始化；
/*===================================================================
 */
NCModel::NCModel()
{
    // 初始化基类参数；
    InitBaseModel();
}


/*===================================================================
 * 函数：setConstC(double src)
 *
 * 说明：
 *  设置常量C的值；
 *
 * 参数：
 *  double src:         源数，将该值传给C；
/*===================================================================
 */
void NCModel::setConstC(double src)
{
    C = src;
}

/*===================================================================
 * 函数：getConstC(double &dst)
 *
 * 说明：
 *  获取常量C的值；
 *
 * 参数：
 *  double &dst:        目标数，将C传入该值；
/*===================================================================
 */
void NCModel::getConstC(double &dst)
{
    dst = C;
}

/*===================================================================
 * 函数：CalcuC(double l, double L, double Z)
 *
 * 说明：
 *  已知l, L, Z, 求C;
 *  基本公式：l = C * L / Z;
 *
 * 注：C对一个模型是确定的，所以该函数在整个程序中应该只调用一次；或者已经求出某模型的
 *  C值后，即将C值设为该模型的常量；
 *
 * 参数：
 *  double l:       图像坐标系中的长度；
 *  double L:       物理坐标系中的长度；
 *  double Z:       物理坐标系中摄像头与物体之间的距离；
/*===================================================================
 */
void NCModel::CalcuC(double l, double L, double Z)
{
    if(l == 0)
    {
        cout<<"Calculate C Error: No l!"<<endl;
        return ;
    }
    if(L == 0)
    {
        cout<<"Calculate C Error: No L!"<<endl;
        return ;
    }
    if(Z == 0)
    {
        cout<<"Calculate C Error: No Z!"<<endl;
        return ;
    }
    C = l * Z / L;
}

/*===================================================================
 * 函数：CalcuZ(double &dst, double l, double L)
 *
 * 说明：
 *  已知C, L, Z, 求距离dst，并传出;
 *  基本公式：l = C * L / Z;
 *
 * 参数：
 *  double &dst:    被计算值，物理坐标系中摄像头与物体之间的距离，计算出该值后传出；
 *  double l:       图像坐标系中的长度；
 *  double L:       物理坐标系中的长度；
/*===================================================================
 */
void NCModel::CalcuZ(double &dst, double l, double L)
{
    if(l == 0)
    {
        cout<<"Calculate Z Error: No l!"<<endl;
        return ;
    }
    if(L == 0)
    {
        cout<<"Calculate Z Error: No L!"<<endl;
        return ;
    }
    if(C == 0)
    {
        cout<<"Calculate Z Error: No C!"<<endl;
        return ;
    }
    dst = C * L / l;
}

/*===================================================================
 * 函数：CalcuZ(double &dst, double l, double L)
 *
 * 说明：
 *  已知C, L, Z, 求距离dst，并传出;
 *  基本公式：l = C * L / Z;
 *
 * 参数：
 *  double &dst:    被计算值，图像坐标系中的长度，计算出该值后传出；
 *  double L:       物理坐标系中的长度；
 *  double Z:       物理坐标系中摄像头与物体之间的距离；
/*===================================================================
 */
void NCModel::Calcul(double &dst, double L, double Z)
{
    if(Z == 0)
    {
        cout<<"Calculate l Error: No Z!"<<endl;
        return ;
    }
    if(L == 0)
    {
        cout<<"Calculate l Error: No L!"<<endl;
        return ;
    }
    if(C == 0)
    {
        cout<<"Calculate l Error: No C!"<<endl;
        return ;
    }
    dst = C * L / Z;
}
