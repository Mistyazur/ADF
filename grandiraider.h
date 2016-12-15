#ifndef GRANDIRAIDER_H
#define GRANDIRAIDER_H

#include "df.h"

class GrandiRaider : public DF
{
    enum Flow{PickRole, MoveToDungeon, Fight, Navigate, FightBoss};
public:
    GrandiRaider();
    ~GrandiRaider();
    void run();
private:
    Flow m_flow;
};

#endif // GRANDIRAIDER_H
