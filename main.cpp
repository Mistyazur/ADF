#include "mainwindow.h"
#include "DmPlugin/dmprivate.h"

#include <QApplication>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>
#include <QMutex>
#include <QTimer>
#include <QFile>
#include <QDebug>

#include <windows.h>
#include <DbgHelp.h>

#define LOG_NAME QApplication::applicationDirPath() + "/debug.log"

void MsgRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;

    mutex.lock();

    QFile log(LOG_NAME);
    if (log.open(QFile::ReadWrite|QFile::Append)) {
        QByteArray localMsg = msg.toLocal8Bit();
        QString msgStr = QString("%1 %2")
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

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    // Save data

    // Create dump file
    QString dmpFile = QString("%1.dmp").arg(QTime::currentTime().toString("HH_mm_ss_zzz"));
    HANDLE hDumpFile = CreateFile(dmpFile.toStdWString().c_str(),
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if( hDumpFile != INVALID_HANDLE_VALUE) {
        // Dump info
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        // Write dump
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }

    // Dump message
    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    QString errAdr(QString::number((uint)record->ExceptionAddress, 16));
    QMessageBox::critical(NULL, "Crash", QString("Code:%1. Addr:%2.").arg(errCode).arg(errAdr), QMessageBox::Ok);

    // Quit app
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    bool startUp = false;

    QApplication a(argc, argv);

    // Arguments
    QStringList args = QApplication::arguments();
    if (args.count() > 1) {
        if (args.at(1) == "-a") {
            startUp = true;
        }
    }

    // Log
    QFile log(LOG_NAME);
    if (log.exists()) {
        qInstallMessageHandler(MsgRedirection);
    }

    // Language
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));

    // Exception captcher
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

    // Init dm plugin
    DmPrivate::dmPluginSetup(true);
    if (!DmPrivate::dmPluginReg("FateCyn220d1daf6c3e4a132d4da742b5fc3691",
                                QApplication::applicationDisplayName(), "block"))
        return 0;

    // GUI
    MainWindow w;
    w.show();

    // Start with windows
    if (startUp) {
        w.setWindowTitle("Z");
        QTimer::singleShot(120*1000, &w, SLOT(on_btnStart_clicked()));
    }

    return a.exec();
}
