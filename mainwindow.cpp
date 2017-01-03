#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "df.h"
#include "grandiraider.h"

#include "Hotkey/hotkey.h"

#include <QMutex>
#include <QTextCursor>
#include <QDebug>

QTextBrowser *g_logBrowser = nullptr;


void MsgRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    if (g_logBrowser == nullptr)
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    QString msgStr = QString("%1\t%2")
            .arg(QTime::currentTime().toString("HH:mm:ss.zzz"))
            .arg(localMsg.constData());

    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        g_logBrowser->append(msgStr);
        break;
    }

    mutex.unlock();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("L");

    g_logBrowser = ui->textBrowser;
    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(logBrowserScroll()));

    m_started = false;

    Hotkey *startHotkey = new Hotkey(QKeySequence("Ins"),this);
    connect(startHotkey,SIGNAL(activated()),this,SLOT(hotkeyStartTriggerd()));

    Hotkey *stopHotkey = new Hotkey(QKeySequence("Del"),this);
    connect(stopHotkey,SIGNAL(activated()),this,SLOT(hotkeyStopTriggerd()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hotkeyStartTriggerd()
{
    if(!m_started) {
        m_started = true;
        qDebug()<<"Start";
        ::MessageBeep(MB_ICONEXCLAMATION);

        m_df = new GrandiRaider();
        m_df->start();
    }
}

void MainWindow::hotkeyStopTriggerd()
{
    if(m_started) {
        m_started = false;
        qDebug()<<"Stop";
        ::MessageBeep(MB_ICONHAND);

        m_df->terminate();
        m_df->deleteLater();
    }

}

void MainWindow::logBrowserScroll()
{
    QTextCursor cursor =  ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);
}
