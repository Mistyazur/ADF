#include "mainwindow.h"
#include "DmPlugin/dmprivate.h"

#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include <QMutex>
#include <QFile>

void MsgRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    static QFile file(QApplication::applicationDirPath() + "/" + QDateTime::currentDateTime().toString("yy_MM_dd__HH_mm_ss_zzz") + ".log");
    static QTextStream out(&file);

    mutex.lock();

    if (!file.isOpen()) {
        if (!file.open(QFile::ReadWrite|QFile::Append)) {
            return;
        }
    }

    QByteArray localMsg = msg.toLocal8Bit();
    QString msgStr = QString("%1\t%2")
            .arg(QTime::currentTime().toString("HH:mm:ss.zzz"))
            .arg(localMsg.constData());

    out << msgStr << endl;
    out.flush();

//    switch (type) {
//    case QtDebugMsg:
//    case QtInfoMsg:
//    case QtWarningMsg:
//    case QtCriticalMsg:
//    case QtFatalMsg:
//        break;
//    }

    mutex.unlock();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(MsgRedirection);

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

    return a.exec();
}
