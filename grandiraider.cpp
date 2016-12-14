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

void GrandiRaider::run()
{
    JSettings js("ADF.json");
    QVariantList pathList = js.value("GrandiPath").toList();

//    for (int i=0; i<pathList.count(); ++i) {
//        QVariant sectionPath = pathList.at(i);
//        foreach(QVariant positionList, sectionPath.toList()) {
//            QVariantList position = positionList.toList();
//            if (position.count() == 2) {
//                int x = position.first().toInt();
//                int y = position.last().toInt();
//                qDebug()<<x<<y;
//            }
//        }
//        qDebug()<<"---";
//    }


    if (!bind())
        return;

    for (int i=0; i<pathList.count(); ++i) {
        QVariant sectionPath = pathList.at(i);
        foreach(QVariant positionList, sectionPath.toList()) {
            QVariantList position = positionList.toList();
            if (position.count() == 2) {
                int x = position.first().toInt();
                int y = position.last().toInt();
                qDebug()<<x<<y;
                navigate(x, y);
                mdsleep(500);
            }
        }
        qDebug()<<"---";
        msleep(3000);
    }

//    navigate(-1, 600);
//    navigate(460, -1);
//    navigate(-1, 0);
//    navigate(650, -1);
//    navigate(-1, 600);

    unbind();
}
