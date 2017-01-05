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
    int fightingIndex = 0;

    if (!bind(false))
        return;

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

                    if (sectionIndex == 0) {
                        // Buff
                        buff();
                    } else if (sectionIndex == 4) {
                        // Get close to generator
                        moveRole(0, 1, 2);
                        msleep(300);
                        moveRole(0, 1);
                        moveRole(-1, 0, 3);
                        msleep(600);
                        moveRole(-1, 0);
                    } else if (sectionIndex == 5) {
                        // Avoid damage
                        moveRole(0, 1, 2);
                        approxSleep(2000, 0.3);
                        moveRole(0, 1);
                    }
                } else {
                    flow = FightBoss;
                    fightingIndex = 0;
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
                    sendKey(Stroke, "x", 30);
                    sendKey(Down, m_arrowR, 30);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                    sendKey(Up, m_arrowR, 100);
                } else {
                    sendKey(Up, m_arrowR, 30);
                    sendKey(Stroke, "x", 30);
                    sendKey(Down, m_arrowL, 30);
                    for (int i=0; i<10; ++i)
                        sendKey(Stroke, "x", 30);
                    sendKey(Up, m_arrowL, 100);
                }
                break;
            case Navigate:
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
                            // SHow drop's name
                            hideDropName(false);
                            // Pre-fight
                            flow = PreFight;
                            break;
                        } else {
                            if (end) {
                                qDebug()<<"Navigate error";
                            }
                        }
                        approxSleep(100);
                    }
                }
                break;
            case FightBoss:
                // Check dungeon status
                if (dungeonEnd()) {

                    sectionIndex = 0;
                    flow = PreFight;

                    approxSleep(10000);
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
