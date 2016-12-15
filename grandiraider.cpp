#include "grandiraider.h"

#include "JSettings/jsettings.h"

#include <QDebug>
#include <QString>

QString pathJson = "{"
        "GrandiPath : ["
            "[(-1, 600), (460, -1), (-1, 0), (650, -1), (-1, 600)]"
        "]"
"}";

GrandiRaider::GrandiRaider()
{

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
                flow = Navigate;
                break;
            case Fight:

                break;
            case Navigate:
                if (sectionIndex < pathList.count()) {
                    QVariant sectionPath = pathList.at(sectionIndex++);
                    foreach(QVariant positionList, sectionPath.toList()) {
                        QVariantList position = positionList.toList();
                        if (position.count() == 2) {
                            int x = position.first().toInt();
                            int y = position.last().toInt();
                            qDebug()<<x<<y;
                            if (navigate(x, y))
                                break;
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

        } catch(DFError e) {

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
