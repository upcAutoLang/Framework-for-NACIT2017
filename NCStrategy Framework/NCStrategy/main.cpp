#include "ncstrategymainwin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NCStrategyMainWin w;
    w.show();

    return a.exec();
}
