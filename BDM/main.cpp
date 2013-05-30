#include <QtGui/QApplication>
#include "bdm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BDM w;
    w.show();
    return a.exec();
}
