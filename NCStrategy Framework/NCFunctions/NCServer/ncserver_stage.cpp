#include "ncserver.h"


/*================================================================
 * 函数：ChangeStage(NCStage *&stage, Mission mission)
 *
 * 参数：
 *      NCStage* &stage:  在CurStage之前的Stage，用其成员变量NextName为CurStage赋值；
 *      Mission mission:  设置该Stage所属的Mission
 *
 * 说明：
 *      根据CurStage的目标Stage，改变CurStage的数据类型；
 *
 * 注：
 *      随着NCStage派生类的增多，该函数需要不断扩充；
 =================================================================
 */
void NCServer::ChangeStage(NCStage *&stage, Mission mission)
{
    if(stage->getNextName() == STAGE_TEST_1)
    {
        NCStage *tmpStage = new NCStage_TEST_1(*stage, mission);
        stage = dynamic_cast<NCStage_TEST_1 *>(tmpStage);
    }
    else if(stage->getNextName() == STAGE_TEST_2)
    {
        NCStage* tmpStage = new NCStage_TEST_2(*stage, mission);
        stage = dynamic_cast<NCStage_TEST_2 *>(tmpStage);
    }
    else if(stage->getNextName() == STAGE_TEST_3)
    {
        NCStage* tmpStage = new NCStage_TEST_3(*stage, mission);
        stage = dynamic_cast<NCStage_TEST_3 *>(tmpStage);
    }
    else if(stage->getNextName() == STAGE_END)
    {
        NCStage* StageEnd = new NCStage_END(*stage, mission);
        stage = dynamic_cast<NCStage_END *>(StageEnd);
    }
}

/*================================================================
 * 函数：SetStartStage(NCStage* &stage, Mission mission)
 *
 * 参数：
 *      NCStage* &stage:        将传入的NCStage设置为NCStage_START；
 *      Mission mission:         设置该Stage所属的Mission
 *
 * 说明：
 *      设置起始Stage的参数；
 *      将当前Stage初始化为NCStage_START，并根据输入的Mission设置该Mission
 * 首先进入的Stage；
 *
 * 注：
 *      随着Mission枚举变量的增多，该函数需要不断扩充；
 =================================================================
 */
void NCServer::SetStartStage(NCStage* &stage, Mission mission)
{
    NCStage* StageStart = new NCStage_START();
    stage = dynamic_cast<NCStage_START *>(StageStart);
    // 激活该Stage
    stage->Active();
    switch(mission)
    {
    case TEST:
        stage->setNextName(STAGE_TEST_1);
        break;
    case END:
        stage->setNextName(STAGE_TEST_2);
        break;
    default:
        cout<<"Set Start Stage ERROR."<<endl;
        break;
    }
}

/*================================================================
 * 函数：getMission()
 *
 * 返回值：string
 *
 * 说明：
 *      根据CurStage的Mission值，获取当前Stage的Mission字符串；
 *      目前仅用作记录；
 *
 * 注：
 *      随着Mission枚举变量的增多，该函数需要不断扩充；
 =================================================================
 */
string NCServer::getMission()
{
    string dst;
    switch(CurStage->getMission())
    {
    case TEST:
        dst = MISSION_TEST;
        break;
    case END:
        dst = MISSION_END;
        break;
    default:
        dst = "ERROR_MISSION";
        break;
    }
    return dst;
}

