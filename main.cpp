#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Resources/Icons/db.png"));
    a.setStyleSheet("QWidget{font-size: 12px;}");
    //a.setStyle("fusion");
    MainWindow w;

    w.showMaximized();
    return a.exec();
}
