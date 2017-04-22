#include "ncstage.h"

/*================================================================
 * 构造函数: NCStage()
 =================================================================
 */
NCStage::NCStage()
{
    // 激活该Stage
    Active();
    MaxTime = 0;
    isNoSwitch = true;

    setCurMissionName();
    // 取得此刻时间作为任务开始时间
    gettimeofday(&StartTime_Mission, NULL);
}

/*================================================================
 * 构造函数: NCStage(NCStage lastStage, Mission initMission, int maxT)
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
NCStage::NCStage(NCStage lastStage, Mission initMission, int maxT)
{
    // 激活该Stage
    Active();
    MaxTime = maxT;
    isNoSwitch = false;
    CurMission = initMission;

    isMissionSame(lastStage);
    setCurMissionName();
}

/*================================================================
 * 析构函数: ~NCStage()
 =================================================================
 */
NCStage::~NCStage()
{
    ;
}

/*================================================================
 * 函数：getIsDone()
 * 返回值：bool
 *
 * 功能：返回NCStage类成员变量isDone
 =================================================================
 */
bool NCStage::getIsDone()
{
    return isDone;
}

/*================================================================
 * 函数：getMission()
 * 返回值：Mission
 *
 * 功能：返回NCStage类成员变量CurMission
 =================================================================
 */
Mission NCStage::getMission()
{
    return CurMission;
}

/*================================================================
 * 函数：getStartTime()
 * 返回值：timeval
 *
 * 功能：获取阶段开始时间StartTime
 =================================================================
 */
timeval NCStage::getStartTime()
{
    return StartTime;
}

/*================================================================
 * 函数：getStartTime_Mission()
 * 返回值：timeval
 *
 * 功能：获取任务开始时间StartTime_Mission
 =================================================================
 */
timeval NCStage::getStartTime_Mission()
{
    return StartTime_Mission;
}

/*================================================================
 * 函数：getName()
 * 返回值：string
 *
 * 功能：获取该Stage的名称Name
 =================================================================
 */
string NCStage::getName()
{
    return Name;
}

/*================================================================
 * 函数：getNextName()
 * 返回值：string
 *
 * 功能：获取该Stage的目标Stage的名称Name
 =================================================================
 */
string NCStage::getNextName()
{
    return NextName;
}

/*================================================================
 * 函数：getState()
 * 返回值：Stage_State
 *
 * 功能：获取当前Stage的状态
 =================================================================
 */
Stage_State NCStage::getState()
{
    return State;
}

/*================================================================
 * 函数：UpdateTime()
 *
 * 参数：
 *      bool output:  output为真，则在控制台打印信息；若为假，则不输出
 *
 * 功能：更新当前时间
 =================================================================
 */
void NCStage::UpdateTime(bool output)
{
    // 取得此刻时间
    timeval time;
    gettimeofday(&time, NULL);
    RunTime = (time.tv_sec - StartTime.tv_sec)
            + (time.tv_usec - StartTime.tv_usec) / 1e6;
    RunTime_Mission = (time.tv_sec - StartTime_Mission.tv_sec)
            + (time.tv_usec - StartTime_Mission.tv_usec) / 1e6;
    if(output)
    {
        cout<<"Stage "<<Name<<" RunTime = "<<RunTime<<endl;
        cout<<"Mission "<<Name_Mission<<" RunTime = "<<RunTime_Mission<<endl<<endl;
    }
}

/*================================================================
 * 函数：Run()
 *
 * 功能：Stage的工作函数，是NCStage做实质工作的部分
 *      1. Stage的数据分析阶段，由StageFunction()函数完成；
 *      2. Stage的条件判断阶段，由JudgeFunction()函数完成；
 *      3. 切换Stage的目标Stage，并跳出该Stage，由SwitchNextStage()函数完成；
 =================================================================
 */
void NCStage::Run()
{
//    while(!isTimeUp())
//    {
        /*================================================================
         *     数据分析阶段
         =================================================================
         */
        StageFunction();

        /*================================================================
         *     条件判断阶段
         =================================================================
         */
        // 判断是否满足切换Stage条件
        JudgeFunction();
//        if(isDone)      break;
//    }
    /*================================================================
     *     切换该Stage的下一个目标，跳出该Stage
     =================================================================
     */
    // 如果因为超过Mission而跳出，则切换Mission，Stage重新回到Stage_START，
    // 并为不同的任务设置对应的初始Stage
    if(State == STATE_DONE_TIMEUP_MISSION)
//        SwitchNextMission();
        ;    else
        SwitchNextStage();
}

/*================================================================
 * 函数：Active()
 *
 * 功能：重新激活该Stage，该Stage的相关数据重置
 *      重置的数据包括：
 *      1. 完成标志位isDone；
 *      2. 运行状态Stage；
 *      3. 当前时间StartTime；
 *      4. 目标Stage的更换条件及名称的初始化，由函数InitChangeNextStage()完成；
 *      5. 当前Stage的数据，由函数InitStageData()完成；
 *      6. 设置当前Stage与目标Stage的默认名称，由函数SetDefaultStagesName()完成；
 *
 * 注：
 *      setDefaultStagesName()函数一定要在InitChangeNextStage()函数前面；
 =================================================================
 */
void NCStage::Active()
{
    // 该Stage完成标志位置为false
    isDone = false;
    // 运行状态置为正在运行RUNNING
    State = STATE_RUNNING;
    // 取得此刻时间
    gettimeofday(&StartTime, NULL);

    isActived = false;
    // 设置默认的当前Stage名称与目标Stage名称
    SetDefaultStagesName();
    // 对目标Stage的更换条件、更换名称进行初始化
    InitChangeNextStage();
    // 初始化该Stage的数据
    InitStageData();
    // 设置任务最大运行时间
    setMaxTimeMission();
}

/*================================================================
 * 函数：InitChangeNextStage()
 *
 * 功能：目标Stage的更换条件及名称的初始化
 *      在NCStage中设置了两个配对的向量：
 *      1. bool类型的ChangeCondition，保存每个目标Stage的更换条件是否完成的标志位；
 *      2. string类型的ChangeName，保存与每个ChangeCondition对应的目标Stage的名称；
 *
 * 注：
 *      1. NCStage中bool成员变量isNoSwitch决定了该Stage是否存在分支选项，true则为
 * 单通道进行，只有一个默认的目标Stage；false则可以进行选择，其余的目标Stage条
 * 件标志位与名称分别存入两个向量中；
 *      2. 两个向量的长度一定要维持一致。
 =================================================================
 */
void NCStage::InitChangeNextStage()
{
    ChangeCondition.clear();
    ChangeName.clear();

    // 如果isNoSwitch为真，则默认该Stage只能顺序执行，不能选择，
    // 此时该Stage的目标Stage只能为初始化时的目标Stage；
    ChangeCondition.push_back(false);
    ChangeName.push_back(NextName);

    // 如果isNoSwitch为假，则说明该Stage的有其他可供选择的目标Stage，
    // 这样的话则可添加其他的目标Stage以及对应的满足条件；
    if(!isNoSwitch)
    {
//    // 目前手动写入ChangeCondition的更新
//    // 基类的UpdateChangeNextStage并没有任何操作，
//    // 但仍旧将如何操作的代码以注释的形式写了出来

//        ChangeCondition.push_back(false);
//        ChangeName.push_back(STAGE_TEST_3);


    }

    // 判断调用该函数的当前对象是否是NCStage类型，如果是，说明该对象
    // 没有进行派生便进入了基类的虚函数中，此时作提示
    if(typeid(this) == typeid(NCStage))
    {
        // 如果派生类没有多态编写，则从基类做出提醒
        cout<<"Warning: Stage "<<Name<<" do not Add In Any DERIVED FUNCTION."<<endl;
    }
}

/*================================================================
 * 函数：InitStageData()
 *
 * 功能：初始化该Stage的数据
 *      需要初始化的数据，如计数器、标志位等；
 =================================================================
 */
void NCStage::InitStageData()
{
    // 判断调用该函数的当前对象是否是NCStage类型，如果是，说明该对象
    // 没有进行派生便进入了基类的虚函数中，此时作提示
    if(typeid(this) == typeid(NCStage))
    {
        // 如果派生类没有多态编写，则从基类做出提醒
        cout<<"Warning: Stage "<<Name<<" did not INITIAL Stage Data."<<endl;
    }
}

/*================================================================
 * 函数：StageFunction()
 *
 * 功能：初始化该Stage的数据
 *      需要初始化的数据，如计数器、标志位等；
 =================================================================
 */
void NCStage::StageFunction()
{
    // 如果派生类没有多态编写，则从基类做出提醒
    cout<<"Warning: Stage "<<Name<<" did not Realize STAGE FUNCTION."<<endl;
}

/*================================================================
 * 函数：isTimeUp()
 * 返回值：bool
 *
 * 功能：判断是否已经超过了Stage设定的最大时间
 *      三种状态：
 *      1. MaxTime < 0: 认为没有时间限制；
 *      2. MaxTime = 0: 立即退出；
 *      3. MaxTime > 0: 正常设置的最大时间，单位为秒
 *      关于最大运行时间MaxTime与当前运行时间RunTime：
 *          A. RunTime > MaxTime，则标志该Stage已经超时；
 *          B. RunTime <= MaxTime, 则说明该Stage的运行时间仍在时间限制之内；
 =================================================================
 */
bool NCStage::isTimeUp(bool output)
{
    // 更新当前时间
    UpdateTime();

    // 如果设定MaxTime < 0，则默认没有时间限制
    if(MaxTime < 0)
        return false;
    else
    {
        if(RunTime_Mission >= MaxTime_Mission)
        {
            if(output)
                cout<<"Mission "<<setCurMissionName()<<"'s Time Up."<<endl;
            // 该Stage完成标志位置为true
            isDone = true;
            State = STATE_DONE_TIMEUP_MISSION;
            return true;
        }
        else if(RunTime >= MaxTime)
        {
            if(output)
            {
                cout<<"Stage "<<Name<<"'s Time Up."<<endl;
                cout<<"    Ready Go to Next Stage: "<<NextName<<endl;
            }
            // 该Stage完成标志位置为true
            isDone = true;
            State = STATE_DONE_TIMEUP_STAGE;
            return true;
        }
        else
            return false;
    }
}

/*================================================================
 * 函数：setNextName(string nxtName)
 *
 * 参数：
 *      string nxtName: 将该参数传递给NCStage对象的NextName变量，作为目标Stage名称；
 *
 * 功能：设置当前Stage的目标Stage名称
 *      只有两种情况可以直接运用该函数：
 *      1. 当前Stage是特殊的开始Stage；
 *      2. 当前状态已经被设定为特许转变目标Stage的状态(STATE_CHANGE_NEXT)
 *      其余状态均报错。
 =================================================================
 */
void NCStage::setNextName(string nxtName)
{
    // 只有StartStage，以及在当前状态为STATE_CHANGE_NEXT时，才可以改变下个任务目标
    // 某些状态具体指什么，待定，暂用true代替
    if(Name == STAGE_START || State == STATE_CHANGE_NEXT)
        NextName = nxtName;
    else
    {
        if(Name != STAGE_START)
            cout<<"  ERROR: Stage "<<Name<<" is NOT START_STAGE."<<endl;
        else
            cout<<"  ERROR: Stage "<<Name<<" is NOT at the RIGHT STATE for Change NextStage."<<endl;
    }
}

/*================================================================
 * 函数：ChangeNextStage(string nxtName)
 *
 * 参数：
 *      string nxtName: 将该参数传递给NCStage对象的NextName变量，作为目标Stage名称；
 *
 * 功能：改变该Stage的下一个目标；
 =================================================================
 */
void NCStage::ChangeNextStage(string nxtName)
{
    // 暂存当前State
    Stage_State tmpState = State;
    // 暂存当前的NextStage
    string tmpNextName = NextName;
    State = STATE_CHANGE_NEXT;
    setNextName(nxtName);
    State = tmpState;
    cout<<"  Stage "<<Name<<" Changed NEXT STAGE from "<<tmpNextName<<" to "<<NextName<<endl;
}

/*================================================================
 * 函数：WarningNextStage()
 *
 * 功能：如果下一个目标Stage与当前Stage的名称相同，则做出警告；
 =================================================================
 */
void NCStage::WarningNextStage()
{
    if(NextName == Name)
        cout<<"  Warning of Stage "<<Name<<" : Current Stage is SAME AS Next Stage!"<<endl;
}

/*================================================================
 * 函数：UpdateChangeNextStage()
 *
 * 功能：对更换目标Stage的标志位进行更新
 *      根据是否有分支目标标志位isNoSwitch的真假，基本分为两种情况：
 *      1. isNoSwitch == true: 则说明只有一个默认目标Stage作为该Stage的出口，这时
 * 两个向量ChangeCondition, ChangeName的长度应该只有一位（只有默认目标的信
 * 息），所以只需要对默认目标Stage的信息进行设置即可；
 *      2. isNoSwitch == false: 则说明该Stage有其他可供选择的目标Stage, 这样就需要
 * 添加其他目标Stage名称与其对应的满足条件；
 *      3. 如果isNoSwitch == false且两个向量的长度仍为1，则说明在派生类成员函数的
 * 编写过程中没有添加其他目标Stgae名称及条件信息，做出警告；
 =================================================================
 */
void NCStage::UpdateChangeNextStage()
{
//    // 目前手动写入ChangeCondition的更新
//    // 基类的UpdateChangeNextStage并没有任何操作，
//    // 但仍旧将如何操作的代码以注释的形式写了出来

//    if(0 < ChangeCondition.size())
//        ChangeCondition[0] = (num_int > 10);

    // 如果isNoSwitch为假，则说明该Stage的有其他可供选择的目标Stage，
    // 这样的话则可添加其他的目标Stage以及对应的满足条件；
    if(!isNoSwitch)
    {
        //    if(1 < ChangeCondition.size())
        //        ChangeCondition[1] = (num_rand > 40);

        // 警告没有添加目标Stage信息的派生类
        WarningDerivedNextStage();
    }

    // 判断调用该函数的当前对象是否是NCStage类型，如果是，说明该对象
    // 没有进行派生便进入了基类的虚函数中，此时作提示
    if(typeid(this) == typeid(NCStage))
    {
        // 如果派生类没有多态编写，则从基类做出提醒
        cout<<"Warning: Stage "<<Name<<" do not Add In Any UPDATE Next Stage Condition."<<endl;
    }
}

/*================================================================
 * 函数：SetDefaultStagesName()
 *
 * 功能：设置默认的当前Stage名称与目标Stage名称
 *      根据后面不同派生类的情况具体设置，此处基类NCStage全部设置为NULL，容
 * 易触发错误警告引起编程者的注意；
 =================================================================
 */
void NCStage::SetDefaultStagesName()
{
    Name = STAGE_NULL;
    // 默认目标Stage
    NextName = STAGE_NULL;
}

/*================================================================
 * 函数：isConditionTrue()
 * 返回值：bool
 *
 * 功能：判断ChangeCondition中是否有满足跳出该Stage的条件
 *      遍历ChangeCondition向量，如果遇到向量中某bool成员变量为true，则说明
 * 达到了条件，返回true，并记下当前bool变量对应的ChangeName变量作为将要
 * 更改的目标Stage名称；
 *
 * 注：
 *      1. 笔者这里的思路希望在填充两个向量时，是按照优先级大小填充的，且以
 * 默认目标Stage为最大优先级；
 *      2. 笔者此处的代码只考虑了ChangeCondition向量中只存在一个bool变量为
 * true的情况，并没有考虑同时满足多个条件的情况；以后需要注意；
 *      3. 这里没有考虑分支目标标志位isNoSwitch变量，是因为这里笔者把默认目
 * 标Stage作为最大优先级考虑，默认目标在前面在两个向量放置，首元素即作为
 * 最大优先级；
 =================================================================
 */
bool NCStage::isConditionTrue()
{
    for(int i = 0; i <= ChangeCondition.size(); i++)
    {
        if(ChangeCondition[i])
        {
            tmpChangeName = ChangeName[i];
            return true;
            break;
        }
    }
    return false;
}

/*================================================================
 * 函数：JudgeFunction()
 *
 * 功能：判断是否满足切换Stage条件
 =================================================================
 */
void NCStage::JudgeFunction()
{
    // 对更换目标Stage的标志位进行更新
    UpdateChangeNextStage();
    // 如果存在满足切换Stage的判断条件，则将Stage的相关标志位更新
    if(isConditionTrue())
    {
        isDone = true;
        State = STATE_DONE_NORMAL;
    }
}

/*================================================================
 * 函数：SwitchNextStage()
 *
 * 功能：切换该Stage的下一个目标，跳出该Stage
 *      按照该Stage不同的状态位State，分为两种情况：
 *      1. STATE_DONE_TIMEUP: 因为超时而结束；
 *      2. STATE_DONE_NORMAL: 正常运行且满足切换Stage的条件而结束；
 *      其余状态，视为异常；
 =================================================================
 */
void NCStage::SwitchNextStage()
{
    if(isDone)
    {
        switch(State)
        {
        case STATE_DONE_TIMEUP_STAGE:
        case STATE_DONE_TIMEUP_MISSION:
            break;
        case STATE_DONE_NORMAL:
        {
            ChangeNextStage(tmpChangeName);
            break;
        }
        default:
            cout<<"ERROR: Stage "<<Name<<" Switch State WRONG: NO DONE."<<endl;
            break;
        }
    }
}

/*================================================================
 * 函数：WarningDerivedNextStage()
 *
 * 功能：警告没有添加目标Stage信息的派生类
 *      如果isNoSwitch == false且两个向量的长度仍为1，则说明在派生类成员函数的
 * 编写过程中没有添加其他目标Stgae名称及条件信息，做出警告；
 =================================================================
 */
void NCStage::WarningDerivedNextStage()
{
    // 如果ChangeCondition的大小过小，说明没有在派生类中添加额外条件，此时警告
    if(ChangeCondition.size() < 1)
        cout<<"  Warning: Stage "<<Name<<" do not Add In Any NEXT STAGE CONDITION."<<endl;
}

/*================================================================
 * 函数：isMissionSame(NCStage lastStage)
 *
 * 功能：判断当前Stage与上一Stage的所属Mission是否相同
 *      如果相同，则直接获取上一Stage的Mission起始时间；
 *      如果不同，则更新StartTime_Mission为当前时间
 =================================================================
 */
bool NCStage::isMissionSame(NCStage lastStage)
{
    if(lastStage.getMission() == CurMission)
    {
        StartTime_Mission = lastStage.getStartTime_Mission();
    }
    else
    {
        // 取得此刻时间
        gettimeofday(&StartTime_Mission, NULL);
    }
}

/*================================================================
 * 函数：setCurMissionName()
 * 返回值：string
 *
 * 功能：设置当前任务的默认目标任务
 *      根据输入的Mission值，设定当前任务的名称
 *
 * 注：
 *      只是作测试用，以后根据任务目标具体更改
 =================================================================
 */
string NCStage::setCurMissionName()
{
    string dst;
    switch(CurMission)
    {
    case TEST:
        Name_Mission = MISSION_TEST;
        break;
    case END:
        Name_Mission = MISSION_END;
        break;
    default:
        Name_Mission = MISSION_END;
        break;
    }
    dst = Name_Mission;
    return dst;
}

/*================================================================
 * 函数：setMaxTimeMission()
 *
 * 功能：设置任务最大运行时间
 *      根据输入的Mission值，设定当前任务的最大运行时间
 *
 * 注：
 *      只是作测试用，以后根据任务目标具体更改
 =================================================================
 */
void NCStage::setMaxTimeMission()
{
    switch(CurMission)
    {
    case TEST:
        MaxTime_Mission = 20;
        break;
    case END:
        MaxTime_Mission = 30;
        break;
    default:
        MaxTime_Mission = 1;
        break;
    }
}
