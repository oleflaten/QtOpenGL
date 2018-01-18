//#include "GL/glew.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);    //Forces the usage of desktop OpenGL
                                                                //Attribute must be set before Q(Gui)Application is constructed.
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
