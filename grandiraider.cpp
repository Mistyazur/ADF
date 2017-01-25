#include "grandiraider.h"

#include <QDebug>
#include <QString>

#define DUNGEON "Grandi"
#define GRANDI_MAP_RECT 720, 50, 795, 105

GrandiRaider::GrandiRaider()
{
    setResourcePath("Images");
}

GrandiRaider::~GrandiRaider()
{
    unbind();
}

void GrandiRaider::run()
{
    Flow flow = StartClient;
    int sectionIndex = 0;
    int timeElapsed = 0;
    QTime timer;

    timer.start();
    m_preFlow = (Flow)-1;

    if (!initDungeonSettings(DUNGEON))
        return;
    

//    flow = PreFight;
//    bind(false);


    while (true) {
        try {
            switch (flow) {
            case StartClient:
                if (window() != 0) {
                    unbind();
                    closeClient();
                    msleep(5000);
                }
                if (startClient()) {
                    flow = BindClient;
                } else {
                    qDebug()<<"StartClient failed";
                }
                break;
            case BindClient:
                if (bind(false)) {
                    flow = PickRole;
                } else {
                    qDebug()<<"Bind client failed";
                    throw DFRESTART;
                }
                break;
            case PickRole:
                pickRole();

                // Check dungeon point
                if (isNoDungeonPoint()) {
                    flow = BackToRoleList;
                    break;
                }

                initRoleOffset();

                flow = MoveToDungeon;
                break;
            case MoveToDungeon:
                navigateOnMap(645, 280, 15000);
                if (enterDungeon(4, 2, false)) {
                    sectionIndex = 0;
                    flow = PreFight;
                } else {
                    qDebug()<<"Enter dungeon: failed";
                    throw DFRESTART;
                }
                break;
            case PreFight:
                // Summon tempester
                summonSupporter();

                if (sectionIndex == 0) {
                    // Buff
                    buff();
                } else if (sectionIndex == 4) {
                    // Get close to generator
                    navigate(-1, 380);
                    navigate(300, -1);
                } else if (sectionIndex == 5) {
                    // Avoid damage
                    moveRole(0, 1, 2);
                    approxSleep(2000, 0.3);
                    moveRole(0, 1);
                }

                flow = Fight;
                break;
            case Fight:
                // Check section state
                if (isSectionClear("216979-051F1F")) {
                    qDebug()<<"Section Clear";
                    approxSleep(200);
                    flow = PickTrophies;
                    break;
                }

                timeElapsed = timer.elapsed();
                if (timeElapsed > 40000) {
                    // Tempester may be disappeared
                    summonSupporter();
                    approxSleep(200);
                } else if (timeElapsed > 60000) {
                    // First section maybe not has clear effect
                    // So we assume it's clear, if it has costed 30 secs
                    if (sectionIndex == 0) {
                        qDebug()<<"Section Clear[timeout]";
                        approxSleep(200);
                        flow = PickTrophies;
                        break;
                    }
                }

                if (sectionIndex == 4) {
                    // Destory generator
                    sendKey(Stroke, m_arrowL, 30);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                } else {
                    // Normal attack
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                }
                break;
            case PickTrophies:
            {
                static bool done = false;
                pickTrophies(done);
                if (done) {
                    flow = Navigate;
                }
                break;
            }
            case Navigate:
            {
                rectifySectionIndex(sectionIndex);

                bool bossRoomArrived;
                if (navigateSection(sectionIndex, bossRoomArrived)) {
                    ++sectionIndex;

                    if (bossRoomArrived) {
                        flow = PreBossFight;
                    } else {
                        flow = PreFight;
                    }
                    break;
                } else {
                    qDebug()<<"navigateSection failed";
                    throw DFRESTART;
                }
                break;
            }
            case PreBossFight:
                // Summon tempester
                summonSupporter();

                flow = BossFight;
                break;
            case BossFight:
                // Check dungeon status
                if (isDungeonEnded()) {
                    moveRole(1, 1);

                    approxSleep(3000, 0.2);

                    // Pick trophies
                    sendKey(Stroke, 189, 600);  // -
                    sendKey(Down, "x", 3000);
                    sendKey(Up, "x");

                    // Sell trophies
                    sellEquipment();
                    approxSleep(100);

                    // Check dungeon point
                    if (isNoDungeonPoint()) {
                        sendKey(Stroke, 123, 1000);  // Esc
                        flow = BackToRoleList;
                        break;
                    }

                    // Reenter dungeon
                    if (reenterDungeon()) {
                        flow = PreFight;
                        sectionIndex = 0;
                        continue;
                    } else {
                        qDebug()<<"reenterDungeon failed";
                        throw DFRESTART;
                    }
                }

                fightBoss();
                break;
            case BackToRoleList:
                if (!updateRoleIndex(DUNGEON)) {
                    // Job finished
                    qDebug()<<"Grandi automating finished";
                    closeClient();
                    return;
                }

                backToRoleList();
                flow = PickRole;
                break;
            default:
                break;
            }

            // Check disconnected
            if (isDisconnected()) {
                qDebug()<<"Disconnected";
                throw DFRESTART;
            }

            // Check timeout
            if (flow != m_preFlow) {
                timer.restart();
                m_preFlow = flow;
            } else {
                if (flow > BindClient) {
                    if (timer.elapsed() > 120000) {
                        qDebug()<<"Timer hit 2 min:"<<flow;
                        throw DFRESTART;
                    }
                }
            }

            // Check death
            if (isRoleDead()) {
                qDebug()<<"Role is dead";
                throw DFRESTART;
            }

        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
            if (e == DFSettingError) {
                return;
            } else if (e == DFRESTART) {
                flow = StartClient;
                continue;
            }
        }
    }
}
