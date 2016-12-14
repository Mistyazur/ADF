#include "mainwindow.h"
#include "DmPlugin/dmprivate.h"

#include <QApplication>
#include <QTextCodec>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Language
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));

    // Init dm plugin
    DmPrivate::dmPluginSetup();
    if (!DmPrivate::dmPluginReg("fatecynfa0a8763009beca243a467d74d4f05e0",
                                QApplication::applicationDisplayName()))
        return 0;

    MainWindow w;
//    w.show();

    return a.exec();
}
