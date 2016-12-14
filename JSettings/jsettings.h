#ifndef JSETTINGS_H
#define JSETTINGS_H

#include <QSettings>
#include <QIODevice>
#include <QJsonDocument>


static bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    bool ret = false;
    QJsonParseError error;

    map = QJsonDocument::fromJson(device.readAll() , &error).toVariant().toMap();
    if(error.error == QJsonParseError::NoError)
        ret = true;

    return ret;
}

static bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    bool ret = false;

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(QVariant::fromValue(map));
    if (device.write(jsonDocument.toJson()) != -1)
        ret = true;

    return ret;
}

const QSettings::Format JsonFormat = QSettings::registerFormat("json", readJsonFile, writeJsonFile);

class JSettings : public QSettings
{
public:
    JSettings(Scope scope, const QString &organization, const QString &application = QString(), QObject *parent = Q_NULLPTR) :
        QSettings(JsonFormat, scope, organization, application, parent)
    {

    }

    JSettings(const QString &fileName, QObject *parent = Q_NULLPTR) :
        QSettings(fileName, JsonFormat, parent)
    {

    }
};

#endif // JSETTINGS_H
