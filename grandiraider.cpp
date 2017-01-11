#include "grandiraider.h"

#include "JSettings/jsettings.h"

#include <QDebug>
#include <QApplication>
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
    JSettings js(QApplication::applicationDirPath()+"/ADF.json");
    QVariantList pathList = js.value("GrandiPath").toList();
    QVariantList nodeList = js.value("GrandiNodes").toList();
    QVariantList node;
    int x, y;
    int rectifiedSectionIndex;

    Flow flow = PickRole;
    int sectionIndex = 0;
    bool moveRoleUsed = false;
    int fightingIndex = 0;

    if (!bind(false))
        return;

    flow = PreFight;
    m_roleOffsetY = 162;

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

    while (true) {
        try {
            switch (flow) {
            case PickRole:
                initRoleOffset();
                flow = MoveToDungeon;
                break;
            case MoveToDungeon:
                navigateOnMap(645, 280);
                approxSleep(15000);
                if (enterDungeon(4, 2, false)) {
                    sectionIndex = 0;
                    flow = PreFight;
                }
                break;
            case PreFight:
                // Summon tempester
                summonSupporter();

                if (sectionIndex < pathList.count()) {
                    flow = Fight;

                    if (sectionIndex == 0) {
                        // Buff
                        buff();
                    } else if (sectionIndex == 4) {
                        // Get close to generator
                        navigate(-1, 400);
                        navigate(200, -1);
                    } else if (sectionIndex == 5) {
                        // Avoid damage
                        moveRole(0, 1, 2);
                        approxSleep(2000, 0.3);
                        moveRole(0, 1);
                    }
                } else {
                    flow = FightBoss;
                }
                break;
            case Fight:
                // Check section state
                if (isSectionClear(GRANDI_MAP_RECT, "216979-051F1F", sectionIndex == 0)) {
                    qDebug()<<"isSectionClear";
                    msleep(500);
                    flow = PickTrophies;
                    break;
                }

                // Destory generator
                if (sectionIndex == 4) {
                    sendKey(Stroke, m_arrowL, 30);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                } else {
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
                if (!getRoleCoordsInMap(GRANDI_MAP_RECT, x, y))
                    continue;

                node = QVariantList({x , y});
                rectifiedSectionIndex = nodeList.indexOf(node);
                if (rectifiedSectionIndex != -1)
                    sectionIndex = rectifiedSectionIndex;

                if (sectionIndex < pathList.count()) {
                    bool success = false;
                    bool end = false;
                    const QVariantList &sectionPathList = pathList.at(sectionIndex++).toList();
                    for (int i = 0; i < sectionPathList.count(); ++i) {
                        QVariantList &position = sectionPathList.at(i).toList();
                        if (position.count() < 2) {
                            qDebug()<<"Path is not acceptable";
                            return;
                        }
                        end = (i == (sectionPathList.count() - 1)) ? true : false;
                        success = navigate(position.first().toInt(), position.last().toInt(), end);
                        if (success) {
                            // Pre-fight
                            flow = PreFight;
                            break;
                        } else {
                            if (end) {
                                qDebug()<<"Navigate error";
                                m_dm.Beep(1000, 10000);
                                if (!moveRoleUsed) {
                                    sendKey(Stroke, 187, 500);
                                    moveRoleUsed = true;
                                }
                            }
                        }
                        approxSleep(100);
                    }
                }

                break;
            }
            case FightBoss:
                // Check dungeon status
                if (dungeonEnd()) {
                    flow = PreFight;
                    sectionIndex = 0;
                    fightingIndex = 0;
                    moveRoleUsed = false;
                    continue;
                }

                if (fightingIndex == 0) {
                    moveRole(0, -1, 2);
                } else if (fightingIndex < 5) {
                    approxSleep(1000);
                    summonSupporter();
                } else if (fightingIndex == 5) {
                    moveRole(0, -1);
                } else if (fightingIndex < 17) {
                    approxSleep(1000);
                } else if (fightingIndex == 17) {
                    moveRole(0, 1, 2);
                } else if (fightingIndex < 22) {
                    approxSleep(1000);
                    summonSupporter();
                } else if (fightingIndex == 22) {
                    moveRole(0, 1);
                } else if (fightingIndex < 34) {
                    approxSleep(1000);
                } else {
                    fightingIndex = 0;
                    continue;
                }

                ++fightingIndex;
                break;
            default:
                break;
            }

        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
        }
    }
}
