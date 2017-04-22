#include "auvautocontrol.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AuvAutoControl w;
    w.show();

    return a.exec();
}
