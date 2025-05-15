#include "patientwindow.h"
// #include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PatientWindow w;
    w.show();
    return a.exec();
}
