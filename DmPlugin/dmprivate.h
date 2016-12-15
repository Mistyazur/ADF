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

enum MouseOper {Move, Left, LeftDown, LeftUp, Right, RightDown, RightUp};
enum KeyOper {Stroke, Down, Up};

class DmPrivate : public QThread
{
    Q_OBJECT
public:
    explicit DmPrivate(QObject *parent = 0);
    ~DmPrivate();

    static void dmPluginSetup();
    static bool dmPluginReg(const QString &sendKey, const QString &flag);

    void setResourcePath(const QString &path);
signals:

public slots:

protected:
    dmsoft m_dm;

    void mdsleep(int msec, float delta=0.1);
    void sendMouse(const MouseOper &oper,
               const int &x=-1,
               const int &y=-1,
               const int delay=-1);
    void sendMouse(const MouseOper &oper,
               const QVariant &x=-1,
               const QVariant &y=-1,
               int delay=-1);
    void sendKey(const KeyOper &oper,
             const int &vk,
             int delay=0);
    void sendKey(const KeyOper &oper,
             const QString &vk,
             int delay=0);
private:

};

#endif // DMPRIVATE_H
