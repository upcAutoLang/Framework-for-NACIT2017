#ifndef NCSTAGE_TEST_H
#define NCSTAGE_TEST_H

#define STAGE_TEST_1     "TEST_INT"
#define STAGE_TEST_2     "TEST_DOUBLE"
#define STAGE_TEST_3     "TEST_CHAR*"

#include "ncstage.h"

class NCStage_TEST_1 : public NCStage
{
public:
    NCStage_TEST_1(NCStage lastStage, Mission initMission, int maxT = 10);
//    void Run();

    // 初始化该Stage的数据
    virtual void InitStageData();
    // 该Stage的数据分析阶段
    virtual void StageFunction();
    // 对更换目标Stage的条件、名称进行初始化
    virtual void InitChangeNextStage();
    // 对更换目标Stage的条件、名称进行更新
    virtual void UpdateChangeNextStage();
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();

protected:
    // 测试使用数据，num_int从0-10逐步增加
    int num_int;
    int num_rand;
};

class NCStage_TEST_2 : public NCStage
{
public:
    NCStage_TEST_2(NCStage lastStage, Mission initMission, int maxT = 10);

    // 初始化该Stage的数据
    virtual void InitStageData();
    // 该Stage的数据分析阶段
    virtual void StageFunction();
    // 对更换目标Stage的条件、名称进行初始化
    virtual void InitChangeNextStage();
    // 对更换目标Stage的条件、名称进行更新
    virtual void UpdateChangeNextStage();
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();

protected:
    // 测试使用数据，num_double从0.01--0.20逐步增加
    double num_double;
};

class NCStage_TEST_3 : public NCStage
{
public:
    NCStage_TEST_3(NCStage lastStage, Mission initMission, int maxT = 10);

    // 初始化该Stage的数据
    virtual void InitStageData();
    // 该Stage的数据分析阶段
    virtual void StageFunction();
    // 对更换目标Stage的条件、名称进行初始化
    virtual void InitChangeNextStage();
    // 对更换目标Stage的条件、名称进行更新
    virtual void UpdateChangeNextStage();
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();

protected:
    // 测试使用数据，num_char从'A'---'Z'逐步增加
    char num_char;
    int num_rand;
};

#endif // NCSTAGE_TEST_H
