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
    Flow preFlow = (Flow)-1;
    Flow flow = StartClient;
    int sectionIndex = 0;
    bool ok = false;
    bool bossArrived = false;

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
                    else {
                        cancelCrystalContract();
                        flow = MoveToDungeon;
                    }
                }
                break;
            case MoveToDungeon:
                navigateOnMap(644, 280, 15000);
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
                    buff();
                    useOwnSkill();
                } else if (sectionIndex == 2) {
                    // Awaken monsters
                    navigate(600, -1);
                } else if (sectionIndex == 4) {
                    // Get close to generator
                    navigate(350, 390);
                } else if (sectionIndex == 5) {
                    // Avoid damage
                    navigate(0, -1);
                }

                flow = Fight;
                break;
            case Fight:
                if (timer.elapsed() > 40000) {
                    // Tempester may be disappeared
                    useOwnSkill();
                    summonSupporter();
                    approxSleep(200);
                } else if (timer.elapsed() > 2000) {
                    // Check section state
                    if (isSectionClear("59a2a3-101010|1f5877-101010", 110)) {
                        useOwnSkill();
                        summonSupporter();
                        flow = PickTrophies;
                        break;
                    }
                }

                if (sectionIndex == 4) {
                    // Destory generator
                    sendKey(Down, m_arrowL, 100);
                    sendKey(Up, m_arrowL);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x");
                } else {
                    // Pick trophies
                    if (isTrophyExisting()) {
                        approxSleep(200);
                        pickTrophies(ok);
                    } else {
                        // Normal attack
                        for (int i=0; i<5; ++i)
                            sendKey(Stroke, "x");
                    }
                }
                break;
            case PickTrophies:
                pickTrophies(ok);
                if (ok) {
                    flow = Navigate;
                }
                break;
            case Navigate:
                rectifySectionIndex(sectionIndex);

                ok = false;
                for (int i=0; i<2; ++i) {
                    if (navigateSection(sectionIndex, bossArrived)) {
                        ok = true;
                        break;
                    }
                }
                if (ok) {
                    ++sectionIndex;
                    if (bossArrived) {
                        flow = PreBossFight;
                    } else {
                        flow = PreFight;
                    }
                } else {
                    qDebug()<<"NavigateSection failed: Section index is"<<sectionIndex;
                    throw DFRESTART;
                }
                break;
            case PreBossFight:
                summonSupporter();

                // Move a litte to make boss not able to teleport
                navigate(350, 400);
                approxSleep(1000);

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

                    // Get exp capsule
                    sendMouse(Left, 780, 590, 100);

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
                        qDebug()<<"GRANDI: COMPLETED";

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
                        qDebug()<<"Timer hit 2 min: Flow is"<<flow;
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
            if (e == DFSettingError) {
                qDebug()<<"Error[Settings]";
                return;
            } else if (e == DFRESTART) {
                qDebug()<<"Error[Restart]:"<<"Role index is"<<m_lastRoleIndex;
                flow = StartClient;
                continue;
            }
        }
    }
}
