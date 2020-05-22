#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Setting meta data
    QCoreApplication::setOrganizationName("Rain");
    QCoreApplication::setOrganizationDomain("rainm.me");
    QCoreApplication::setApplicationName("script-navigator");

    MainWindow w;
    w.show();

    return a.exec();
}
