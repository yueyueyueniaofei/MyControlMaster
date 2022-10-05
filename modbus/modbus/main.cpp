#include "mastercontrol.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MasterControl w;
    w.show();
    return a.exec();
}
