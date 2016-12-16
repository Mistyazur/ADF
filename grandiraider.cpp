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
                    sendKey(Stroke, m_arrowL, 30);
                    for (int i=0; i<100; ++i)
                        sendKey(Stroke, "x", 30);
                }

                flow = Fight;
                break;
            case Fight:

                // Check section state
                if (isSectionClear()) {
                    qDebug()<<"Section is clear";
                    flow = Navigate;

                    // Summon tempester
                    summonSupporter();

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
//                            qDebug()<<"navigate"<<x<<y;
                            if (navigate(x, y)) {
//                                qDebug()<<"navigate true";
                                flow = PreFight;
                                break;
                            }
                            mdsleep(500);
                        }
                    }
                } else {
                    flow = FightBoss;
                }

                break;
            case FightBoss:
                qDebug()<<FightBoss;
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
