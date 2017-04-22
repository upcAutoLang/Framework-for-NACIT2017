#ifndef NCSTAGE_BASIC_H
#define NCSTAGE_BASIC_H

#include "ncstage.h"
#include "ncstage_macro.h"

class NCStage_START : public NCStage
{
public:
//    void Run();
    NCStage_START();
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();
};

class NCStage_END : public NCStage
{
public:
//    void Run();
    NCStage_END(NCStage stage, Mission initMission, int maxT = 0);
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();
};

#endif // NCSTAGE_BASIC_H
