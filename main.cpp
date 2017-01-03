#include "mainwindow.h"
#include "DmPlugin/dmprivate.h"

#include <QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Language
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));


    // Init dm plugin
    DmPrivate::dmPluginSetup(true);
    if (!DmPrivate::dmPluginReg("FateCyn220d1daf6c3e4a132d4da742b5fc3691",
                                QApplication::applicationDisplayName(), "b2"))
        return 0;

    MainWindow w;
    w.show();

//    qInstallMessageHandler(MsgRedirection);

    return a.exec();
}
