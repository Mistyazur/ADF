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
    int m_roleOffsetY;

    // Client functions
    int window();
    bool bind(bool underground=false);
    void unbind();
    
    // Common functions
    void setArrowKey(int left, int up, int right, int down);
    bool isBlackScreen(int x1, int y1, int x2, int y2);
    void switchRole(int index);
    void teleport(const QString &destination);
    void navigateOnMap(int x, int y);

    // Dungeon functions
    bool initRoleOffset();
    bool enterDungeon(int index, int difficulty, bool leftEntrance=true);
    bool dungeonIn();
    bool dungeonEnd();
    bool summonSupporter();
    void buff();
    bool isSectionClear(int x1, int y1, int x2, int y2,
                        const QString &brightColor,
                        bool isFirstSection);
    bool getTrophyCoords(int roleX, int roleY, int &x, int &y);
    bool pickTrophies();
    bool getRoleCoordsInMap(int x1, int y1, int x2, int y2, int &x, int &y);
    bool getRoleCoords(int &x, int &y);
    void moveRole(int hDir, int vDir, int speed=0);
    bool navigate(int x, int y, bool end);
};

#endif // DF_H
