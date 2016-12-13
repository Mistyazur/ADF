#ifndef CHOTKEYEVENTFILTER_HPP
#define CHOTKEYEVENTFILTER_HPP

#include <Windows.h>

#include <QAbstractNativeEventFilter>
#include <QKeySequence>
#include <QObject>
#include <QHash>
#include <qwindowdefs.h>

class Hotkey;
class HotkeyCenter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit HotkeyCenter(QObject *parent = 0);
    ~HotkeyCenter();

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);

    static bool addHotkey(quint32 nativeKey, quint32 nativeMods,Hotkey *hotkey);
    static bool delHotkey(quint32 nativeKey, quint32 nativeMods);
signals:
    void activated();

public slots:

protected:

private:
    static int ref;
    static QHash<QPair<quint32, quint32>, Hotkey*> hotkeys;

    void activateHotkey(quint32 nativeKey, quint32 nativeMods);
};

#endif // CHOTKEYEVENTFILTER_HPP
