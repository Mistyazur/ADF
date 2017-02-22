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
    static Flow preFlow = (Flow)-1;
    Flow flow = StartClient;
    int sectionIndex = 0;

    QTime timer;
    timer.start();

//    flow = PreFight;
//    bind(false);

    if (!initDungeonSettings(DUNGEON))
        return;
    
    while (true) {
        try {
            switch (flow) {
            case WaitForReset:
                if (resetRoleIndex(DUNGEON)) {
                    flow = StartClient;
                }
                approxSleep(300000, 0.3);
                break;
            case StartClient:
                closeClient();
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
                flow = Init;
                break;
            case Init:
                if (initRoleOffset()) {
                    // Check dungeon point
                    if (isNoDungeonPoint())
                        flow = UpdateRoleIndex;
                    else
                        flow = MoveToDungeon;
                }
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
                summonSupporter();

                if (sectionIndex == 0) {
                    // Window maybe pop up when first summon
                    sendKey(Down, 32, 200);
                    sendKey(Up, 32, 100);

                    summonSupporter();
                    useOwnSkill();
                    buff();

                    // Awaken monsters
                    navigate(470, -1);
                    sendKey(Down, m_arrowL, 100);
                    sendKey(Up, m_arrowL);
                } else if (sectionIndex == 2) {
                    // Awaken monsters
                    navigate(470, -1);
                    sendKey(Down, m_arrowL, 100);
                    sendKey(Up, m_arrowL);
                } else if (sectionIndex == 4) {
                    // Get close to generator
                    navigate(350, 390);
                } else if (sectionIndex == 5) {
                    // Avoid damage
                    navigate(-1, 0);
                }

                flow = Fight;
                break;
            case Fight:
                // Check section state
                if (isSectionClear("2B6272-1F1F1F")) {
                    useOwnSkill();
                    summonSupporter();

                    approxSleep(200);
                    flow = PickTrophies;
                    break;
                }

                if (timer.elapsed() > 40000) {
                    // Tempester may be disappeared
                    summonSupporter();
                    approxSleep(200);
                }

                if (sectionIndex == 4) {
                    // Destory generator
                    sendKey(Down, m_arrowL, 100);
                    sendKey(Up, m_arrowL);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x");
                } else {
                    // Normal attack
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x");
                }
                break;
            case PickTrophies:
            {
                bool done = false;
                pickTrophies(done);
                if (done) {
                    flow = Navigate;
                }
            }
                break;
            case Navigate:
            {
                rectifySectionIndex(sectionIndex);

                bool bossRoomArrived;
                bool ok = false;
                for (int i=0; i<2; ++i) {
                    if (navigateSection(sectionIndex, bossRoomArrived)) {
                        ok = true;
                        break;
                    }
                }
                if (ok) {
                    ++sectionIndex;
                    if (bossRoomArrived) {
                        flow = PreBossFight;
                    } else {
                        flow = PreFight;
                    }
                } else {
                    qDebug()<<"NavigateSection failed: Index"<<sectionIndex;
                    throw DFRESTART;
                }
            }
                break;
            case PreBossFight:
                // Summon tempester
                summonSupporter();

                // Move a litte to make boss not able to teleport
                navigate(350, -1);

                flow = BossFight;
                break;
            case BossFight:
                // Check dungeon status
                if (isDungeonEnded()) {
                    moveRole(1, 1);

                    // Wait for thophies fall on ground
                    // 3000 is minimum
                    approxSleep(3000);

                    // Move trophies (-)
                    sendKey(Down, 189, 200);
                    sendKey(Up, 189, 100);

                    // Pick trophies
                    for (int i = 0; i < 80; ++i) {
                        sendKey(Stroke, "x");
                    }

                    // Get free card
                    pickFreeGoldenCard();

                    // Sell trophies
                    sellEquipment();
                    approxSleep(100);

                    // Next role if no dungeon point
                    if (isNoDungeonPoint()) {
                        // F12
                        sendKey(Down, 123, 200);
                        sendKey(Up, 123, 5000);
                        flow = RoleSummary;
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
            case RoleSummary:
                checkMail();
                repickCurrentRole();
                updateShareStorage();
                flow = UpdateRoleIndex;
            case UpdateRoleIndex:
                if (!resetRoleIndex(DUNGEON)) {
                    if (!updateRoleIndex(DUNGEON)) {
                        // Job finished
                        qDebug()<<"Grandi automating completed";

                        // Close client and wait for game reset
                        closeClient();
                        flow = WaitForReset;
                        break;
                    }
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
            if (flow != preFlow) {
                timer.restart();
                preFlow = flow;
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
            qDebug()<<"DFError"<<e<<m_lastRoleIndex;
            if (e == DFSettingError) {
                return;
            } else if (e == DFRESTART) {
                flow = StartClient;
                continue;
            }
        }
    }
}
