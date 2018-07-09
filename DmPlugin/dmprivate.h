#ifndef DMPRIVATE_H
#define DMPRIVATE_H

#ifdef DM_FREE
#include "dmfree/dm.h"
using namespace Dm;
#endif

#ifdef DM_REG
#include "dmreg/dm.h"
using namespace dm;
#endif

#include <QThread>

#define MEDIAN_RANDOM(min, max) min+qrand()%(max-min+1)

enum MouseOper {Move, Left, LeftDn, LeftUp, Right, RightDn, RightUp};
enum KeyOper {Sk, Dn, Up};

class DmPrivate : public QThread
{
    Q_OBJECT
public:
    explicit DmPrivate(QObject *parent = 0);
    ~DmPrivate();

    static void dmPluginSetup(bool local=false);
    static bool dmPluginReg(const QString &sendKey, const QString &flag);

    void setResourcePath(const QString &path);
signals:

public slots:

protected:
    dmsoft m_dm;

    int randomNumber(int min, int max);

    void approxSleep(int msec, double delta=0.1);

    bool activateWindow(HWND hWnd);

    int setMouseDuration(int duration);
    int setKeyDuration(int duration);
    void setMouseDurationDelta(double delta);
    void setKeyDurationDelta(double delta);

    int setMouseDelay(int delay);
    int setKeyDelay(int delay);
    void setMouseDelayDelta(double delta);
    void setKeyDelayDelta(double delta);

    void sendMouse(const MouseOper &oper,
               const int &x=-1,
               const int &y=-1,
               const int extraDelay=-1);
    void sendMouse(const MouseOper &oper,
               const QVariant &x=-1,
               const QVariant &y=-1,
               int extraDelay=-1);
    void sendKey(const KeyOper &oper,
             const int &vk,
             int extraDelay=0);
    void sendKey(const KeyOper &oper,
             const QString &vk,
             int extraDelay=0);
private:
    int m_mouseDuration;
    int m_keyDuration;
    double m_mouseDurationDelta;
    double m_keyDurationDelta;

    int m_mouseDelay;
    int m_keyDelay;
    double m_mouseDelayDelta;
    double m_keyDelayDelta;
};

#endif // DMPRIVATE_H
