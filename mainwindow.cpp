#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "df.h"
#include "grandiraider.h"

#include "Hotkey/hotkey.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
