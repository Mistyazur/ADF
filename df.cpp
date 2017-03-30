#include "df.h"

#include "JSettings/jsettings.h"

#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QProcess>
#include <QTime>
#include <QDebug>

#include <Windows.h>

#undef FindWindow

#define PICK_ROLE_POS 380, 450


DF::DF()
{
    m_hBindWnd = 0;
    m_roleOffsetY = 150;

    // Default arrow keys
    m_arrowL = 37;
    m_arrowU = 38;
    m_arrowR = 39;
    m_arrowD = 40;

    // Set mouse and key duration
    setMouseDuration(30);
    setKeyDuration(30);
    setMouseDurationDelta(0.2);
    setKeyDurationDelta(0.2);

    // Set mouse and key delay
    setMouseDelay(30);
    setKeyDelay(30);
    setMouseDelayDelta(0.2);
    setKeyDelayDelta(0.2);
}

DF::~DF()
{
}

bool DF::startTGP()
{
    QSettings settings("Mistyazur", QApplication::applicationName());

    QString path = settings.value("tgp/path", "").toString();
    if (path.isEmpty())
        return false;

   if (!QFileInfo(path).exists())
        return false;

   return QProcess::startDetached("\"" + path + "\"");
}

HWND DF::getTGPWindow()
{
    HWND hTGPWnd;

    QString res = m_dm.EnumWindow(0, "Chrome Legacy Window", "Chrome_RenderWidgetHostHWND", 1+2+16);
    QStringList winList = res.split(",", QString::SkipEmptyParts);
    if (winList.size() < 1)
        hTGPWnd = (HWND)0;
    else
        hTGPWnd = GetAncestor((HWND)winList.first().toInt(), GA_ROOTOWNER);

    return hTGPWnd;
}

bool DF::startClient()
{
    bool ok = false;

    HWND hTGPWnd = getTGPWindow();
    if (hTGPWnd == 0) {
        startTGP();

        ok = false;
        for (int i = 0; i < 90; ++i) {
            hTGPWnd = getTGPWindow();
            if (hTGPWnd != 0) {
                ok = true;
                break;
            }
            approxSleep(1000);
        }
        if (!ok) {
            qDebug()<<"Start Client: Can't start TGP";
            return false;
        }
    }

    // Activate tgp
    if (!activateWindow(hTGPWnd)) {
        qDebug()<<"Start Client: Can't activate tgp window";
        return false;
    }
    msleep(1000);

    // Bind tgp
    m_dm.SetWindowSize((int)hTGPWnd, 1020, 720);
    m_dm.BindWindow((int)hTGPWnd, "normal", "normal", "normal", 0);

    // Start client
    sendMouse(Left, 50, 300, 3000);
    sendMouse(Left, 900, 680, 1000);

    // Skip checking window
    QVariant vx, vy;
    for (int i = 0; i < 5; ++i) {
        if (m_dm.FindPic(0, 0, 1020, 720, "tgp_dnf_skip_check.bmp", "101010", 1.0, 0, vx, vy) != -1)
            sendMouse(Left, vx.toInt() + 25, vy.toInt() + 6, 1000);
        msleep(1000);
    }

    // Unbind tgp
    m_dm.UnBindWindow();

    // Wait for client
    for (int i = 0; i < 180; ++i) {
        if (window() != 0) {
            activateWindow((HWND)window());
            return true;
        }
        msleep(1000);
    }

    qDebug()<<"Start Client: Failed waiting for client";

    return false;
}

bool DF::closeClient()
{
//    int hTGPWnd;

//    qDebug()<<"Close client";

//    hTGPWnd = m_dm.FindWindow("TWINCONTROL", "腾讯游戏平台");
//    if (hTGPWnd == 0) {
//        qDebug()<<"Start Client: Can't find tgp window";
//        return false;
//    }

//    // Activate tgp
//    if (m_dm.GetForegroundWindow() != hTGPWnd) {
//        activateWindow((HWND)hTGPWnd);
//        msleep(1000);
//    }

//    // wait for tgp activated
//    bool ok = false;
//    for (int i = 0; i < 10; ++i) {
//        if (m_dm.GetForegroundWindow() == hTGPWnd) {
//            ok = true;
//            break;
//        }
//        msleep(1000);
//    }
//    if (!ok) {
//        qDebug()<<"Start Client: Can't activate tgp window";
//        return false;
//    }

//    // Bind tgp
//    m_dm.SetWindowSize(hTGPWnd, 1020, 720);
//    m_dm.BindWindow(hTGPWnd, "normal", "normal", "normal", 0);

//    // Close client
//    QVariant vx, vy;
//    if (m_dm.FindPic(0, 0, 1020, 720, "tgp_terminate_game.bmp", "101010", 1.0, 0, vx, vy) != -1) {
//        sendMouse(Left, vx.toInt() + 10, vy.toInt() + 10, 2000);
//        if (m_dm.FindPic(0, 0, 1020, 720, "tgp_terminate_game_confirm.bmp", "101010", 1.0, 0, vx, vy) != -1)
//            sendMouse(Left, vx.toInt(), vy.toInt());
//    }

//    // Unbind tgp
//    m_dm.UnBindWindow();

//    msleep(2000);


    QProcess::startDetached("TASKKILL /IM DNF.exe /F /T");
    msleep(2000);
    QProcess::startDetached("TASKKILL /IM Client.exe /IM Repair.exe /F /T");
    msleep(3000);

    return false;
}

int DF::window()
{
    return m_dm.FindWindow("地下城与勇士", "地下城与勇士");
}

bool DF::bind(bool underground)
{
    bool ret = false;

    m_hBindWnd = window();
    if (0 == m_hBindWnd)
        return ret;

    if (underground)
        ret = m_dm.BindWindowEx(m_hBindWnd,
                              "dx2",
                              "dx.mouse.position.lock.api|dx.mouse.position.lock.message|dx.mouse.clip.lock.api|dx.mouse.input.lock.api|dx.mouse.state.api|dx.mouse.api|dx.mouse.cursor",
                              "dx.keypad.input.lock.api|dx.keypad.state.api|dx.keypad.api",
                              "dx.public.graphic.protect|dx.public.km.protect",
                              103);
    else {
//        SetWindowPos((HWND)m_hBindWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        ret = m_dm.BindWindow(m_hBindWnd, "normal", "normal", "normal", 0);
    }

    approxSleep(2000);
    return ret;
}

void DF::unbind()
{
//    SetWindowPos((HWND)m_hBindWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    m_dm.UnBindWindow();
}

bool DF::waitForRoleList()
{
    QVariant vx, vy;

    for (int i = 0; i < 60; ++i) {
        if (m_dm.FindPic(500, 530, 600, 560, "terminate_game.bmp", "000000", 1.0, 0, vx, vy) != -1) {
            return true;
        }

        approxSleep(1000);
    }

    return false;
}

bool DF::openSystemMenu()
{
    QVariant vx, vy;

    for (int i = 0; i < 50; ++i) {
        if (m_dm.FindPic(520, 120, 580, 140, "system_menu.bmp", "000000", 1.0, 0, vx, vy) == -1) {
            sendKey(Sk, 27, 200);
        } else {
            return true;
        }
    }

    return false;
}

bool DF::closeSystemMenu()
{
    QVariant vx, vy;

    for (int i = 0; i < 50; ++i) {
        if (m_dm.FindPic(520, 120, 580, 140, "system_menu.bmp", "000000", 1.0, 0, vx, vy) == -1) {
            return true;
        } else {
            sendKey(Sk, 27, 200);
        }
    }

    return false;
}

bool DF::isDisconnected()
{
    QVariant vx, vy;

    if (m_dm.FindPic(300, 200, 500, 400, "disconnected.bmp", "101010", 1.0, 0, vx, vy) == -1) {
        return false;
    }

    return true;
}

void DF::setArrowKey(int left, int up, int right, int down)
{
    m_arrowL = left;
    m_arrowU = up;
    m_arrowR = right;
    m_arrowD = down;
}

void DF::pickRole(int index)
{
    const int originalX = 128;
    const int originalY = 190;
    const int offsetX = 120;
    const int offsetY = 210;
    int row = index%4;
    int column = index/4;

    // Wait for role list
    if (!waitForRoleList()) {
        qDebug()<<"Failed to wait for role list";
        throw DFRESTART;
    }

    // Reset scroll bar
    for (int i = 0; i < 30; ++i)
        sendMouse(Left, 580, 90);

    int oldMouseDuration = setMouseDuration(200);

    // Scroll
    if (column > 1) {
        for (int i=0; i < column-1; ++i)
            sendMouse(Left, 580, 495, 1000);
        column = 1;
    }

    // Pick role
    int roleX = originalX+offsetX*row;
    int roleY = originalY+offsetY*column;
    for (int i = 0; i < 2; ++i) {
        sendMouse(Left, roleX, roleY, 100);
        sendMouse(Left, roleX, roleY, 500);
    }

    // Game start
    sendKey(Sk, 32, 3000);

    setMouseDuration(oldMouseDuration);

    // Use system menu to make sure role is switched successfully
    // It'll also close ohter unknown window
    if (!openSystemMenu()) {
        qDebug()<<"Pick role: Failed to open system menu";
        throw DFRESTART;
    }

    // Close system menu
    if (!closeSystemMenu()) {
        qDebug()<<"Pick role: Failed to close system menu";
        throw DFRESTART;
    }
}

void DF::repickCurrentRole()
{
    backToRoleList();
    approxSleep(1000);
    sendKey(Sk, 32, 1000);
    sendKey(Sk, 32, 1000);

    // Use system menu to make sure role is switched successfully
    // It'll also close ohter unknown window
    if (!openSystemMenu()) {
        qDebug()<<"Pick role: Failed to open system menu";
        throw DFRESTART;
    }

    // Close system menu
    if (!closeSystemMenu()) {
        qDebug()<<"Pick role: Failed to close system menu";
        throw DFRESTART;
    }
}

void DF::backToRoleList()
{
    if (!openSystemMenu()) {
        qDebug()<<"Back to role list: Failed to open system menu";
        throw DFRESTART;
    }

    approxSleep(1000);
    sendMouse(Left, PICK_ROLE_POS, 1000);

    // Wait for role list
    if (!waitForRoleList()) {
        qDebug()<<"Back to role list: Failed to wait for role list";
        throw DFRESTART;
    }
}

void DF::teleport(const QString &destination)
{
    QVariant x, y;

    // Check current location
    if (m_dm.FindPic(CLIENT_RECT, destination, "000000", 1.0, 0, x, y) != -1)
        return;

    // Teleport
}

void DF::navigateOnMap(int x, int y, int time)
{
    sendKey(Sk, "N", 1000);
    sendMouse(Right, x, y, 200);
    sendMouse(Right, x, y, 200);
    sendKey(Sk, "N", time);
}

void DF::sellEquipment()
{
    QVariant vx, vy;
    int sellX, sellY;
    int ox, oy;
    int x, y;
    bool found;

    // Reset mouse because it may be cover something I'll search for
    sendMouse(Move, 0, 0);

    // Wait for selling
    found = false;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(0, 300, 400, 600, "sell.bmp", "000000", 1.0, 1, vx, vy) != -1) {
            sellX = vx.toInt();
            sellY = vy.toInt();
            found = true;
            break;
        }
        msleep(500);
    }
    if (!found)
        return;

    // Search sort button
    found = false;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(0, 300, 800, 600, "sort.bmp", "000000", 1.0, 1, vx, vy) != -1) {
            found = true;
            break;
        }
        msleep(500);
    }
    if (!found)
        return;

    // Click equipment button
    sendMouse(Left, vx.toInt() - 190, vy.toInt() - 240, 100);
    sendMouse(Left, vx.toInt() - 190, vy.toInt() - 240, 100);

    // Click sort button
    sendMouse(Left, vx.toInt(), vy.toInt(), 100);

    // Select item
    ox = vx.toInt() - 190 + 4;
    oy = vy.toInt() - 240 + 36;
    for (int i = 0; i < 32; ++i) {
        x = ox + ((i % 8) * 30);
        y = oy + ((i / 8) * 30);

        // Check empty
        if (m_dm.GetColor(x, y - 14) == "000000") {
            break;
        }

        // Sell
        sendMouse(Move, x, y, 100);
        if (m_dm.FindPic(0, 0, 800, 400, "item_unique.bmp|item_legendary.bmp|item_epic.bmp", "000000", 1.0, 0, vx, vy) == -1) {
            // Click sell button
            sendMouse(Left, sellX, sellY, 50);

            // Select and confirm
            for (int i = 0; i < 4; ++i)
                sendMouse(Left, x, y, 50);
        }
    }
}

void DF::buyMaterials()
{
    QVariant vx, vy;

    for (int i = 0; i < 4; ++i) {
        if (m_dm.FindPic(10, 410, 300, 500, "crystal.bmp|abyss_ticket.bmp", "101010", 1.0, 1, vx, vy) != -1) {
            // Cancel sell mode
            sendMouse(Right, -1, -1, 300);

            // Buy all
            sendKey(Dn, 16, 300);
            sendMouse(Left, vx, vy, 100);
            sendKey(Up, 16, 300);

            // Confirm
            sendKey(Sk, 13, 200);
            sendKey(Sk, 13, 200);
        }
    }
}

void DF::checkMail()
{
    QVariant vx, vy;

    int oldMouseDuration = setMouseDuration(200);

    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(260, 480, 500, 560, "mail.bmp", "101010", 1.0, 1, vx, vy) != -1) {
            sendMouse(Left, vx.toInt() + 9, vy.toInt() + 5, 1000);  // Open mail box
            sendMouse(Left, 300, 465, 4000);  // Receive all mails
            openSystemMenu();
            closeSystemMenu();
        }
        msleep(100);
    }

    setMouseDuration(oldMouseDuration);

    approxSleep(1000);
}

void DF::updateShareStorage()
{
    QVariant vx, vy;

    int oldMouseDuration = setMouseDuration(200);
    int oldKeyDuration = setKeyDuration(200);

    sendMouse(Left, 270, 370, 3000);  // Click storage
    sendMouse(Left, 470, 360, 1000);  // Confirm
    sendMouse(Left, 300, 128, 1000);  // Switch to share storage

    // Save money
    if (m_dm.FindPic(CLIENT_RECT, "save_gold.bmp", "000000", 1.0, 0, vx, vy) != -1) {
        sendMouse(Left, vx, vy, 500);
        sendMouse(Left, vx, vy, 500);
    }

    // Save items
    sendKey(Sk, "A", 100);
    sendKey(Sk, 13, 1000);

    setMouseDuration(oldMouseDuration);
    setKeyDuration(oldKeyDuration);

    openSystemMenu();
    closeSystemMenu();
}

void DF::cancelCrystalContract()
{
    // Open packet
    sendKey(Sk, "i", 500);

    // Switch to material
    sendMouse(Left, 590, 250, 200);

    // Cancel contract
    sendMouse(Left, 745, 470, 100);
    sendMouse(Left, 745, 470, 100);

    // Close packet
    sendKey(Sk, "i", 500);
}

void DF::playMercenary()
{
    if (!openSystemMenu())
        return;
        
    // Click adventure party button
    sendMouse(Left, 490, 180, 200);
    
    // Click mercenary button
    sendMouse(Left, 375, 135, 100);
    sendMouse(Left, 375, 135, 200);
    
    // Play
    QVariant vx, vy;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(200, 100, 400, 500, "combo_box_button.bmp", "000000", 1.0, 0, vx, vy) != -1) {
            // Pick time
            sendMouse(Left, vx.toInt(), vy.toInt(), 100);
            sendMouse(Left, vx.toInt() - 30, vy.toInt() + 70, 100);

            // Pick map
            sendMouse(Left, vx.toInt() + 180, vy.toInt(), 100);
            sendMouse(Left, vx.toInt() + 180 - 30, vy.toInt() + 55, 500);

            // Confirm
            sendMouse(Left, vx.toInt() + 220, vy.toInt(), 500);
        }
        approxSleep(100);
    }

    openSystemMenu();
    closeSystemMenu();
}

bool DF::initDungeonSettings(const QString &dungeon)
{
    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    settings.beginGroup(dungeon);

    // Get role index
    QString index = settings.value("role_index", "").toString();
    QStringList indexList = index.split("-", QString::SkipEmptyParts);
    if (indexList.size() != 2)
        return false;

    m_firstRoleIndex = indexList.first().toInt() - 1;
    m_lastRoleIndex = indexList.last().toInt() - 1;
    if ((m_firstRoleIndex < 0) || (m_lastRoleIndex < 0) || (m_firstRoleIndex > m_lastRoleIndex))
        return false;

    // Get dungeon settings
    QString jsonFile = settings.value("json", "").toString();

    settings.endGroup();

    // Initialize dungeon settings
    if (jsonFile.isEmpty()) {
        qDebug()<<dungeon<<"Json file not exist";
        return false;
    }

    JSettings js(QApplication::applicationDirPath() + "/" + jsonFile);
    m_pathList = js.value("Path").toList();
    m_nodeList = js.value("Nodes").toList();
    if (m_pathList.isEmpty() || m_nodeList.isEmpty()) {
        qDebug()<<dungeon<<"Path or node is empty";
        return false;
    }

    QVariantList mapRect = js.value("MapRect").toList();
    if (mapRect.count() < 4) {
        qDebug()<<dungeon<<"Map rect undefined";
        return false;
    }

    m_dungeonMapX1 = mapRect.at(0).toInt();
    m_dungeonMapY1 = mapRect.at(1).toInt();
    m_dungeonMapX2 = mapRect.at(2).toInt();
    m_dungeonMapY2 = mapRect.at(3).toInt();

    return true;
}

bool DF::resetRoleCount(const QString &dungeon)
{
    bool reseted = false;

    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    settings.beginGroup(dungeon);

    QDateTime lastDateTime = settings.value("last_time").toDateTime();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QDateTime thresholdDateTime;
    thresholdDateTime.setDate(lastDateTime.date());
    thresholdDateTime.setTime(QTime(6, 0, 0));
    if (lastDateTime.time().hour() >= 6) {
        thresholdDateTime = thresholdDateTime.addDays(1);
    }

    if (currentDateTime >= thresholdDateTime) {
        settings.setValue("role_count", 0);
        m_roleCount = 0;
        reseted = true;
    } else {
        // Get count of role that'll be automated
        m_roleCount = settings.value("role_count", 0).toInt();
    }

    settings.setValue("last_time", currentDateTime);

    settings.endGroup();
    settings.sync();

    return reseted;
}

bool DF::updateRoleCount(const QString &dungeon)
{
    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    ++m_roleCount;

    if (m_firstRoleIndex + m_roleCount > m_lastRoleIndex) {
        return false;
    }

    settings.beginGroup(dungeon);
    settings.setValue("role_count", m_roleCount);
    settings.endGroup();

    return true;
}

void DF::pickRole()
{
    pickRole(m_firstRoleIndex + m_roleCount);
}

bool DF::initRoleOffset()
{
    QVariant vx, vy;

    bool ok = false;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindMultiColor(0, 100, 800, 400,
                                 "FF00FF-001000", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-001000",
                                 1.0, 0,
                                 vx, vy)) {
            ok = true;
            break;
        }
        msleep(1000);
    }
    if (!ok) {
        qDebug()<<"Initialize role offset failed";
        return false;
    }

    m_roleOffsetY = 430 - vy.toInt();

    return true;
}

bool DF::isBlackScreen()
{
    QVariant vx, vy;

    if (m_dm.FindPic(350, 250, 450, 350, "black_screen.bmp", "202020", 1.0, 0, vx, vy) == -1)
        return false;

    return true;
}

bool DF::enterDungeon(int index, int difficulty, bool leftEntrance)
{
    QVariant vx, vy;

    int directionKey = leftEntrance ? m_arrowL : m_arrowR;

    sendKey(Dn, directionKey, 3000);
    sendKey(Up, directionKey, 100);

    // Cancel mercenary
    if (m_dm.FindPic(CLIENT_RECT, "announcement.bmp", "000000", 1.0, 0, vx, vy) != -1) {
        sendMouse(Left, vx.toInt() - 20, vy.toInt() + 100, 100);
        sendMouse(Left, vx.toInt() - 20, vy.toInt() + 100, 100);
        openSystemMenu();
        closeSystemMenu();

        sendKey(Dn, leftEntrance ? m_arrowR : m_arrowL, 500);
        sendKey(Up, leftEntrance ? m_arrowR : m_arrowL, 100);
        sendKey(Dn, directionKey, 1000);
        sendKey(Up, directionKey, 100);
    }

    // Wait for dungeon picking UI
    for (int i=0; i<10; ++i) {
        if (m_dm.FindPic(CLIENT_RECT, "options_back_to_town.bmp", "000000", 1.0, 0, vx, vy) != -1) {
            goto EnterDungeon;
        }
        msleep(1000);
    }

    return false;

EnterDungeon:

    int oldKeyDuration = setKeyDuration(200);

    // Pick dungeon
    for (int i=0; i<index; ++i)
        sendKey(Sk, m_arrowU, 100);

    // Pick difficulty
    for (int i=0; i<4; ++i)
        sendKey(Sk, m_arrowL);
    for (int i=0; i<difficulty; ++i)
        sendKey(Sk, m_arrowR, 100);

    // Commit
    sendKey(Sk, 32, 500);

    setKeyDuration(oldKeyDuration);

    // Wait
    return waitForDungeonBeign();
}

bool DF::reenterDungeon()
{
    // Reenter
    sendKey(Sk, 121, 500);
    sendKey(Sk, 121, 500);

    // Wait
    return waitForDungeonBeign();
}

bool DF::waitForDungeonBeign()
{
    QVariant vx, vy;

    for (int i = 0; i < 50; ++i) {
        if (m_dm.FindPic(m_dungeonMapX1, m_dungeonMapY1, m_dungeonMapX2, m_dungeonMapY2,
                         "minimap_role.bmp", "000020", 1.0, 0, vx, vy) != -1) {
            return true;
        }
        msleep(200);
    }

    return false;
}

bool DF::isRoleDead()
{
    QVariant vx, vy;

    if (m_dm.FindPic(330, 420, 360, 450, "dead.bmp", "101010", 1.0, 0, vx, vy) == -1)
        return false;

    return true;
}

bool DF::isDungeonEnded()
{
    QVariant vx, vy;

    if (m_dm.FindPic(m_dungeonMapX1, m_dungeonMapY1, m_dungeonMapX2, m_dungeonMapY2,
                     "minimap_boss.bmp", "000020", 1.0, 0, vx, vy) != -1)
        return false;

    return true;
}

bool DF::isNoDungeonPoint()
{
    QVariant vx, vy;

    sendMouse(Move, 400, 554, 500);
    if (m_dm.FindPic(200, 450, 500, 550, "no_dungeon_point.bmp", "000000", 1.0, 1, vx, vy) != -1)
        return true;

    return false;
}

void DF::pickFreeGoldenCard()
{
    QVariant vx, vy;

    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(0, 300, 400, 600, "sell.bmp", "000000", 1.0, 1, vx, vy) != -1)
            return;

        if (m_dm.FindPic(0, 300, 400, 600, "free_golden_card.bmp", "101010", 1.0, 1, vx, vy) != -1) {
            sendKey(Sk, 53, 100);
            break;
        }

        approxSleep(500);
    }
}

void DF::summonSupporter()
{
    // Tab to summon
    sendKey(Sk, 9, 100);
}

void DF::useOwnSkill()
{
    sendKey(Sk, "z", 100);
}

void DF::buff()
{
    sendKey(Sk, m_arrowU);
    sendKey(Sk, m_arrowD);
    sendKey(Sk, 32, 2000);

    sendKey(Sk, m_arrowD);
    sendKey(Sk, m_arrowU);
    sendKey(Dn, 32, 2000);
    sendKey(Up, 32);
}

int DF::getSectionIndex()
{
    int x, y;
    QVariant vx, vy;

    if (!getRoleCoordsInMap(x, y)) {
        return -1;
    }

    if ((x == -1) && (y == -1)) {
        // Boss room
        return -2;
    }

    return m_nodeList.indexOf(QVariantList({x , y}));
}

bool DF::isSectionClear()
{
    QVariant vx, vy;

    for (int i = 0; i < 4; ++i) {
        if (m_dm.FindPic(m_dungeonMapX1, m_dungeonMapY1, m_dungeonMapX2, m_dungeonMapY2,
                         "section_clear.bmp", "000000", 1.0, 0, vx, vy) != -1)
            return true;
        msleep(50);
    }

    return false;
}

bool DF::isPickable()
{
    QVariant vx, vy;

    if (m_dm.FindPic(0, 0, 800, 500, "drop_activated_left.bmp|drop_activated_right.bmp", "000000", 1.0, 0, vx, vy) != -1)
        return true;

    return false;
}

bool DF::getTrophyCoords(int x, int y, int &nx, int &ny, bool &pickable)
{
    QString res;
    QVariant vx, vy;

    pickable = false;

    res = m_dm.FindPicEx(0, 0, 800, 500, "drop_activated_left.bmp|drop_normal_left.bmp|drop_event_left.bmp", "000000", 1.0, 1);
    if (res.isEmpty())
        return false;

    if (res.startsWith("0")) {
        pickable = true;
        return true;
    }

    res = m_dm.FindNearestPos(res, 0, x, y);
    QStringList resList = res.split(",", QString::SkipEmptyParts);
    if (resList.size() != 3)
        return false;

    int index = resList.at(0).toInt();
    int tx = resList.at(1).toInt();
    int ty = resList.at(2).toInt();

    if (m_dm.FindPic(tx, ty, tx + 200, ty + 6,
                     (index == 1) ? "drop_normal_right.bmp" : "drop_event_left.bmp",
                     "000000", 1.0, 0, vx, vy) != -1) {
        nx = tx + (vx.toInt() + 10 - tx) / 2;
        ny = ty + 30;
    } else {
        nx = tx + 50;
        ny = ty + 30;
    }

    return true;
}

bool DF::getRoleCoordsInMap(int &x, int &y)
{
    QVariant vx, vy;

    int index = m_dm.FindPic(m_dungeonMapX1, m_dungeonMapY1, m_dungeonMapX2, m_dungeonMapY2,
                            "minimap_role.bmp|minimap_boss.bmp", "000000", 1.0, 0, vx, vy);
    if (index == -1) {
        return false;
    } else if (index == 0) {
        x = vx.toInt();
        y = vy.toInt();
    } else if (index == 1) {
        x = -1;
        y = -1;
    }

    return true;
}

bool DF::getRoleCoords(int &x, int &y)
{
    QVariant vx, vy;

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FF00FF-001000", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-001000",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt();
        y = vy.toInt() + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FF00FF-001000", "0|1|FFFFFF, 0|2|FF00FF-001000, 0|3|FFFFFF",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt() + 45;
        y = vy.toInt() + 12 + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FFFFFF", "0|1|FF00FF-001000, 0|2|FFFFFF, 0|3|FF00FF-001000",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt() - 44;
        y = vy.toInt() - 12 + m_roleOffsetY;

        return true;
    }

    return false;
}

/*
void DF::moveRole(int hDir, int vDir, int speed)
// speed :  0 - relese direction key if direction is zero
//			1 - click direction key
//          2 - hold direction key
//          3 - double click then hold direction key
{
    static int hHeldKey = 0;
    static int vHeldKey = 0;


    if (speed == 0) {
        if (hDir && hHeldKey) {
            sendKey(Up, hHeldKey);
            hHeldKey = 0;
        }
        if (vDir && vHeldKey) {
            sendKey(Up, vHeldKey);
            vHeldKey = 0;
        }
    } else if (speed == 1) {
        if (hDir)
            sendKey(Stroke, (hDir > 0) ? m_arrowR : m_arrowL);
        if (vDir)
            sendKey(Stroke, (vDir > 0) ? m_arrowD : m_arrowU);
    } else {
        if (hDir) {
            if (hHeldKey) {
                sendKey(Up, hHeldKey);
            }
            hHeldKey = (hDir > 0) ? m_arrowR : m_arrowL;
        }
        if (vDir) {
            if (vHeldKey) {
                sendKey(Up, vHeldKey);
            }
            vHeldKey = (vDir > 0) ? m_arrowD : m_arrowU;
        }

        if (speed == 2) {
            if (hHeldKey)
                sendKey(Down, hHeldKey);
            if (vHeldKey)
                sendKey(Down, vHeldKey);
        } else if (speed == 3) {
            if (hHeldKey) {
                sendKey(Stroke, hHeldKey);
                sendKey(Down, hHeldKey);
            }
            if (vHeldKey) {
                if (!hHeldKey) {
                    sendKey(Stroke, m_arrowL);
                    sendKey(Stroke, m_arrowL);
                    sendKey(Stroke, m_arrowR);
                    sendKey(Down, m_arrowR);
                }
                sendKey(Down, vHeldKey);
                if (!hHeldKey) {
                    sendKey(Up, m_arrowR);
                }
            }
        }
    }
}
*/

void DF::moveRole(int hDir, int hSpeed, int vDir, int vSpeed)
{
    static int hHeldKey = 0;
    static int vHeldKey = 0;
    static int hKey = 0;
    static int vKey = 0;
    static int hPreSpeed = 0;
    static int vPreSpeed = 0;

    // Horizontal

    if (hDir) {
        hKey = (hDir > 0) ? m_arrowR : m_arrowL;
    } else {
        hKey = 0;
    }

    if (hSpeed == 0) {
        if (hKey && hHeldKey) {
            sendKey(Up, hHeldKey);
            hHeldKey = 0;
            hPreSpeed = hSpeed;
        }
    } else {
        if (hKey && hHeldKey) {
            if ((hKey == hHeldKey) && (hSpeed == hPreSpeed)) {
                hKey = 0;
            } else {
                sendKey(Up, hHeldKey);
                hHeldKey = 0;
            }
        }
        if (hSpeed == 1) {
            if (hKey) {
                sendKey(Dn, hKey);
                hHeldKey = hKey;
                hPreSpeed = hSpeed;
            }
        } else if (hSpeed ==2) {
            if (hKey) {
                sendKey(Sk, hKey);
                sendKey(Dn, hKey);
                hHeldKey = hKey;
                hPreSpeed = hSpeed;
            }
        }
    }

    // Vertical

    if (vDir) {
        vKey = (vDir > 0) ? m_arrowD : m_arrowU;
    } else {
        vKey = 0;
    }

    if (vSpeed == 0) {
        if (vKey && vHeldKey) {
            sendKey(Up, vHeldKey);
            vHeldKey = 0;
            vPreSpeed = vSpeed;
        }
    } else {
        if (vKey && vHeldKey) {
            if ((vKey == vHeldKey) && (vSpeed == vPreSpeed)) {
                vKey = 0;
            } else {
                sendKey(Up, vHeldKey);
                vHeldKey = 0;
            }
        }
        if (vSpeed == 1) {
            if (vKey) {
                sendKey(Dn, vKey);
                vHeldKey = vKey;
                vPreSpeed = vSpeed;
            }
        } else if (vSpeed ==2) {
            if (vKey) {
                if (!hHeldKey) {
                    sendKey(Sk, m_arrowL);
                    sendKey(Sk, m_arrowL);
                    sendKey(Sk, m_arrowR);
                    sendKey(Dn, m_arrowR);
                }
                sendKey(Dn, vKey);
                vHeldKey = vKey;
                vPreSpeed = vSpeed;
                if (!hHeldKey)
                    sendKey(Up, m_arrowR);
            }
        }
    }
}

bool DF::pickTrophies(bool &cross)
{
    bool finished = false;
    bool hArrived = false;
    bool vArrived = false;
    bool stucked = false;
    bool pickable = false;
    int preRoleX = -1;
    int preRoleY = -1;
    int roleX = -1;
    int roleY = -1;
    int hPreDir = 0;
    int vPreDir = 0;
    int hDir = 0;
    int vDir = 0;
    int hSpeed = 0;
    int vSpeed = 0;
    int x;
    int y;
    QTime timer;
    static const uint blockLength = 20;
    static const uint blockSize = blockLength * blockLength * 4;
    static const uint blockCount = 5;
    static uchar clientBlocks[blockCount][blockSize] = {0};
    static uchar preClientBlocks[blockCount][blockSize] = {0};
    static int counter = 0;

    cross = false;

    timer.start();

    // Avoid insisting picking a unpickable item
    if (counter++ > 8) {
        finished = true;
    } else {
        while (true) {
            // Timeout
            if (timer.elapsed() > 5000) {
                finished = true;
                break;
            }

            // Check if reached next section
            if (isBlackScreen()) {
                cross = true;
                finished = true;
            }

            // Get position of role
            if (!getRoleCoords(roleX, roleY))
                continue;

            // Get postion of trophy
            if (!getTrophyCoords(roleX, roleY, x, y, pickable)) {
                finished = true;
                break;
            }

            // Already stand on trophy
            if (pickable) {
                finished = false;
                break;
            }

            // Arrived but nothing is pickable
            if (hArrived && vArrived) {
                finished = false;
                break;
            }

            stucked = ((hPreDir != 0) || (vPreDir != 0)) && ((roleX == preRoleX) && (roleY == preRoleY));

            // Save position as previous
            preRoleX = roleX;
            preRoleY = roleY;

            // Check if role is stucked
            if (stucked) {
                // Situations against definition of stuck
                if (((hPreDir > 0) && (roleX > x)) ||
                        ((hPreDir < 0) && (roleX < x)) ||
                        ((vPreDir > 0) && (roleY > y)) ||
                        ((vPreDir < 0) && (roleY < y))) {
                    preRoleX = -1;
                    preRoleY = -1;
                    continue;
                }

                // Get client color blocks
                for (int i=0; i<blockCount; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                    memcpy(clientBlocks[i], data, blockSize);
                }

                stucked = false;
                for (int i=0; i<blockCount; ++i) {
                    if (memcmp(clientBlocks[i], preClientBlocks[i], blockSize) == 0) {
                        stucked = true;
                        break;
                    }
                }
                if (stucked) {
                    finished = true;
                    break;
                }

                // Save client blocks for checking next time
                memcpy(preClientBlocks, clientBlocks, blockCount*blockSize);
            }

            // Horizontal moving
            if (hArrived) {
                hDir = 0;
            } else {
                hDir = x - roleX;
                if (abs(hDir) < 10) {
                    hArrived = true;
                    hSpeed = 0;
                } else if (abs(hDir) < 30) {
                    if (hPreDir == 0) {
                        hSpeed = 1;
                    }
                } else {
                    if (hPreDir == 0) {
                        hSpeed = 2;
                    } else {
                        // Move over
                        if (abs(hDir + hPreDir) != abs(hDir) + abs(hPreDir)) {
                            hSpeed = 0;
                        }
                    }
                }
            }
            if (hSpeed == 0) {
                hDir = 1;
                hPreDir = 0;
            } else {
                hPreDir = hDir;
            }

            // Vertical moving
            if (vArrived) {
                vDir = 0;
            } else {
                vDir = y - roleY;
                if (abs(vDir) < 10) {
                    vArrived = true;
                    vSpeed = 0;
                } else if (abs(vDir) < 30) {
                    if (vPreDir == 0) {
                        vSpeed = 1;
                    }
                } else {
                    if (vPreDir == 0) {
                        vSpeed = 2;
                    } else {
                        // Move over
                        if (abs(vDir + vPreDir) != abs(vDir) + abs(vPreDir)) {
                            vSpeed = 0;
                        }
                    }
                }
            }
            if (vSpeed == 0) {
                vDir = 1;
                vPreDir = 0;
            } else {
                vPreDir = vDir;
            }

            // Move
            moveRole(hDir, hSpeed, vDir, vSpeed);
        }
    }

    moveRole(1, 0, 1, 0);

    if (pickable)
        sendKey(Sk, "x", 50);

    if (finished)
        counter = 0;

    return finished;
}

bool DF::navigate(int x, int y, bool end)
{
    int checkBlackScreenCount = 0;
    bool hArrived = false;
    bool vArrived = false;
    bool stucked = false;
    int preRoleX = -1;
    int preRoleY = -1;
    int roleX = -1;
    int roleY = -1;
    int hPreDir = 0;
    int vPreDir = 0;
    int hDir = 0;
    int vDir = 0;
    int hSpeed = 0;
    int vSpeed = 0;
    QTime timer;
    static const uint blockLength = 20;
    static const uint blockSize = blockLength * blockLength * 4;
    static const uint blockCount = 5;
    static uchar preClientBlocks[blockCount][blockSize] = {0};
    static uchar clientBlocks[blockCount][blockSize] = {0};
    QVariant vx, vy;

    if (-1 == x)
        hArrived = true;
    if (-1 == y)
        vArrived = true;

    timer.start();

    while (true) {
        // Timeout
        if (timer.elapsed() > 5000) {
            return false;
        }

        // Check if reached next section
        if (isBlackScreen()) {
            // Stop
            moveRole(1, 0, 1, 0);

            return true;
        }

        // Check arrival
        if (hArrived && vArrived) {
            // Check black screen for secs
            if (end) {
                if (checkBlackScreenCount++ < 100) {
                    msleep(50);
                    continue;
                }
            }

            // end
            break;
        }

        // Get position
        if (!getRoleCoords(roleX, roleY))
            continue;

        stucked = ((hPreDir != 0) || (vPreDir != 0)) && ((roleX == preRoleX) && (roleY == preRoleY));

        // Save position as previous
        preRoleX = roleX;
        preRoleY = roleY;

        // Check if role is stucked
        if (stucked) {
            // Situations against definition of stuck
            if (((hPreDir > 0) && (roleX > x)) ||
                ((hPreDir < 0) && (roleX < x)) ||
                ((vPreDir > 0) && (roleY > y)) ||
                ((vPreDir < 0) && (roleY < y))) {
                preRoleX = -1;
                preRoleY = -1;
                continue;
            }

            // Get client color blocks
            for (int i=0; i<blockCount; ++i) {
                uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                memcpy(clientBlocks[i], data, blockSize);
            }

            stucked = false;
            for (int i=0; i<blockCount; ++i) {
                if (memcmp(clientBlocks[i], preClientBlocks[i], blockSize) == 0) {
                    stucked = true;
                    break;
                }
            }
            if (stucked) {
                // Stucked, but also need to check blackscreen
                moveRole(1, 0, 1, 0);
                hPreDir = vPreDir = 0;
                hArrived = vArrived = true;
                continue;
            }

            // Save client blocks for checking next time
            memcpy(preClientBlocks, clientBlocks, blockCount*blockSize);
        }

        // Horizontal moving
        if (hArrived) {
            hDir = 0;
        } else {
            hDir = x - roleX;
            if (abs(hDir) < 10) {
                hArrived = true;
                hSpeed = 0;
            } else if (abs(hDir) < 30) {
                if (hPreDir == 0) {
                    hSpeed = 1;
                }
            } else {
                if (hPreDir == 0) {
                    hSpeed = 2;
                } else {
                    // Move over
                    if (abs(hDir + hPreDir) != abs(hDir) + abs(hPreDir)) {
                        hSpeed = 0;
                    }
                }
            }
        }
        if (hSpeed == 0) {
            hDir = 1;
            hPreDir = 0;
        } else {
            hPreDir = hDir;
        }

        // Vertical moving
        if (vArrived) {
            vDir = 0;
        } else {
            vDir = y - roleY;
            if (abs(vDir) < 10) {
                vArrived = true;
                vSpeed = 0;
            } else if (abs(vDir) < 30) {
                if (vPreDir == 0) {
                    vSpeed = 1;
                }
            } else {
                if (vPreDir == 0) {
                    vSpeed = 2;
                } else {
                    // Move over
                    if (abs(vDir + vPreDir) != abs(vDir) + abs(vPreDir)) {
                        vSpeed = 0;
                    }
                }
            }
        }
        if (vSpeed == 0) {
            vDir = 1;
            vPreDir = 0;
        } else {
            vPreDir = vDir;
        }

        // Move
        moveRole(hDir, hSpeed, vDir, vSpeed);
    }

    moveRole(1, 0, 1, 0);

    return false;
}

bool DF::navigateSection(int sectionIndex)
{
    bool end = false;

    if ((0 <= sectionIndex) && (sectionIndex < m_pathList.count())) {
        const QVariantList &sectionPathList = m_pathList.at(sectionIndex).toList();
        for (int i = 0; i < sectionPathList.count(); ++i) {
            QVariantList &position = sectionPathList.at(i).toList();
            if (position.count() < 2) {
                throw DFSettingError;
            }

            end = (i == (sectionPathList.count() - 1)) ? true : false;
            if (navigate(position.first().toInt(), position.last().toInt(), end)) {
                return true;
            }
        }
    }

    return false;
}

bool DF::killBoss()
{
    QVariant vx, vy;
    int rx, ry;
    int bx, by;

    // Get role position
    if (!getRoleCoords(rx, ry)) {
        return true;
    }

    // Get boss position
    if (!m_dm.FindMultiColor(0, 100, 800, 450,
                        "FF00FF", "1|0|FF00FF, 2|0|FF00FF, 3|0|FF00FF",
                        1.0, 0,
                        vx, vy)) {
        useOwnSkill();
        summonSupporter();
        return true;
    }
    bx = vx.toInt() + 50;
    by = vy.toInt() + 150;

    // Get away from boss
    if (abs(rx- bx) < 250) {
        moveRole((bx < 400) ? 1 : -1, 2, 0, 0);
        approxSleep(100);
    } else {
        useOwnSkill();
        summonSupporter();
    }

    return true;
}

