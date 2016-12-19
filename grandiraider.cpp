#include "grandiraider.h"

#include "JSettings/jsettings.h"

#include <QDebug>
#include <QString>

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
    JSettings js("ADF.json");
    QVariantList pathList = js.value("GrandiPath").toList();
    Flow flow = MoveToDungeon;
    int sectionIndex;
    int fightBossIndex;

    if (!bind())
        return;

//    while (true) {
//        qDebug()<<isSectionClear(false);
//        msleep(1000);
//    }
//    unbind();
//    return;

    while (true) {
        try {
            switch (flow) {
            case MoveToDungeon:

                sectionIndex = 0;
                flow = PreFight;
                break;
            case PreFight:
                // Summon tempester
                summonSupporter();

                if (sectionIndex < pathList.count()) {
                    flow = Fight;

                    if (sectionIndex == 4) {
                        // Get close to generator
                        moveRole(0, 1, 2);
                        msleep(300);
                        stopRole(0, 1);
                        moveRole(-1, 0, 3);
                        msleep(500);
                        stopRole(-1, 0);
                    } else if (sectionIndex == 5) {
                        // Avoid damage
                        moveRole(0, 1, 2);
                        msleep(1000);
                        stopRole(0, 1);
                    }
                } else {
                    flow = FightBoss;
                    fightBossIndex = 0;
                }
                break;
            case Fight:
                // Check section state
                if (isSectionClear((sectionIndex == 0))) {
                    qDebug()<<"Section is clear";

                    // Hide drop's name
                    hideDropName(true);

                    flow = Navigate;
                    break;
                }

                // Destory generator
                if (sectionIndex == 4) {
                    sendKey(Stroke, m_arrowL, 30);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                }
                break;
            case Navigate:
                if (sectionIndex < pathList.count()) {
                    bool success = false;
                    const QVariant &sectionPath = pathList.at(sectionIndex++);
                    foreach(const QVariant &positionList, sectionPath.toList()) {
                        QVariantList position = positionList.toList();
                        if (position.count() == 2) {
                            if (navigate(position.first().toInt(), position.last().toInt())) {
                                // SHow drop's name
                                hideDropName(false);

                                // Pre-fight
                                success = true;
                                flow = PreFight;
                                break;
                            }
                            mdsleep(100);
                        }
                    }

                    if (!success)
                        qDebug()<<"Failed to find gate";
                }

                break;
            case FightBoss:
                // Check dungeon status
                if (reenterDungeon()) {
                    sectionIndex = 0;
                    flow = PreFight;
                    msleep(10000);
                    continue;
                }

                // Move
                if (fightBossIndex%4 == 0) {
                    // Up
                   moveRole(0, -1, 2);
                   mdsleep(2000);
                   stopRole(0, -1);
                } else if (fightBossIndex%4 == 1) {
                    // Right
                   moveRole(1, 0, 2);
                   mdsleep(2000);
                   stopRole(1, 0);
                } else if (fightBossIndex%4 == 2) {
                    // Down
                   moveRole(0, 1, 2);
                   mdsleep(2000);
                   stopRole(0, 1);
                } else if (fightBossIndex%4 == 3) {
                    // Left
                   moveRole(-1, 0, 2);
                   mdsleep(2000);
                   stopRole(-1, 0);
                }
                ++fightBossIndex;

                // Summon
                summonSupporter();
                break;
            default:
                break;
            }

//            msleep(10);
        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
        }
    }
}
