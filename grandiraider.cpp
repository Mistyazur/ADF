#include "grandiraider.h"

#include "JSettings/jsettings.h"

#include <QDebug>
#include <QApplication>
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
    JSettings js(QApplication::applicationDirPath()+"/ADF.json");
    QVariantList pathList = js.value("GrandiPath").toList();
    Flow flow = MoveToDungeon;
    int sectionIndex = 0;
    int fightBossIndex = 0;

    if (!bind(false))
        return;

    while (true) {
        try {
//            qDebug()<<"Flow: "<<flow<<sectionIndex;
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

                    if (sectionIndex == 0) {
                        // Buff
                        buff();
                    } else if (sectionIndex == 4) {
                        // Get close to generator
                        moveRole(0, 1, 2);
                        msleep(500);
                        stopRole(0, 1);
                        moveRole(-1, 0, 3);
                        msleep(600);
                        stopRole(-1, 0);
                    } else if (sectionIndex == 5) {
                        // Avoid damage
                        moveRole(0, 1, 2);
                        approxSleep(2000, 0.3);
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
                } else {
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
                            approxSleep(100);
                        }
                    }

                    if (!success)
                        qDebug()<<"Failed to find gate";
                }

                break;
            case FightBoss:

                // Check dungeon status
                if (reenterDungeon()) {
                    if ((fightBossIndex < 5) &&
                            (fightBossIndex > 0)) {
                        stopRole(0, -1);
                    } else  if ((fightBossIndex < 22) &&
                            (fightBossIndex > 17)) {
                        stopRole(0, 1);
                    }

                    sectionIndex = 0;
                    flow = PreFight;

                    approxSleep(10000, 0.3);
                    continue;
                }

                if (fightBossIndex == 0) {
                    moveRole(0, -1, 2);
                } else if (fightBossIndex < 5) {
                    approxSleep(1000);
                    summonSupporter();
                } else if (fightBossIndex == 5) {
                    stopRole(0, -1);
                } else if (fightBossIndex < 17) {
                    approxSleep(1000);
                } else if (fightBossIndex == 17) {
                    moveRole(0, 1, 2);
                } else if (fightBossIndex < 22) {
                    approxSleep(1000);
                    summonSupporter();
                } else if (fightBossIndex == 22) {
                    stopRole(0, 1);
                } else if (fightBossIndex < 34) {
                    approxSleep(1000);
                } else {
                    fightBossIndex = 0;
                    continue;
                }

                ++fightBossIndex;
                break;
            default:
                break;
            }

        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
        }
    }
}
