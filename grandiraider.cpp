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
                    flow == FightBoss;

                break;
            case Fight:

                // Check section state
                if (isSectionClear((sectionIndex == 0))) {
                    qDebug()<<"Section is clear";
                    flow = Navigate;

                    // Summon tempester
                    summonSupporter();

                    // Hide drop's name
                    hideDropName(true);

                    break;
                }


                break;
            case Navigate:
                if (sectionIndex < pathList.count()) {
                    QVariant sectionPath = pathList.at(sectionIndex++);
                    foreach(QVariant positionList, sectionPath.toList()) {
                        QVariantList position = positionList.toList();
                        if (position.count() == 2) {
                            int x = position.first().toInt();
                            int y = position.last().toInt();
                            if (navigate(x, y)) {
                                flow = PreFight;

                                // SHow drop's name
                                hideDropName(false);

                                break;
                            }
                            mdsleep(500);
                        }
                    }
                }

                break;
            case FightBoss:
                qDebug()<<FightBoss;
                // Move up
                moveRole(0, -1, 2);
                mdsleep(15000);
                stopRole(0, -1);
                mdsleep(100);

                // Summon
                summonSupporter();
                mdsleep(100);

                // Move down
                moveRole(0, 1, 2);
                mdsleep(15000);
                stopRole(0, 1);
                mdsleep(100);

                // Summon
                summonSupporter();
                mdsleep(100);
                break;
            default:
                break;
            }

            msleep(10);
        } catch(DFError e) {
            qDebug()<<"DFError"<<e;
        }
    }



//    for (int i=0; i<pathList.count(); ++i) {
//        QVariant sectionPath = pathList.at(i);
//        foreach(QVariant positionList, sectionPath.toList()) {
//            QVariantList position = positionList.toList();
//            if (position.count() == 2) {
//                int x = position.first().toInt();
//                int y = position.last().toInt();
//                qDebug()<<x<<y;
//                if (navigate(x, y))
//                    break;
//                mdsleep(500);
//            }
//        }
//        qDebug()<<"---";
//        msleep(2000);
//    }


}
