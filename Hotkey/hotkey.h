#ifndef HOTKEY_H
#define HOTKEY_H

#include "HotkeyCenter.h"

#include <QObject>


class Hotkey : public QObject
{
    Q_OBJECT
public:
    explicit Hotkey(const QKeySequence& hotkey,QObject *parent = 0);
    ~Hotkey();
signals:
    void activated();
public slots:

private:
    /*static*/ HotkeyCenter hotkeyCenter;
    bool m_bExist;
    quint32 m_nativeKey;
    quint32 m_nativeMods;

    bool setHotkey(const QKeySequence& hotkey);
    bool unsetHotkey();

    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);

    static bool registerHotkey(quint32 m_nativeKey, quint32 m_nativeMods);
    static bool unregisterHotkey(quint32 m_nativeKey, quint32 m_nativeMods);
};

#endif // HOTKEY_H
