#include "ncstage_basic.h"

/*================================================================
 * 构造函数: NCStage_START()
 =================================================================
 */
NCStage_START::NCStage_START()
{
    // 激活该Stage
    Active();
    MaxTime = 0;
    isNoSwitch = true;
}

/*================================================================
 * 函数：SetDefaultStagesName()
 *
 * 功能：设置默认的当前Stage名称与目标Stage名称
 *      Stage_START只需要设置Name即可，NextName并不确定，所以需要在
 * NCStage_START对象之外进行目标Stage的设置
 =================================================================
 */
void NCStage_START::SetDefaultStagesName()
{
    Name = STAGE_START;
}

/*================================================================
 * 构造函数: NCStage_END(NCStage lastStage, Mission initMission, int maxT)
 *
 * 参数：
 *      NCStage lastStage: 上一个NCStage对象，用于获取上一个NCStage对象的信息；
 *      Mission initMission: 设置当前Stage对象的Mission，并在构造函数中判断与上一
 * NCStage的Mission是否相同；
 *      int maxT:  当前Stage的最大运行时间
 * 注：
 *      该构造函数主要为其基础派生类NCStage_START, NCStage_END而定义，主要
 * 还是使用构造函数：NCStage(NCStage lastStage, Mission initMission, int maxT)
 =================================================================
 */
NCStage_END::NCStage_END(NCStage stage, Mission initMission, int maxT)
{
    // 激活该Stage
    Active();
    MaxTime = 0;
    isNoSwitch = true;
    CurMission = initMission;

    isMissionSame(stage);
    setCurMissionName();
}

/*================================================================
 * 函数：SetDefaultStagesName()
 *
 * 功能：设置默认的当前Stage名称与目标Stage名称
 *      Stage_END的Name为STAGE_END，NextName不需要设置，所以此处设置了
 * STAGE_NULL缺省值模拟空指针；
 =================================================================
 */
void NCStage_END::SetDefaultStagesName()
{
    Name = STAGE_END;
    NextName = STAGE_NULL;
}

