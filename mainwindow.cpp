#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "df.h"
#include "grandiraider.h"

#include "Hotkey/hotkey.h"

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
    setFixedSize(200, 200);

    RECT winRect;
    ::GetWindowRect((HWND)winId(), &winRect);
    move(QApplication::desktop()->availableGeometry().width() - (winRect.right - winRect.left),
         QApplication::desktop()->availableGeometry().height() - (winRect.bottom - winRect.top));

    m_started = false;
    ui->btnStop->setEnabled(false);

    Hotkey *startHotkey = new Hotkey(QKeySequence("Page Up"),this);
    connect(startHotkey, SIGNAL(activated()), this, SLOT(on_btnStart_clicked()));

    Hotkey *stopHotkey = new Hotkey(QKeySequence("Page Down"),this);
    connect(stopHotkey, SIGNAL(activated()), this, SLOT(on_btnStop_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStart_clicked()
{
    if(!m_started) {
        m_started = true;
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(true);

//        m_df = new GrandiRaider();
//        m_df->start();
    }
}

void MainWindow::on_btnStop_clicked()
{
    if(m_started) {
        m_started = false;
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);

//        m_df->terminate();
//        m_df->deleteLater();
    }
}
