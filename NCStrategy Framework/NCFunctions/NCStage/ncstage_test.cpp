#include "ncstage_test.h"

NCStage_TEST_1::NCStage_TEST_1(NCStage stage, Mission initMission, int maxT)
{
    // 激活该Stage
    Active();
    MaxTime = maxT;
    isNoSwitch = false;
    CurMission = initMission;

    setCurMissionName();
    isMissionSame(stage);
}

void NCStage_TEST_1::InitStageData()
{
    num_int = 0;
    num_rand = 0;
}

void NCStage_TEST_1::StageFunction()
{
    cout<<"Waiting your Enter: ";
    usleep(5e4);
    num_int++;
    srand((int)time(0));
    num_rand += rand()%15;
    cout<<"  num_int = "<<num_int<<endl;
    cout<<"  num_ran = "<<num_rand<<endl;
}

// 对更换目标Stage的条件、名称进行初始化
void NCStage_TEST_1::InitChangeNextStage()
{
    ChangeCondition.clear();
    ChangeName.clear();

    // 目前手动添加ChangeCondition, ChangeName
    // 且在InitChangeNextStage中，默认所有Condition都为false
    // 测试阶段，此处只添加一个Condition与Name
    ChangeCondition.push_back(false);
    ChangeName.push_back(NextName);

    // 如果isNoSwitch为真，则默认该Stage只能顺序执行，不能选择，
    //      此时该Stage的目标Stage只能为初始化时的目标Stage；
    // 如果isNoSwitch为假，则说明该Stage的有其他可供选择的目标Stage，
    //      这样的话则可添加其他的目标Stage以及对应的满足条件；
    if(!isNoSwitch)
    {
        // 警告没有添加目标Stage信息的派生类
        WarningDerivedNextStage();

        ChangeCondition.push_back(false);
        ChangeName.push_back(STAGE_TEST_3);
    }
}

void NCStage_TEST_1::UpdateChangeNextStage()
{
    // 目前手动写入ChangeCondition的更新
    // 测试阶段，此处只添加一个Condition与Name
    if(0 < ChangeCondition.size())
        ChangeCondition[0] = (num_int >= 10);

    if(!isNoSwitch)
    {
        if(1 < ChangeCondition.size())
            ChangeCondition[1] = (num_rand >= 70);

        // 警告没有添加目标Stage信息的派生类
        WarningDerivedNextStage();
    }
}

// 设置默认的当前Stage名称与目标Stage名称
void NCStage_TEST_1::SetDefaultStagesName()
{
    Name = STAGE_TEST_1;
    // 默认目标Stage
    NextName = STAGE_TEST_2;
}

NCStage_TEST_2::NCStage_TEST_2(NCStage stage, Mission initMission, int maxT)
{
    // 激活该Stage
    Active();
    MaxTime = maxT;
    isNoSwitch = true;
    CurMission = initMission;

    setCurMissionName();
    isMissionSame(stage);
}

// 初始化该Stage的数据
void NCStage_TEST_2::InitStageData()
{
    num_double = 0.01;
}

void NCStage_TEST_2::StageFunction()
{
    cout<<"Waiting your Enter: ";
    usleep(5e4);
    num_double+=0.01;
    cout<<"  num_double = "<<num_double<<endl;
}

// 对更换目标Stage的条件、名称进行初始化
void NCStage_TEST_2::InitChangeNextStage()
{
    ChangeCondition.clear();
    ChangeName.clear();

    // 目前手动添加ChangeCondition, ChangeName
    // 且在InitChangeNextStage中，默认所有Condition都为false
    // 测试阶段，此处只添加一个Condition与Name
    ChangeCondition.push_back(false);
    ChangeName.push_back(NextName);
}

// 对更换目标Stage的条件、名称进行更新
void NCStage_TEST_2::UpdateChangeNextStage()
{
    // 目前手动写入ChangeCondition的更新
    // 测试阶段，此处只添加一个Condition与Name
    if(0 < ChangeCondition.size())
        ChangeCondition[0] = (num_double > 0.20);
}

// 设置默认的当前Stage名称与目标Stage名称
void NCStage_TEST_2::SetDefaultStagesName()
{
    Name = STAGE_TEST_2;
    // 默认目标Stage
    NextName = STAGE_TEST_3;
}

NCStage_TEST_3::NCStage_TEST_3(NCStage stage, Mission initMission, int maxT)
{
    // 激活该Stage
    Active();
    MaxTime = maxT;
    isNoSwitch = false;
    CurMission = initMission;

    setCurMissionName();
    isMissionSame(stage);

}

void NCStage_TEST_3::InitStageData()
{
    num_char = 'A';
    num_rand = 0;
}

void NCStage_TEST_3::StageFunction()
{
    cout<<"Waiting your Enter: ";
    usleep(5e4);
    num_char++;
    srand((int)time(0));
    num_rand += rand()%15;
    cout<<"  num_char = "<<num_char<<endl;
    cout<<"  num_ran = "<<num_rand<<endl;
}

void NCStage_TEST_3::InitChangeNextStage()
{
    ChangeCondition.clear();
    ChangeName.clear();

    // 目前手动添加ChangeCondition, ChangeName
    // 且在InitChangeNextStage中，默认所有Condition都为false
    // 测试阶段，此处只添加一个Condition与Name
    ChangeCondition.push_back(false);
    ChangeName.push_back(NextName);

    // 如果isNoSwitch为真，则默认该Stage只能顺序执行，不能选择，
    //      此时该Stage的目标Stage只能为初始化时的目标Stage；
    // 如果isNoSwitch为假，则说明该Stage的有其他可供选择的目标Stage，
    //      这样的话则可添加其他的目标Stage以及对应的满足条件；
    if(!isNoSwitch)
    {
        // 警告没有添加目标Stage信息的派生类
        WarningDerivedNextStage();

        ChangeCondition.push_back(false);
        ChangeName.push_back(STAGE_TEST_1);
    }
}

// 对更换目标Stage的条件、名称进行更新
void NCStage_TEST_3::UpdateChangeNextStage()
{
    // 目前手动写入ChangeCondition的更新
    // 测试阶段，此处只添加一个Condition与Name
    if(0 < ChangeCondition.size())
        ChangeCondition[0] = (num_char > 'Z');

    if(!isNoSwitch)
    {
        if(1 < ChangeCondition.size())
            ChangeCondition[1] = (num_rand >= 150);

        // 警告没有添加目标Stage信息的派生类
        WarningDerivedNextStage();
    }
}

// 设置默认的当前Stage名称与目标Stage名称
void NCStage_TEST_3::SetDefaultStagesName()
{
    Name = STAGE_TEST_3;
    // 默认目标Stage
    NextName = STAGE_END;
}
