#include "ncstrategymainwin.h"
#include "ui_ncstrategymainwin.h"

/*================================================================
 * 构造函数：NCStrategyMainWin(QWidget *parent)
 =================================================================
 */
NCStrategyMainWin::NCStrategyMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NCStrategyMainWin)
{
    ui->setupUi(this);
}

/*================================================================
 * 析构函数：~NCStrategyMainWin()
 =================================================================
 */
NCStrategyMainWin::~NCStrategyMainWin()
{
    delete threadClient;
    delete ui;
}

/*================================================================
 * 槽函数：SurfaceShow(QString str)
 * 参数：
 *      QString str:  响应信号SendMessage(string str)，接受从信号函数中传来的界面客户端信息;
 *
 * 说明：
 *      接受界面客户端传来的信息，并进行解算，更新显示在界面上；
 =================================================================
 */
void NCStrategyMainWin::SurfaceShow(QString str)
{
    ServerInfo = str;
    ui->MessagesBrowser->append(ServerInfo);

    // 解算Stage, Mission
    ShowStage();
    ShowMission();
    // 解算Image, Send客户端传来的信息
    ShowImageData();
    ShowSendData();
}

/*================================================================
 * 槽函数：on_ServerRunButton_clicked()
 *
 * 说明：
 *      点下ServerRunButton后，界面客户端线程运行；
 *
 * 注：
 *      目前实现功能，只按了一下按钮就完事儿了，所以Surface Stop状态没有研究，
 * 比较鸡肋……各位师弟以后如果需要完善的话就完善吧
 =================================================================
 */
void NCStrategyMainWin::on_ServerRunButton_clicked()
{
    if(!isSurfRun)
    {
        isSurfRun = true;
        ui->ServerRunButton->setText("Surface Stop");
        threadClient = new Thread_Client();
        connect(threadClient, SIGNAL(SendMessage(QString)),
                this, SLOT(SurfaceShow(QString)));
        threadClient->start();
    }
    else
    {
        isSurfRun = false;
        ui->ServerRunButton->setText("Surface Run");
        threadClient->stop();
        threadClient->exit();
    }
}

/*================================================================
 * 函数：RemoveSubStr(QString str, QString substr)
 * 返回值：QString
 * 参数：
 *      QString str: 主字符串
 *      QString substr:  被消除的字符串
 *
 * 说明：
 *      删除在主字符串str中的子串substr，并返回删除后的字符串；
 *      若主字符串中不存在子串，则返回原字符串；
 =================================================================
 */
QString NCStrategyMainWin::RemoveSubStr(QString str, QString substr)
{
    QString tmpStr;
    if(str.contains(substr))
    {
        tmpStr = str;
        tmpStr.replace(tmpStr.indexOf(substr),
                       substr.length(),
                       "");
    }
    else
        tmpStr = str;

    return tmpStr;
}

/*================================================================
 * 函数：RemoveSubStr(QString str, QString substr)
 * 返回值：QString
 * 参数：
 *      QString str: 主字符串
 *      QString substr:  被消除的字符串
 *
 * 说明：
 *      删除在主字符串str中的子串substr，并返回删除后的字符串；
 *      若主字符串中不存在子串，则返回原字符串；
 =================================================================
 */
QString NCStrategyMainWin::RemoveSubStr(QString str, char substr[])
{
    QString tmpStr;
    QString tmpSubStr = QString::fromStdString(substr);

    tmpStr = RemoveSubStr(str, tmpSubStr);
    return tmpStr;
}

/*================================================================
 * 函数：ShowStage()
 *
 * 说明：
 *      界面显示Stage信息；
 =================================================================
 */
void NCStrategyMainWin::ShowStage()
{
    QString tmpStr;
    if(ServerInfo.startsWith(NAME_CLIENT_SURF))
    {
        tmpStr = ServerInfo;
        ui->StageMessageLineEdit->setText(tmpStr);
        tmpStr = RemoveSubStr(tmpStr, NAME_CLIENT_SURF);
        tmpStr = RemoveSubStr(tmpStr, "@");

        if(tmpStr.contains("stage:"))
        {
            tmpStr = RemoveSubStr(tmpStr, "stage:");
            ui->CurStageLineEdit->setText(tmpStr);
        }
    }
}

/*================================================================
 * 函数：ShowMission()
 *
 * 说明：
 *      界面显示Mission信息；
 =================================================================
 */
void NCStrategyMainWin::ShowMission()
{
    QString tmpStr;
    if(ServerInfo.startsWith(NAME_CLIENT_SURF))
    {
        tmpStr = ServerInfo;
        tmpStr = RemoveSubStr(tmpStr, NAME_CLIENT_SURF);
        tmpStr = RemoveSubStr(tmpStr, "@");

        if(tmpStr.contains("mission:"))
        {
            tmpStr = RemoveSubStr(tmpStr, "mission:");
            ui->CurMissionLineEdit->setText(tmpStr);
        }
    }
}

/*================================================================
 * 函数：ShowSendData()
 *
 * 说明：
 *      界面显示Send Client信息；
 =================================================================
 */
void NCStrategyMainWin::ShowSendData()
{
    QString tmpStr;
    if(ServerInfo.startsWith(NAME_CLIENT_SURF))
    {
        tmpStr = ServerInfo;
        tmpStr = RemoveSubStr(tmpStr, NAME_CLIENT_SURF);
        tmpStr = RemoveSubStr(tmpStr, "@");

        if(tmpStr.contains("SEND Client TEST:"))
        {
            tmpStr = RemoveSubStr(tmpStr, "SEND Client TEST:");
            ui->SendTESTLineEdit->setText(tmpStr);
        }
    }
}

/*================================================================
 * 函数：ShowImageData()
 *
 * 说明：
 *      界面显示Image Client信息；
 =================================================================
 */
void NCStrategyMainWin::ShowImageData()
{
    QString tmpStr;
    if(ServerInfo.startsWith(NAME_CLIENT_SURF))
    {
        tmpStr = ServerInfo;
        tmpStr = RemoveSubStr(tmpStr, NAME_CLIENT_SURF);
        tmpStr = RemoveSubStr(tmpStr, "@");

        if(tmpStr.contains("IMAGE Client TEST:"))
        {
            tmpStr = RemoveSubStr(tmpStr, "IMAGE Client TEST:");
            ui->ImageTESTLineEdit->setText(tmpStr);
        }
    }
}

/*================================================================
 * 槽函数：on_ServerCMDSendButton_clicked()
 *
 * 说明：
 *      点下ServerCMDSendButton后，将ServerCMDSendLineEdit中的数据发送给服
 * 务器，并在服务器中发送给各个客户端；
 =================================================================
 */
void NCStrategyMainWin::on_ServerCMDSendButton_clicked()
{
    QString cmd = ui->ServerCMDLineEdit->text();
    cmd += '\n';
    threadClient->SurfSendCMD(cmd);
}
