#ifndef GRANDIRAIDER_H
#define GRANDIRAIDER_H

#include "df.h"

class GrandiRaider : public DF
{
    enum Flow{StartClient, BindClient, PickRole, Init, MoveToDungeon, PreFight, Fight, PickTrophies, Navigate, PreBossFight, BossFight, UpdateShareStorage, UpdateRoleIndex};
public:
    GrandiRaider();
    ~GrandiRaider();
    void run();
private:
    Flow m_preFlow;
};

#endif // GRANDIRAIDER_H
