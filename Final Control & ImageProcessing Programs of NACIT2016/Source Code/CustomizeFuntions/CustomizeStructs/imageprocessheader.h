#ifndef IMAGEPROCESSHEADER
#define IMAGEPROCESSHEADER

/****************************************************************************
 * 结构体：HSVInfo
 *
 * 说明：内部存储HSV相关阈值上下限
/****************************************************************************
*/
struct HSVInfo
{
    int HMin;
    int HMax;
    int SMin;
    int SMax;
    int VMin;
    int VMax;
};

/****************************************************************************
 * 结构体：RGBInfo
 *
 * 说明：内部存储RGB相关阈值上下限
/****************************************************************************
*/
struct RGBInfo
{
    int RMin;
    int RMax;
    int GMin;
    int GMax;
    int BMin;
    int BMax;
};

#endif // IMAGEPROCESSHEADER

