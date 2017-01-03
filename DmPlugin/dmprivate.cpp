#include "dmprivate.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>

#include <windows.h>
#include <time.h>

typedef int (WINAPI *SETDLLPATHW)(LPCWSTR path, int type);

DmPrivate::DmPrivate(QObject *parent) :
    QThread(parent)
{
    // Init
    m_mouseDelayDelta = 0;
    m_keyDelayDelta = 0;

    // Init random seed
    qsrand(time(0));

    // Ingore errors showed by dm plugin
    m_dm.SetShowErrorMsg(0);
}

DmPrivate::~DmPrivate()
{
    wait();
}

void DmPrivate::dmPluginSetup(bool local)
{
    if (local) {
        QString regDLLPath = QApplication::applicationDirPath()+"/DmReg.dll";
        QString pluginDLLPath = QApplication::applicationDirPath()+"/dm(mta).dll";

        SETDLLPATHW SetDllPathW = (SETDLLPATHW)GetProcAddress(LoadLibrary(regDLLPath.toStdWString().c_str()),
                                                              "SetDllPathW");
        SetDllPathW(pluginDLLPath.toStdWString().c_str(), 1);
    } else {
        QProcess proc;

        proc.start("regsvr32 /s dm(mta).dll");
        proc.waitForStarted();
        proc.waitForFinished();
        proc.close();

        msleep(1000);
    }
}
bool DmPrivate::dmPluginReg(const QString &key, const QString &flag, const QString &guard)
{
    dmsoft dm;

    qDebug()<<dm.Ver();

    if (dm.Ver() != DM_VER)
        return false;

    if (dm.Ver().toDouble() > 3.1233) {
        int code = dm.Reg(key, flag);
        if (code != 1) {
            qWarning() << "Register failed:" <<code;
            return false;
        }

        // Guard
        if (!guard.isEmpty())
            qDebug()<<"Guard"<<dm.DmGuard(1, guard);
    }

    if (!dm.BindWindow(dm.GetForegroundWindow(),"normal","normal","normal",0))
        return false;

    dm.UnBindWindow();

    dm.DisablePowerSave();
    dm.DisableScreenSave();

    return true;
}

void DmPrivate::setResourcePath(const QString &path)
{
    m_dm.SetPath(qApp->applicationDirPath()+"/"+path);
}

void DmPrivate::approxSleep(int msec, double delta)
{
    int max = msec*(1+delta);
    int min = msec*(1-delta);

    msleep(MEDIAN_RANDOM(min, max));
}

void DmPrivate::setMouseDelayDelta(double delta)
{
    m_mouseDelayDelta = delta;
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const int &x,
                      const int &y,
                      const int delay)
{
    if ((x >= 0) && (y >= 0)) {
        m_dm.MoveTo(x,y);
        approxSleep(delay, m_mouseDelayDelta);
    }

    switch (oper) {
    case Move:
        break;
    case Left:
        m_dm.LeftClick();
        break;
    case LeftDown:
        m_dm.LeftDown();
        break;
    case LeftUp:
        m_dm.LeftUp();
        break;
    case Right:
        m_dm.RightClick();
        break;
    case RightDown:
        m_dm.RightDown();
        break;
    case RightUp:
        m_dm.RightUp();
        break;
    default:
        break;
    }

    if (delay > 0)
        approxSleep(delay, m_mouseDelayDelta);
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const QVariant &x,
                      const QVariant &y,
                      int delay)
{
    sendMouse(oper, x.toInt(), y.toInt(), delay);
}

void DmPrivate::setKeyDelayDelta(double delta)
{
    m_keyDelayDelta = delta;
}

void DmPrivate::sendKey(const KeyOper &oper,
                    const int &vk,
                    int delay)
{
    switch (oper) {
    case Stroke:
        m_dm.KeyPress(vk);
        break;
    case Down:
        m_dm.KeyDown(vk);
        break;
    case Up:
        m_dm.KeyUp(vk);
        break;
    default:
        break;
    }

    if (delay > 0)
        approxSleep(delay, m_keyDelayDelta);
}

void DmPrivate::sendKey(const KeyOper &oper,
                    const QString &vk,
                    int delay)
{
    switch (oper) {
    case Stroke:
        m_dm.KeyPressChar(vk);
        break;
    case Down:
        m_dm.KeyDownChar(vk);
        break;
    case Up:
        m_dm.KeyUpChar(vk);
        break;
    default:
        break;
    }

    if (delay > 0)
        approxSleep(delay, m_keyDelayDelta);
}
