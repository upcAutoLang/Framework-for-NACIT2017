#ifndef NCCLIENT_IMAGEMACROS_H
#define NCCLIENT_IMAGEMACROS_H

/*===============================================================
 * 字符串定义：图像客户端命令
 *
 * 说明：
 *      测试使用，目前设置了三个任务：
 *      1. RAW: 不作处理
 *      2. BGR2GRAY: 处理成灰度图
 *      3. BGR2BIN: 原图处理成二值图
 ================================================================
 */
#define IMG_CMD_NO        "RAW"
#define IMG_CMD_GRAY    "BGR2GRAY"
#define IMG_CMD_BIN      "BGR2BIN"

/*===============================================================
 * 字符串定义：图像客户端显示窗口名称
 *
 * 说明：
 *      目前测试使用，设置了三个窗口：
 *      显示处理后的结果，便于调试使用：
 *      1. Frame: 原图像
 *      2. Gray Image: 灰度图
 *      3. Bin Image: 二值图
 ================================================================
 */
#define IMG_WIN_NO      "Frame"
#define IMG_WIN_GRAY    "Gray Image"
#define IMG_WIN_BIN      "Bin Image"

/*===============================================================
 * 枚举：图像任务
 *
 * 说明：
 *      目前测试使用，设置了三个图像任务：
 *      1. IMG_NO: 不作处理
 *      2. IMG_GRAY: 灰度图处理
 *      3. IMG_BIN: 二值化处理
 ================================================================
 */
enum Image_Mission
{
    IMG_NO,
    IMG_GRAY,
    IMG_BIN
};

#endif // NCCLIENT_IMAGEMACROS_H
