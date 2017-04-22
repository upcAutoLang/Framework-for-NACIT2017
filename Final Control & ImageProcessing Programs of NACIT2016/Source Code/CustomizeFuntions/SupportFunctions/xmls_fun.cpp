#include <SupportFunctions/xmls_fun.h>

/*===================================================================
 * 函数名：GetHSVInfoFromXML
 *
 * 功能：从XML文件中获取HSV阈值信息，并将信息读取在内存中
 *
 * 参数：
 *  string fileName:       需要读取的XML文件名
 *  int &HMin:             色相Hue最小值
 *  int &HMax:             色相Hue最大值
 *  int &SMin:             饱和度Saturation最小值
 *  int &SMax:             饱和度Saturation最大值
 *  int &VMin:             亮度Value最小值
 *  int &VMax:             亮度Value最大值
=====================================================================
*/
void GetHSVInfoFromXML(string fileName, int &HMin, int &HMax,
                       int &SMin, int &SMax, int &VMin, int &VMax)
{
    FileStorage fs(fileName, FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open XML Error: "<<fileName<<" is Wrong!"<<endl;
        return ;
    }

    fs["HMin"] >> HMin;
    fs["HMax"] >> HMax;
    fs["SMin"] >> SMin;
    fs["SMax"] >> SMax;
    fs["VMin"] >> VMin;
    fs["VMax"] >> VMax;

    fs.release();
}

/*===================================================================
 * 函数名：GetHSVInfoFromXML
 *
 * 功能：从XML文件中获取HSV阈值信息，并将信息读取在内存中
 *
 * 参数：
 *  string fileName:       需要读取的XML文件名
 *  HSVInfo &Info:         HSV阈值
=====================================================================
*/
void GetHSVInfoFromXML(string fileName, HSVInfo &Info)
{
    FileStorage fs(fileName, FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open XML Error: "<<fileName<<" is Wrong!"<<endl;
        return ;
    }

    fs["HMin"] >> Info.HMin;
    fs["HMax"] >> Info.HMax;
    fs["SMin"] >> Info.SMin;
    fs["SMax"] >> Info.SMax;
    fs["VMin"] >> Info.VMin;
    fs["VMax"] >> Info.VMax;

    fs.release();
}

/*===================================================================
 * 函数名：GetRGBInfoFromXML
 *
 * 功能：从XML文件中获取RGB阈值信息，并将信息读取在内存中
 *
 * 参数：
 *  string fileName:       需要读取的XML文件名
 *  int &RMin:             红色通道Red最小值
 *  int &RMax:             红色通道Red最大值
 *  int &GMin:             绿色通道Green最小值
 *  int &GMax:             绿色通道Green最大值
 *  int &BMin:             蓝色通道Blue最小值
 *  int &BMax:             蓝色通道Blue最大值
=====================================================================
*/
void GetRGBInfoFromXML(string fileName, int &RMin, int &RMax,
                       int &GMin, int &GMax, int &BMin, int &BMax)
{
    FileStorage fs(fileName, FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open XML Error: "<<fileName<<" is Wrong!"<<endl;
        return ;
    }

    fs["RMin"] >> RMin;
    fs["RMax"] >> RMax;
    fs["GMin"] >> GMin;
    fs["GMax"] >> GMax;
    fs["BMin"] >> BMin;
    fs["BMax"] >> BMax;

    fs.release();
}

/*===================================================================
 * 函数名：GetRGBInfoFromXML
 *
 * 功能：从XML文件中获取RGB阈值信息，并将信息读取在内存中
 *
 * 参数：
 *  string fileName:       需要读取的XML文件名
 *  RGBInfo &Info:         RGB阈值上下限信息
=====================================================================
*/
void GetRGBInfoFromXML(string fileName, RGBInfo &Info)
{
    FileStorage fs(fileName, FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open XML Error: "<<fileName<<" is Wrong!"<<endl;
        return ;
    }

    fs["RMin"] >> Info.RMin;
    fs["RMax"] >> Info.RMax;
    fs["GMin"] >> Info.GMin;
    fs["GMax"] >> Info.GMax;
    fs["BMin"] >> Info.BMin;
    fs["BMax"] >> Info.BMax;

    fs.release();
}

/*===================================================================
 * 函数名：GetJoinMinusInfoFromXML
 *
 * 功能：从XML文件中获取并减运算相关HSV、RGB阈值信息，并将信息读取在内存中
 *
 * 参数：
 *  string fileName:        需要读取的XML文件名
 *  HSVInfo &HSVInfo1:      目标物体HSV阈值上下限信息1
 *  HSVInfo &HSVInfo2:      目标物体HSV阈值上下限信息2
 *  RGBInfo &RGBInfo1:      噪声RGB阈值上下限信息1
 *  RGBInfo &RGBInfo2:      噪声RGB阈值上下限信息2
=====================================================================
*/
void GetJoinMinusInfoFromXML(string fileName, HSVInfo &HSVInfo1, HSVInfo &HSVInfo2,
                          RGBInfo &RGBInfo1, RGBInfo &RGBInfo2)
{
    FileStorage fs(fileName, FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open XML Error: "<<fileName<<" is Wrong!"<<endl;
        return ;
    }

    fs["HMin_Info1"] >> HSVInfo1.HMin;
    fs["HMax_Info1"] >> HSVInfo1.HMax;
    fs["SMin_Info1"] >> HSVInfo1.SMin;
    fs["SMax_Info1"] >> HSVInfo1.SMax;
    fs["VMin_Info1"] >> HSVInfo1.VMin;
    fs["VMax_Info1"] >> HSVInfo1.VMax;

    fs["HMin_Info2"] >> HSVInfo2.HMin;
    fs["HMax_Info2"] >> HSVInfo2.HMax;
    fs["SMin_Info2"] >> HSVInfo2.SMin;
    fs["SMax_Info2"] >> HSVInfo2.SMax;
    fs["VMin_Info2"] >> HSVInfo2.VMin;
    fs["VMax_Info2"] >> HSVInfo2.VMax;

    fs["RMin_Info1"] >> RGBInfo1.RMin;
    fs["RMax_Info1"] >> RGBInfo1.RMax;
    fs["GMin_Info1"] >> RGBInfo1.GMin;
    fs["GMax_Info1"] >> RGBInfo1.GMax;
    fs["BMin_Info1"] >> RGBInfo1.BMin;
    fs["BMax_Info1"] >> RGBInfo1.BMax;

    fs["RMin_Info2"] >> RGBInfo2.RMin;
    fs["RMax_Info2"] >> RGBInfo2.RMax;
    fs["GMin_Info2"] >> RGBInfo2.GMin;
    fs["GMax_Info2"] >> RGBInfo2.GMax;
    fs["BMin_Info2"] >> RGBInfo2.BMin;
    fs["BMax_Info2"] >> RGBInfo2.BMax;
}

/*===================================================================
 * 函数名：GetCameraCalibInfoFromXML
 *
 * 功能：根据输入的XML路径，输出对应XML路径的摄像头相关标定信息：内参数矩阵、畸变矩阵
 *
 * 参数：
 *  string XML_address:           需要读取的XML文件名
 *  Size &FrameSize:              被处理帧的尺寸信息
 *  Mat &IntrinMat:               内参数矩阵
 *  Mat &DistortMat:              畸变矩阵
=====================================================================
*/
void GetCameraCalibInfoFromXML(string XML_address, Size &FrameSize, Mat &IntrinMat, Mat &DistortMat)
{
    // 摄像机尺寸
    int width, height;
    // 从XML文件中读取浮点型数据，并保存在FileStorage类型的对象中
    FileStorage fs(XML_address,
                   FileStorage::READ);

    if(!fs.isOpened())
    {
        cout<<"Open Calib XML file Error: No found XML file!"<<endl;
    }

    // 读取高度、宽度、内参数矩阵、畸变矩阵
    fs["image_height"] >> height;
    fs["image_width"]  >> width;
    fs["camera_matrix"] >> IntrinMat;
    fs["distortion_coefficients"] >> DistortMat;

    FrameSize.width = width;
    FrameSize.height = height;


}
