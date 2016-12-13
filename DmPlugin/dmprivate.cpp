#include "dmprivate.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>

DmPrivate::DmPrivate(QObject *parent) :
    QThread(parent)
{
    //Ingore errors showed by dm plugin
    m_dm.SetShowErrorMsg(0);
}

DmPrivate::~DmPrivate()
{
    wait();
}

void DmPrivate::dmPluginSetup()
{
    QProcess proc;

    proc.start("regsvr32 /s dm.dll");
    proc.waitForStarted();
    proc.waitForFinished();
    proc.close();

    msleep(1000);
}

bool DmPrivate::dmPluginReg(const QString &key, const QString &flag)
{
    dmsoft dm;
    if (dm.Ver() != DM_VER)
        return false;

    if (dm.Ver().toDouble() > 3.1233) {
        int code = dm.Reg(key,flag);
        if (code != 1) {
            qWarning() << "Register failed:" <<code;
            return false;
        }
    }

    if (!dm.BindWindow(dm.GetForegroundWindow(),"normal","normal","normal",0)) {
//        if (m_dm.GetOsType()==3
//                ||m_dm.GetOsType()==4
//                ||m_dm.GetOsType()==5)
//        {
//            //Check if UAC is enabled
//            if (m_dm.CheckUAC())
//            {
//                //Close UAC and restart system
//                if (m_dm.SetUAC(0))
//                {
//                    qWarning() << "UAC is disabled, But it will effect after system restarting.";
//                    m_dm.ExitOs(2);
//                    return false;
//                }
//            }
//        }

        return false;
    }

    dm.UnBindWindow();
    dm.DisablePowerSave();
    dm.DisableScreenSave();

    return true;
}

void DmPrivate::setResourcePath(const QString &path)
{
    m_dm.SetPath(qApp->applicationDirPath()+"/" + path);
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const int &x,
                      const int &y,
                      const int delay)
{
    m_dm.MoveTo(x,y);
    msleep(10);

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
        msleep(delay);
}

void DmPrivate::sendMouse(const MouseOper &oper,
                      const QVariant &x,
                      const QVariant &y,
                      int delay)
{
    sendMouse(oper, x.toInt(), y.toInt(), delay);
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
        msleep(delay);
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
        msleep(delay);
}
