#include "mainwindow.h"
#include "DmPlugin/dmprivate.h"

#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include <QMutex>
#include <QFile>

#include <QDebug>
#include <windows.h>

#define LOG_NAME QApplication::applicationDirPath() + "/record.log"

void MsgRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;

    mutex.lock();

    QFile log(LOG_NAME);
    if (log.open(QFile::ReadWrite|QFile::Append)) {
        QByteArray localMsg = msg.toLocal8Bit();
        QString msgStr = QString("%1\t%2")
                .arg(QDateTime::currentDateTime().toString("MM-dd HH:mm:ss.zzz"))
                .arg(localMsg.constData());

        QTextStream out(&log);
        out << msgStr << endl;

        log.close();
    }


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
    QApplication a(argc, argv);

    QFile log(LOG_NAME);
    if (log.exists()) {
        qInstallMessageHandler(MsgRedirection);
    }

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
