#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class DF;
class QSettings;

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
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_actionStart_with_windows_triggered(bool checked = false);
    void on_actionStart_TGP_triggered(bool checked = false);
private:
    Ui::MainWindow *ui;
    DF *m_df;
    QSettings *m_settings;

    bool m_started;
    QString m_tgpPath;
};

#endif // MAINWINDOW_H
