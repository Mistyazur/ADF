#ifndef DF_H
#define DF_H

#include "DmPlugin/dmprivate.h"

#define CLIENT_RECT 0, 0, 800, 600

enum DFError { DFSettingError, DFRESTART };

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
    int m_dungeonMapX1;
    int m_dungeonMapY1;
    int m_dungeonMapX2;
    int m_dungeonMapY2;
    int m_roleCount;
    int m_lastRoleIndex;
    QVariantList m_pathList;
    QVariantList m_nodeList;


    // Client functions
    int window();
    bool bind(bool underground=false);
    void unbind();
    
    // Common functions
    bool closeClient();
    bool startClient();
    bool waitForRoleList();
    bool openSystemMenu();
    bool closeSystemMenu();
    bool isDisconnected();
    void setArrowKey(int left, int up, int right, int down);
    bool isBlackScreen(int x1, int y1, int x2, int y2);
    void pickRole(int index);
    void repickCurrentRole();
    void backToRoleList();
    void teleport(const QString &destination);
    void navigateOnMap(int x, int y, int time);
    void sellEquipment();
    void checkMail();
    void updateShareStorage();

    // Dungeon functions
//    bool initDungeonSettings(const QString &file);
    bool initDungeonSettings(const QString &dungeon);
    bool resetRoleIndex(const QString &dungeon);
    bool updateRoleIndex(const QString &dungeon);
    void pickRole();
    bool initRoleOffset();
    bool enterDungeon(int index, int difficulty, bool leftEntrance=true);
    bool reenterDungeon();
    bool waitForDungeonBeign();
    bool isRoleDead();
    bool isDungeonEnded();
    bool isNoDungeonPoint();
    void pickFreeGoldenCard();
    void summonSupporter();
    void useOwnSkill();
    void buff();
    void rectifySectionIndex(int &sectionIndex);
    bool isSectionClear(const QString &brightColor, const int threshold);
    bool isPickable();
    bool getNearestTrophyCoords(int x, int y, int &nx, int &ny);
    bool getTrophyCoords(int &nx, int &ny);
    bool getRoleCoordsInMap(int &x, int &y);
    bool getRoleCoords(int &x, int &y);
    void moveRole(int hDir, int vDir, int speed=0);
    void pickTrophies(bool &done);
    bool navigate(int x, int y, bool end=false);
    bool navigateSection(int sectionIndex, bool &boss);
    bool fightBoss();
private:
    int m_hBindWnd;
    int m_roleOffsetY;
};

#endif // DF_H
