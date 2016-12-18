#ifndef DF_H
#define DF_H

#include "DmPlugin/dmprivate.h"

#define CLIENT_RECT 0, 0, 800, 600

enum DFError { DFCrashed , DFDisconnected};

class DF : public DmPrivate
{
public:
    DF();
protected:
    int m_arrowL;
    int m_arrowU;
    int m_arrowR;
    int m_arrowD;

    // Client functions
    int window();
    bool bind(bool underground=false);
    void unbind();
    
    // Common functions
    void setArrowKey(int left, int up, int right, int down);
    void switchRole(int index);
    void teleport(const QString &destination);
    void navigateOnMap(int x, int y);

    // Dungeon functions
    bool enterDungeon(int index, int difficulty, bool leftEntrance=true);
    bool reenterDungeon();
    bool summonSupporter();
    bool isSectionClear(bool isFirstSection);
    bool navigate(int x=-1, int y=-1);
    bool getRoleCoords(int &x, int &y);
    void hideDropName(bool enable);
    void moveRole(int horizontal=0, int vertical=0, int speed=0);
    void stopRole(int horizontal=0, int vertical=0);
};

#endif // DF_H
