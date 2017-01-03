#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class DF;

void MsgRedirection(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &msg);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void hotkeyStartTriggerd();
    void hotkeyStopTriggerd();
    void logBrowserScroll();
private:
    Ui::MainWindow *ui;
    DF *m_df;
    bool m_started;
};

#endif // MAINWINDOW_H
