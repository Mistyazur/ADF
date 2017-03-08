#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "df.h"
#include "grandiraider.h"

#include "Hotkey/hotkey.h"

#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

#include <Windows.h>


#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

bool IsAutoStartEnbaled()
{
    QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    QString application_path = QApplication::applicationFilePath();

    if (settings->value(QApplication::applicationName()).isNull())
        return false;

    return true;
}

void SetAutoStartEnabled(bool enabled)
{
    QString appName = QApplication::applicationName();
    QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    if(enabled)
    {
        QString cmd = QString("\"%1\" -a")
                .arg(QApplication::applicationFilePath().replace("/", "\\"));
        settings->setValue(appName, cmd);
    }
    else
    {
        settings->remove(appName);
    }
    delete settings;
}


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

    ui->btnStop->setEnabled(false);

    // Hotkeys
    Hotkey *startHotkey = new Hotkey(QKeySequence("Page Up"),this);
    Hotkey *stopHotkey = new Hotkey(QKeySequence("Page Down"),this);
    connect(startHotkey, SIGNAL(activated()), this, SLOT(on_btnStart_clicked()));
    connect(stopHotkey, SIGNAL(activated()), this, SLOT(on_btnStop_clicked()));

    // Auto-start
    if (IsAutoStartEnbaled()) {
        ui->actionStart_with_windows->setChecked(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStart_clicked()
{
    if(ui->btnStart->isEnabled()) {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(true);

        m_df = new GrandiRaider();
        m_df->start();
    }
}

void MainWindow::on_btnStop_clicked()
{
    if(ui->btnStop->isEnabled()) {
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);

        m_df->terminate();
        m_df->deleteLater();
    }
}

void MainWindow::on_actionStart_with_windows_triggered(bool checked)
{
    SetAutoStartEnabled(checked);
}

void MainWindow::on_actionSet_tgp_path_triggered(bool checked)
{
    QSettings settings("Mistyazur", QApplication::applicationName());
    QString path = QFileDialog::getOpenFileName(this, tr("Select TGP"), "C:/", tr("TGP (tgp_daemon.exe)"));
    if (!path.isEmpty()) {
        settings.setValue("TGP/path", path);
    }
}
