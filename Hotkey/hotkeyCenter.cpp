#include "HotkeyCenter.h"
#include "Hotkey.h"

#include <QAbstractEventDispatcher>
#include <QHash>

int HotkeyCenter::ref = 0;
QHash<QPair<quint32, quint32>, Hotkey*> HotkeyCenter::hotkeys;

HotkeyCenter::HotkeyCenter(QObject *parent):
    QObject(parent)
{
    if (!ref++)
        QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

HotkeyCenter::~HotkeyCenter()
{
    if (!--ref)
        QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);
}

bool HotkeyCenter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            const quint32 keycode = HIWORD(msg->lParam);
            const quint32 modifiers = LOWORD(msg->lParam);
            activateHotkey(keycode, modifiers);
        }
    }
    else if (eventType == "windows_dispatcher_MSG") {

    }
    return false;
}

bool HotkeyCenter::addHotkey(quint32 nativeKey, quint32 nativeMods, Hotkey *hotkey)
{
    if (hotkeys.find(qMakePair(nativeKey, nativeMods)) != hotkeys.end())
        return false;

    hotkeys.insert(qMakePair(nativeKey, nativeMods), hotkey);

    return true;
}

bool HotkeyCenter::delHotkey(quint32 nativeKey, quint32 nativeMods)
{
    return hotkeys.remove(qMakePair(nativeKey, nativeMods));
}

void HotkeyCenter::activateHotkey(quint32 nativeKey, quint32 nativeMods)
{
    Hotkey* hotkey = hotkeys.value(qMakePair(nativeKey, nativeMods));
    if (hotkey)
        emit hotkey->activated();
}
