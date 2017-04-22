#ifndef NCVINFOREPLAY_H
#define NCVINFOREPLAY_H

#include <ncheader.h>
#include <GeneralImageProcess/draw_fun.h>

class NCVInfoReplay
{
public:
    NCVInfoReplay();
    void setVisionInfo(VisionInfo Info);
    void getVisionInfo(VisionInfo &dst);

    // 从文件中读取信息集合，并存储在内存中
    void ReadVisionInfo();
    // 从输入的string获取信息，并存入VInfo中；
    void setVisionInfoByString(string str);
    // 设置VisionInfo文件存储路径
    void setVInfoFilePath(string path);
    // 重绘一帧图像
    void ReplayOneVInfo();
    // 打开VisionInfo记录文件
    void OpenVInfoRecordFile();
    // 绘制图像
    void Replay();

    void test();

    void ShowOneFrame();

private:
    int StrNum;
    VisionInfo VInfo;
    string VInfoPath;
    Mat DownImg;
    Mat FwdImg;
    ifstream VInfoSrc;
    vector<string> InfoString;
};

#endif // NCVINFOREPLAY_H
