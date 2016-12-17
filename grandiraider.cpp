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
    JSettings js("../ADF/ADF.json");
    QVariantList pathList = js.value("GrandiPath").toList();
    Flow flow = MoveToDungeon;
    int sectionIndex;

    if (!bind())
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

                // Get close to generator
                if (sectionIndex == 4) {
                    moveRole(-1, 1, 2);
                    msleep(700);
                    stopRole(-1, 1);
                    for (int i=0; i<100; ++i) {
                        sendKey(Stroke, m_arrowL, 30);
                        sendKey(Stroke, "x", 30);
                    }
                }

                if (sectionIndex < pathList.count())
                    flow = Fight;
                else
                    flow = FightBoss;

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
                // Move up
                moveRole(0, -1, 2);
                msleep(3000);
                stopRole(0, -1);
                msleep(12000);

                // Summon
                summonSupporter();
                msleep(100);

                // Move down
                moveRole(0, 1, 2);
                msleep(3000);
                stopRole(0, 1);
                msleep(12000);

                // Summon
                summonSupporter();
                msleep(100);
                break;
            default:
                break;
            }

            msleep(10);
        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
        }
    }
}
