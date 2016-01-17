#include "ssd_optimizer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ssd_optimizer w;
    w.show();

    return a.exec();
}
