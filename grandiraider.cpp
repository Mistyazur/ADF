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
    Flow preFlow = Unknown;
    Flow flow = ResetRoleCount;
    int sectionIndex = 0;
    bool ok = false;
    bool waitForDpReset = false;
    bool cross = false;
    QTime timer;

    timer.start();

    if (!initDungeonSettings(DUNGEON))
        return;
    
    while (true) {
        try {
            switch (flow) {
            case ResetRoleCount:
                if (waitForDpReset) {
                    if (resetRoleCount(DUNGEON)) {
                        waitForDpReset = false;
                        flow = StartClient;
                    }
                    approxSleep(120000, 0.3);
                } else {
                    resetRoleCount(DUNGEON);
                    flow = StartClient;
                }
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
                flow = InitRole;
                break;
            case InitRole:
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
                    flow = PreFight;
                } else {
                    qDebug()<<"Enter dungeon: failed";
                    throw DFRESTART;
                }
                break;
            case PreFight:
                sectionIndex = getSectionIndex();

                if (sectionIndex < 0) {
                    if (sectionIndex == -2) {
                        flow = PreBossFight;
                        break;
                    }
                    continue;
                }

                if (isSectionClear()) {
                    flow = Navigate;
                    break;
                }

                summonSupporter();

                if (sectionIndex == 0) {
                    // Window maybe pop up when first summonx
                    sendKey(Sk, 32, 100);

                    summonSupporter();
                    useOwnSkill();
                    approxSleep(1000);
                    buff();
                } else if (sectionIndex == 2) {
                    // Awaken monsters
                    navigate(600, -1);
                } else if (sectionIndex == 6) {
                    // Get close to generator
                    navigate(350, 380);
                } else if (sectionIndex == 7) {
                    // Avoid damage
                    navigate(0, -1);
                }

                flow = Fight;
                break;
            case Fight:
                sectionIndex = getSectionIndex();
                if (sectionIndex < 0)
                    continue;

                if (isSectionClear()) {
                    useOwnSkill();
                    summonSupporter();
                    flow = PickTrophies;
                    break;
                }

                if (timer.elapsed() > 40000) {
                    // Tempester may be disappeared
                    useOwnSkill();
                    summonSupporter();
                    approxSleep(200);
                }

                if (sectionIndex == 6) {
                    // Destory generator
                    sendKey(Dn, m_arrowL);
                    for (int i=0; i<10; ++i)
                        sendKey(Sk, "x");
                    sendKey(Up, m_arrowL);
                } else {
                    // Pick trophies
                    if (pickTrophies(cross)) {
                        if (!cross) {
                            // Normal attack
                            for (int i=0; i<5; ++i)
                                sendKey(Sk, "x");
                        }
                    }

                    // Cross map
                    if (cross) {
                        flow = PreFight;
                        break;
                    }
                }
                break;
            case PickTrophies:
                if (pickTrophies(cross)) {
                    if (cross)
                        flow = PreFight;
                    else
                        flow = Navigate;
                }
                break;
            case Navigate:
                sectionIndex = getSectionIndex();
                if (sectionIndex < 0)
                    continue;

                ok = false;
                for (int i=0; i<2; ++i) {
                    if (navigateSection(sectionIndex)) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    qDebug()<<"NavigateSection failed: Section index is"<<sectionIndex;
                    throw DFRESTART;
                }

                flow = PreFight;
                break;
            case PreBossFight:
                summonSupporter();

                // Move a litte to make boss not able to teleport
                navigate(350, 400);
                approxSleep(2000);

                flow = BossFight;
                break;
            case BossFight:
                // Check dungeon status
                if (isDungeonEnded()) {

                    // Move to stones
                    moveRole(1, 2, -1, 2);

                    // Get free card
                    pickFreeGoldenCard();

                    // Stop moving
                    moveRole(1, 0, 1, 0);

                    // Destory stones
                    sendKey(Dn, m_arrowR);
                    for (int i = 0; i < 40; ++i)
                        sendKey(Sk, "x");
                    sendKey(Up, m_arrowR);
                    approxSleep(200);

                    // Move trophies (-)
                    sendKey(Dn, 189, 100);
                    sendKey(Up, 189, 100);

                    // Pick trophies
                    int oldKeyDelay = setKeyDelay(10);
                    for (int i = 0; i < 80; ++i) {
                        sendKey(Sk, "x");
                    }
                    setKeyDelay(oldKeyDelay);

                    // Sell trophies
                    sellEquipment();

                    // Buy something from store
                    buyMaterials();

                    // Close packet
                    openSystemMenu();
                    closeSystemMenu();

                    approxSleep(100);

                    // Get exp capsule
                    sendMouse(Left, 780, 590, 100);

                    // Next role if no dungeon point
                    if (isNoDungeonPoint()) {
                        // F12
                        sendKey(Dn, 123, 200);
                        sendKey(Up, 123, 4000);
                        flow = RoleSummary;
                        break;
                    }

                    // Reenter dungeon
                    if (reenterDungeon()) {
                        flow = PreFight;
                        continue;
                    } else {
                        qDebug()<<"reenterDungeon failed";
                        throw DFRESTART;
                    }
                } else {
                    killBoss();
                }

                break;
            case RoleSummary:
                checkMail();
                playMercenary();
                repickCurrentRole();
                updateShareStorage();
                flow = UpdateRoleIndex;
            case UpdateRoleIndex:
                qDebug()<<"Role index"<<(m_firstRoleIndex + m_roleCount);
                if (resetRoleCount(DUNGEON)) {
                    backToRoleList();
                    flow = PickRole;
                } else {
                    if (updateRoleCount(DUNGEON)) {
                        backToRoleList();
                        flow = PickRole;
                    } else {
                        // Job finished
                        qDebug()<<"GRANDI: COMPLETED";

                        // Close client
                        closeClient();

                        // Wait for dungeon point reset
                        flow = ResetRoleCount;
                        waitForDpReset = true;
                    }
                }
                break;
            default:
                break;
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

            // Check disconnected
            if (isDisconnected()) {
                qDebug()<<"Disconnected";
                throw DFRESTART;
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
                qDebug()<<"Error[Restart]:"<<"Role index is"<<(m_firstRoleIndex + m_roleCount);
                m_dm.CapturePng(CLIENT_RECT, tr("%1_%2.png")
                                .arg(QDateTime::currentDateTime().toString("MMdd_HHmmss"))
                                .arg(flow));
                flow = StartClient;
                continue;
            }
        }
    }
}
