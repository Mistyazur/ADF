#ifndef DF_H
#define DF_H

#include "DmPlugin/dmprivate.h"

#define CLIENT_RECT 0, 0, 800, 600

enum DFError { DFCrashed , DFDisconnected};

class DF : public DmPrivate
{
public:
    DF();
    virtual ~DF();
protected:
    int m_arrowL;
    int m_arrowU;
    int m_arrowR;
    int m_arrowD;
    int m_roleOffsetY;
    QVariantList m_pathList;
    QVariantList m_nodeList;


    // Client functions
    int window();
    bool bind(bool underground=false);
    void unbind();
    
    // Common functions
    void setArrowKey(int left, int up, int right, int down);
    bool isBlackScreen(int x1, int y1, int x2, int y2);
    void switchRole(int index);
    void teleport(const QString &destination);
    void navigateOnMap(int x, int y, int time);
    void sellEquipment();

    // Dungeon functions
    void initSettings(const QString &file);
    bool initRoleOffset();
    bool enterDungeon(int index, int difficulty, bool leftEntrance=true);
    bool reenterDungeon();
    bool isInDungeon();
    bool isDungeonEnded();
    bool summonSupporter();
    void buff();
    void rectifySectionIndex(int x1, int y1, int x2, int y2, int &sectionIndex);
    bool isSectionClear(int x1, int y1, int x2, int y2,
                        const QString &brightColor,
                        bool isFirstSection);
    bool getTrophyCoords(int &x, int &y, bool &pickable);
    bool getRoleCoordsInMap(int x1, int y1, int x2, int y2, int &x, int &y);
    bool getRoleCoords(int &x, int &y);
    void moveRole(int hDir, int vDir, int speed=0);
    bool pickTrophies();
    bool navigate(int x, int y, bool end=false);
    bool navigateSection(int sectionIndex, bool &bossRoomArrived);
    bool fightBoss();
private:
    QTime *m_firstSectionTimer;
};

#endif // DF_H
