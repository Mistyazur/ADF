#include "grandiraider.h"

#include <QDebug>
#include <QString>

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

    Flow flow = PickRole;
    int sectionIndex = 0;
    bool moveRoleUsed = false;

    if (!bind(false))
        return;

    if (!initSettings("ADF.json"))
        return;

    flow = PreFight;
    m_roleOffsetY = 150;

//    sellEquipment();
//    unbind();
//    return;

//    while (true) {

////        if (getTrophyCoords(x, y)) {
////            qDebug()<<"Trophy"<<x<<y;
////            navigate(x, -1, false);
////            navigate(-1, y, false);
////            approxSleep(500);
////            sendKey(Stroke, "x", 100);
////        }
//        pickTrophies();
//        msleep(10);
//    }

    QTime fightingTimer;
    fightingTimer.start();

    while (true) {
        try {
            switch (flow) {
            case PickRole:
                initRoleOffset();
                flow = MoveToDungeon;
                break;
            case MoveToDungeon:
                navigateOnMap(645, 280, 15000);
                if (enterDungeon(4, 2, false)) {
                    sectionIndex = 0;
                    flow = PreFight;
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
                    navigate(-1, 390);
                    navigate(200, -1);
                } else if (sectionIndex == 5) {
                    // Avoid damage
                    moveRole(0, 1, 2);
                    approxSleep(2000, 0.3);
                    moveRole(0, 1);
                }

                fightingTimer.restart();
                flow = Fight;
                break;
            case Fight:
                // Check section state
                if (isSectionClear(GRANDI_MAP_RECT, "216979-051F1F", sectionIndex == 0)) {
                    qDebug()<<"Section Clear";
                    approxSleep(200);
                    flow = PickTrophies;
                    break;
                }

                // Check supporter
                if (fightingTimer.elapsed() > 40000) {
                    summonSupporter();
                    approxSleep(5000);
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
                if (!pickTrophies()) {
                    flow = Navigate;
                }
                break;
            case Navigate:
            {
                rectifySectionIndex(GRANDI_MAP_RECT, sectionIndex);

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

                }

//                if (sectionIndex < pathList.count()) {
//                    bool success = false;
//                    bool end = false;
//                    const QVariantList &sectionPathList = pathList.at(sectionIndex++).toList();
//                    for (int i = 0; i < sectionPathList.count(); ++i) {
//                        QVariantList &position = sectionPathList.at(i).toList();
//                        if (position.count() < 2) {
//                            qDebug()<<"Path is not acceptable";
//                            return;
//                        }

//                        end = (i == (sectionPathList.count() - 1)) ? true : false;
//                        success = navigate(position.first().toInt(), position.last().toInt(), end);
//                        if (success) {
//                            // Pre-fight
//                            flow = PreFight;
//                            break;
//                        } else {
//                            if (end) {
//                                qDebug()<<"Navigate error";
//                                if (!moveRoleUsed) {
//                                    qDebug()<<"Reset";
//                                    sendKey(Stroke, 187, 500);
//                                    moveRoleUsed = true;
//                                }
//                            }
//                        }
//                    }
//                }
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

                    // sell trophies
                    sellEquipment();
                    approxSleep(100);

                    // reenter dungeon
                    if (reenterDungeon()) {
                        flow = PreFight;
                        sectionIndex = 0;
                        moveRoleUsed = false;
                        continue;
                    } else {
                        qDebug()<<"error: restart game";
                    }
                }

                fightBoss();
                break;
            default:
                break;
            }

        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
            if (e == DFSettingError) {
                return;
            }
        }
    }
}
