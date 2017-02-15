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
    m_mouseDuration = 0;
    m_keyDuration = 0;
    m_mouseDurationDelta = 0;
    m_keyDurationDelta = 0;

    m_mouseDelay = 0;
    m_keyDelay = 0;
    m_mouseDelayDelta = 0;
    m_keyDelayDelta = 0;

    // Init random seed
    qsrand(time(0));

    // Ingore errors showed by dm plugin
    m_dm.SetShowErrorMsg(0);

    m_dm.SetMouseDelay("normal", 0);
    m_dm.SetKeypadDelay("normal", 0);
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

int DmPrivate::setMouseDuration(int duration)
{
    int old = m_mouseDuration;
    m_mouseDuration = duration;

    return old;
}

int DmPrivate::setKeyDuration(int duration)
{
    int old = m_keyDuration;
    m_keyDuration = duration;

    return old;
}

void DmPrivate::setMouseDurationDelta(double delta)
{
    m_mouseDurationDelta = delta;
}

void DmPrivate::setKeyDurationDelta(double delta)
{
    m_keyDurationDelta = delta;
}

int DmPrivate::setMouseDelay(int delay)
{
    int old = m_mouseDelay;
    m_mouseDelay = delay;

    return old;
}

int DmPrivate::setKeyDelay(int delay)
{
    int old = m_keyDelay;
    m_keyDelay = delay;

    return old;
}

void DmPrivate::setMouseDelayDelta(double delta)
{
    m_mouseDelayDelta = delta;
}

void DmPrivate::setKeyDelayDelta(double delta)
{
    m_keyDelayDelta = delta;
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const int &x,
                      const int &y,
                      const int extraDelay)
{
    if ((x >= 0) && (y >= 0)) {
        m_dm.MoveTo(x, y);
        approxSleep(m_mouseDelay, m_mouseDelayDelta);
    }

    switch (oper) {
    case Move:
        break;
    case Left:
        m_dm.LeftDown();
        approxSleep(m_mouseDuration, m_mouseDurationDelta);
        m_dm.LeftUp();
        break;
    case LeftDown:
        m_dm.LeftDown();
        break;
    case LeftUp:
        m_dm.LeftUp();
        break;
    case Right:
        m_dm.RightDown();
        approxSleep(m_mouseDuration, m_keyDurationDelta);
        m_dm.RightUp();
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
    if (oper != Move)
        approxSleep(m_mouseDelay, m_mouseDelayDelta);

    approxSleep(extraDelay);
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const QVariant &x,
                      const QVariant &y,
                      int extraDelay)
{
    sendMouse(oper, x.toInt(), y.toInt(), extraDelay);
}

void DmPrivate::sendKey(const KeyOper &oper,
                    const int &vk,
                    int extraDelay)
{
    switch (oper) {
    case Stroke:
        m_dm.KeyDown(vk);
        approxSleep(m_keyDuration, m_keyDurationDelta);
        m_dm.KeyUp(vk);
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
    approxSleep(m_keyDelay, m_keyDelayDelta);

    approxSleep(extraDelay);
}

void DmPrivate::sendKey(const KeyOper &oper,
                    const QString &vk,
                    int extraDelay)
{
    switch (oper) {
    case Stroke:
        m_dm.KeyDownChar(vk);
        approxSleep(m_keyDuration, m_keyDurationDelta);
        m_dm.KeyUpChar(vk);
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
    approxSleep(m_keyDelay, m_keyDelayDelta);

    approxSleep(extraDelay);
}
