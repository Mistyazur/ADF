#ifndef GRANDIRAIDER_H
#define GRANDIRAIDER_H

#include "df.h"

class GrandiRaider : public DF
{
    enum Flow{StartClient, BindClient, PickRole, Init, MoveToDungeon, PreFight, Fight, PickTrophies, Navigate, PreBossFight, BossFight, RoleSummary, UpdateRoleIndex};
public:
    GrandiRaider();
    ~GrandiRaider();
    void run();
};

#endif // GRANDIRAIDER_H
