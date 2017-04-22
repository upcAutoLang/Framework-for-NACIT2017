#ifndef NCSTAGE_H
#define NCSTAGE_H

#include "ncstage_macro.h"

#include <sys/time.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <unistd.h>

using namespace std;

// 阶段的运行状态
enum Stage_State
{
    STATE_RUNNING,
    STATE_CHANGE_NEXT,
    STATE_DONE_TIMEUP_STAGE,
    STATE_DONE_TIMEUP_MISSION,
    STATE_DONE_NORMAL
};

// 任务的枚举
enum Mission
{
    TEST,
    END
};

// 任务名称的定义
#define MISSION_TEST    "TEST"
#define MISSION_END     "END"

class NCStage
{
public:
    NCStage();
    NCStage(NCStage lastStage, Mission initMission, int maxT = 20);
    ~NCStage();
    // 初始化该Stage的数据
    virtual void InitStageData();
    // 该Stage的数据分析阶段
    virtual void StageFunction();
    // 设置默认的当前Stage名称与目标Stage名称
    virtual void SetDefaultStagesName();
    // 对更换目标Stage的条件、名称进行初始化
    virtual void InitChangeNextStage();
    // 对更换目标Stage的条件、名称进行更新
    virtual void UpdateChangeNextStage();

    // 更新当前时间
    void UpdateTime(bool output = true);
    // 该Stage开始工作
    void Run();    
    // 激活该Stage，Stage起始时间重置
    void Active();
    // 判断是否已经超过了时间
    bool isTimeUp(bool output  = true);
    // 获取该Stage是否完成的标志位
    bool getIsDone();
    // 获取当前任务
    Mission getMission();
    // 获取阶段开始时间
    timeval getStartTime();
    // 获取任务开始时间
    timeval getStartTime_Mission();
    // 获取该Stage的名称
    string getName();
    // 获取该Stage的下一个目标Stage名称
    string getNextName();
    // 获取当前Stage的状态
    Stage_State getState();
    // 设置该Stage的下一个目标Stage名称
    void setNextName(string nxtName);
    // 改变该Stage的下一个目标
    void ChangeNextStage(string nxtName);
    // 如果下一个目标Stage与当前Stage的名称相同，则做出警告
    void WarningNextStage();
    // 判断ChangeCondition中是否有满足跳出该Stage的条件
    bool isConditionTrue();
    // 判断是否满足切换Stage条件
    void JudgeFunction();
    // 切换该Stage的下一个目标，跳出该Stage
    void SwitchNextStage();
    // 警告没有添加目标Stage信息的派生类
    void WarningDerivedNextStage();
    // 判断当前Stage与上一Stage的所属Mission是否相同，如果不同则更新StartTime
    bool isMissionSame(NCStage lastStage);
    // 根据输入的Mission值，设定当前任务的名称
    string setCurMissionName();
    // 设置任务最大运行时间
    void setMaxTimeMission();

protected:
    // 阶段开始时间
    timeval StartTime;
    // 阶段运行时间
    double RunTime;
    // 在该阶段的最大时间（单位秒）
    double MaxTime;
    // 该Stage名称
    string Name;
    // 该Stage的下一个目标Stage名称
    string NextName;
    // 阶段的运行状态
    Stage_State State;
    // 该阶段已经完成（一般都为false，该值为true时，也会在相应的函数中被重新置为false）
    bool isDone;
    // 该阶段被激活（一般都为false，该值为true时，也会在相应的函数中被重新置为false）
    bool isActived;
    // 该Stage是否存在目标Stage的分支
    bool isNoSwitch;
    // 当前任务
    Mission CurMission;
    // 当前任务开始时间
    timeval StartTime_Mission;
    // 任务运行时间
    double RunTime_Mission;
    // 任务最大运行时间
    double MaxTime_Mission;
    // 该Stage名称
    string Name_Mission;

    // 对应的向量组合，判断是否达到某种更换目标Stage的条件
    vector<bool> ChangeCondition;
    vector<string> ChangeName;

    // 各种缓存变量
//    timeval tmpStartTime;
    string tmpChangeName;
};



#endif // NCSTAGE_H
