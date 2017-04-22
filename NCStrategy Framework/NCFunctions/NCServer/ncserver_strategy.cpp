#include "ncserver.h"

/*================================================================
 * 函数：Strategy()
 *
 * 说明：
 *      服务器功能运行；
 *      基本同于NCStage_v1.07中的流程：
 *      循环遍历所有Stage，直到遇到Stage_END跳出循环；
 *          (1) 激活当前Stage；
 *          (2) 根据当前Stage的目标Stage名称，判断当前Stage是否有错误；
 *          (3) 运行当前Stage；
 *              A.设置超时时间；
 *              B. 初始化FD集合；
 *              C. 连接服务器端；
 *              D. 连接多个客户端；
 *              E. 判断服务器FD集合连接是否正常，如果返回false，则continue该循环；
 *              F. 向多个客户端发送数据；
 *              G. 检测服务器套接字FD是否在FD集合中；
 *              H. 遍历所有客户端，针对性的处理对应客户端传来的消息；
 *              I. NCStage.dat文件记录；
 *              J. 当前Stage运行；
 *          (4) 如果NCStage执行阶段达到了Mission设定的最大时间，则切换任务；
 *          (5) 如果下一个目标Stage与当前Stage的名称相同，则做出警告；
 *          (6) 根据运行后的目标Stage名称，更新CurStage的变量类型；
 *          (7) 直到*CurStage的类型为NCStage_END，跳出循环，所有任务结束；
 =================================================================
 */
void NCServer::Strategy()
{
    //================================================
    // 2. 循环遍历所有Stage，直到遇到Stage_END跳出循环
    //================================================
    while(CurStage->getName() != STAGE_END)
    {
        //================================================
        // (1) 激活当前Stage
        //================================================
        CurStage->Active();

        //================================================
        // (2) 根据当前Stage的目标Stage名称，判断当前Stage是否有错误；
        //================================================
        if(CurStage->getNextName() == STAGE_NULL)
        {
            if(CurStage->getName() != STAGE_END
                    && CurStage->getName() != STAGE_START)
            {
                cout<<"Stage "<<CurStage->getName()<<" ERROR: Next Stage Name is NULL."<<endl;
                State = SERVER_ERROR_STAGENULL;
                break;
            }
        }

        //================================================
        // (3) 运行当前Stage；
        //================================================
        while(!CurStage->getIsDone()
              && !CurStage->isTimeUp())
        {
            // 设置超时时间
            SetTimeValue(SERVER_UPTIME);

            // 初始化文件描述符集合
            InitFDSet();

            // 连接服务器端
            ConnectServer();

            // 连接多个客户端
            // 在此处添加针对性连接客户端的代码
            ConnectClients();

            // 判断服务器FD集合连接是否正常
            if(!isSelectOK())   continue;

            // 向多个客户端发送数据
            SendData();

            // 检测服务器套接字文件描述符(server_sock_fd)是否在文件描述符集合中，且是否有变化
            DetectFDSet();

            // 遍历所有客户端，处理对应客户端过来的消息
            // 在此处添加针对性连接客户端的代码
            ClientDataProcess();

            // NCStage.dat记录函数
            RecordNCStage();

            //
            SendStages2Surf();

            // 当前Stage运行
            CurStage->Run();
        }

        //================================================
        // (4) 如果NCStage执行阶段达到了Mission设定的最大时间，则切换任务；
        //      每个Mission都应该设定一个为TimeUp状态准备的备用Mission目标与Stage目标，此处略去；
        //================================================
        if(CurStage->getState() == STATE_DONE_TIMEUP_MISSION)
        {
            if(tmpMission == TEST)      tmpMission = END;
            else if(tmpMission == END)       tmpMission = TEST;

            Mission_i = 0;

            SetStartStage(CurStage, tmpMission);
        }

        //================================================
        // (5) 如果下一个目标Stage与当前Stage的名称相同，则做出警告；
        //================================================
        CurStage->WarningNextStage();

        //================================================
        // (6) 根据运行后的目标Stage名称，更新CurStage的变量类型；
        //================================================
        ChangeStage(CurStage, tmpMission);

        //================================================
        // (7) 直到*CurStage的类型为NCStage_END，跳出循环，所有任务结束
        //================================================
        if(typeid(*CurStage) == typeid(NCStage_END))
        {
            State = SERVER_END;
            break;
        }

        // 随便更改一下当前任务，检查Mission的计时起始时间是否会根据任务不同而改变，做测试用
        Mission_i++;
        if(Mission_i == 3)
        {
            if(tmpMission == TEST)      tmpMission = END;
            else if(tmpMission == END)       tmpMission = TEST;

            Mission_i = 0;

            SetStartStage(CurStage, tmpMission);
        }
    }
}
