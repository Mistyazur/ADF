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

enum MouseOper {Move, Left, LeftDown, LeftUp, Right, RightDown, RightUp};
enum KeyOper {Stroke, Down, Up};

class DmPrivate : public QThread
{
    Q_OBJECT
public:
    explicit DmPrivate(QObject *parent = 0);
    ~DmPrivate();

    static void dmPluginSetup(bool local=false);
    static bool dmPluginReg(const QString &sendKey, const QString &flag, const QString &guard=QString());

    void setResourcePath(const QString &path);
signals:

public slots:

protected:
    dmsoft m_dm;

    void approxSleep(int msec, double delta=0.1);
    void setMouseDelayDelta(double delta);
    void sendMouse(const MouseOper &oper,
               const int &x=-1,
               const int &y=-1,
               const int delay=-1);
    void sendMouse(const MouseOper &oper,
               const QVariant &x=-1,
               const QVariant &y=-1,
               int delay=-1);
    void setKeyDelayDelta(double delta);
    void sendKey(const KeyOper &oper,
             const int &vk,
             int delay=0);
    void sendKey(const KeyOper &oper,
             const QString &vk,
             int delay=0);
private:
    double m_mouseDelayDelta;
    double m_keyDelayDelta;
};

#endif // DMPRIVATE_H
