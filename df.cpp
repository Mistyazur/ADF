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
    if (winList.isEmpty())
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
        ret = m_dm.BindWindow(m_hBindWnd, "dx2", "dx", "dx", 101);
    else {
//        SetWindowPos((HWND)m_hBindWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        ret = m_dm.BindWindow(m_hBindWnd, "normal", "normal", "normal", 101);
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
            sendKey(Stroke, 27, 200);
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
            sendKey(Stroke, 27, 200);
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
    for (int i = 0; i < 40; ++i)
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
    sendMouse(Left, roleX, roleY, 1000);

    // Game start
    sendKey(Stroke, 32, 100);
    sendKey(Stroke, 32, 5000);

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
    sendKey(Stroke, 32, 1000);
    sendKey(Stroke, 32, 1000);

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
    sendKey(Stroke, "N", 1000);
    sendMouse(Right, x, y, 200);
    sendMouse(Right, x, y, 200);
    sendKey(Stroke, "N", time);
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
            for (int i = 0; i < 3; ++i)
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
            sendKey(Down, 16, 300);
            sendMouse(Left, vx, vy, 100);
            sendKey(Up, 16, 300);

            // Confirm
            sendKey(Stroke, 13, 200);
            sendKey(Stroke, 13, 200);
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

//    // Save money
//    if (m_dm.FindPic(CLIENT_RECT, "save_gold.bmp", "000000", 1.0, 0, vx, vy) != -1) {
//        sendMouse(Left, vx, vy, 1000);
//        sendMouse(Left, vx, vy, 1000);
//    }

    // Save items
    sendKey(Stroke, "A", 100);
    sendKey(Stroke, 13, 1000);

    setMouseDuration(oldMouseDuration);
    setKeyDuration(oldKeyDuration);

    openSystemMenu();
    closeSystemMenu();
}

void DF::cancelCrystalContract()
{
    // Open packet
    sendKey(Stroke, "i", 500);

    // Switch to material
    sendMouse(Left, 590, 250, 200);

    // Cancel contract
    sendMouse(Left, 745, 470, 100);
    sendMouse(Left, 745, 470, 100);

    // Close packet
    sendKey(Stroke, "i", 500);
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
            sendMouse(Left, vx.toInt() - 30, vy.toInt() + 55, 100);

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

    // Reset last role index
    if (!resetRoleIndex(dungeon)) {
        // Get last role index that was automating
        m_lastRoleIndex = settings.value("role_index", 0).toInt();
    }

    // Get count of role that'll be automated
    m_roleCount = settings.value("role_count", 0).toInt();

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

bool DF::resetRoleIndex(const QString &dungeon)
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
        settings.setValue("role_index", 0);
        m_lastRoleIndex = 0;
        reseted = true;
    }
    settings.setValue("last_time", currentDateTime);
    settings.sync();

    settings.endGroup();

    return reseted;
}

bool DF::updateRoleIndex(const QString &dungeon)
{
    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    m_lastRoleIndex += 1;
    if (m_lastRoleIndex >= m_roleCount) {
        return false;
    }

    settings.beginGroup(dungeon);
    settings.setValue("role_index", m_lastRoleIndex);
    settings.endGroup();

    return true;
}

void DF::pickRole()
{
    pickRole(m_lastRoleIndex);
}

//bool DF::initDungeonSettings(const QString &file)
//{
//    JSettings js(QApplication::applicationDirPath() + "/" + file);
//    m_pathList = js.value("Path").toList();
//    m_nodeList = js.value("Nodes").toList();
//    if (m_pathList.isEmpty() || m_nodeList.isEmpty())
//        return false;

//    QVariantList mapRect = js.value("MapRect").toList();
//    if (mapRect.count() < 4)
//        return false;
//    m_dungeonMapX1 = mapRect.at(0).toInt();
//    m_dungeonMapY1 = mapRect.at(1).toInt();
//    m_dungeonMapX2 = mapRect.at(2).toInt();
//    m_dungeonMapY2 = mapRect.at(3).toInt();

//    return true;
//}

bool DF::initRoleOffset()
{
    QVariant vx, vy;

    bool ok = false;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindMultiColor(0, 100, 800, 400,
                                 "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
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

bool DF::isBlackScreen(int x1, int y1, int x2, int y2)
{
    int blackCount = m_dm.GetColorNum(x1, y1, x2, y2, "000000-202020", 1.0);
    int totalCount = (x2 - x1) * ( y2 - y1);
    if (blackCount < totalCount * 0.8)
        return false;

    return true;
}

bool DF::enterDungeon(int index, int difficulty, bool leftEntrance)
{
    QVariant x, y;

    int directionKey = leftEntrance ? m_arrowL : m_arrowR;

    sendKey(Down, directionKey, 3000);
    sendKey(Up, directionKey, 100);

    // Cancel mercenary
    if (m_dm.FindPic(CLIENT_RECT, "announcement.bmp", "000000", 1.0, 0, x, y) != -1) {
        sendMouse(Left, x.toInt() - 20, y.toInt() + 100, 100);
        sendMouse(Left, x.toInt() - 20, y.toInt() + 100, 100);
        openSystemMenu();
        closeSystemMenu();

        sendKey(Down, leftEntrance ? m_arrowR : m_arrowL, 500);
        sendKey(Up, leftEntrance ? m_arrowR : m_arrowL, 100);
        sendKey(Down, directionKey, 1000);
        sendKey(Up, directionKey, 100);
    }

    // Wait for dungeon picking UI
    for (int i=0; i<10; ++i) {
        if (m_dm.FindPic(CLIENT_RECT, "options_back_to_town.bmp", "000000", 1.0, 0, x, y) != -1) {
            goto EnterDungeon;
        }
        msleep(1000);
    }

    return false;

EnterDungeon:

    int oldKeyDuration = setKeyDuration(200);

    // Pick dungeon
    for (int i=0; i<index; ++i)
        sendKey(Stroke, m_arrowU, 100);

    // Pick difficulty
    for (int i=0; i<4; ++i)
        sendKey(Stroke, m_arrowL);
    for (int i=0; i<difficulty; ++i)
        sendKey(Stroke, m_arrowR, 100);

    // Commit
    sendKey(Stroke, 32, 500);

    setKeyDuration(oldKeyDuration);

    // Wait
    return waitForDungeonBeign();
}

bool DF::reenterDungeon()
{
    // Reenter
    sendKey(Stroke, 121, 500);
    sendKey(Stroke, 121, 500);

    // Wait
    return waitForDungeonBeign();
}

bool DF::waitForDungeonBeign()
{
    QVariant x, y;

    for (int i = 0; i < 10; ++i) {
        if (m_dm.GetColorNum(300, 575, 500, 595, "FFFF00", 1.0) > 100) {
            for (int j = 0; j < 10; ++j) {
                if (m_dm.GetColorNum(300, 575, 500, 595, "FFFF00", 1.0) == 0) {
                    msleep(1000);
                    return true;
                }
                msleep(1000);
            }
        }
        msleep(1000);
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

    if (m_dm.FindPic(CLIENT_RECT, "dungeon_end.bmp", "202020", 1.0, 2, vx, vy) == -1)
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
            sendKey(Stroke, 53, 100);
            break;
        }

        approxSleep(500);
    }
}

void DF::summonSupporter()
{
    // Tab to summon
    sendKey(Stroke, 9, 100);
}

void DF::useOwnSkill()
{
    sendKey(Stroke, "z", 200);
}

void DF::buff()
{
    sendKey(Stroke, m_arrowU);
    sendKey(Stroke, m_arrowD);
    sendKey(Stroke, 32, 2000);

    sendKey(Stroke, m_arrowD);
    sendKey(Stroke, m_arrowU);
    sendKey(Down, 32, 2000);
    sendKey(Up, 32);
}

int DF::getSectionIndex()
{
    QVariant vx, vy;
    int x, y;

    bool ok = false;
    for (int i = 0; i < 50; ++i) {
        if (m_dm.FindPic(750, 0, 800, 50, "dungeon_in.bmp", "101010", 1.0, 0, vx, vy) != -1) {
            ok = true;
            break;
        }
        approxSleep(100);
    }
    if (!ok) {
        qDebug()<<"Can't get section index";
        throw DFRESTART;
    }

    if (!getRoleCoordsInMap(x, y))
        return -1;

    return m_nodeList.indexOf(QVariantList({x , y}));
}

bool DF::isSectionClear(const QString &brightColor, const int threshold)
{
    int x, y;
    if (!getRoleCoordsInMap(x, y))
        return false;

//    qDebug()<<x<<y;

//    qDebug()<<"L"<<m_dm.GetColorNum(x-25, y-4, x-9, y+12, brightColor, 1.0);
    if (m_dm.GetColorNum(x-25, y-4, x-9, y+12, brightColor, 1.0) > threshold)
        return true;

//    qDebug()<<"R"<<m_dm.GetColorNum(x+11, y-4, x+27, y+12, brightColor, 1.0);
    if (m_dm.GetColorNum(x+11, y-4, x+27, y+12, brightColor, 1.0) > threshold)
        return true;

//    qDebug()<<"D"<<m_dm.GetColorNum(x-6, y-22, x+10, y-6, brightColor, 1.0);
    if (m_dm.GetColorNum(x-6, y-22, x+8, y-8, brightColor, 1.0) > threshold)
        return true;

//    qDebug()<<"U"<<m_dm.GetColorNum(x-6, y+14, x+10, y+30, brightColor, 1.0);
    if (m_dm.GetColorNum(x-6, y+14, x+8, y+28, brightColor, 1.0) > threshold)
        return true;

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

    res = m_dm.FindPicEx(0, 0, 800, 500, "drop_activated_left.bmp|drop_normal_left.bmp", "000000", 1.0, 1);
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

    if (m_dm.FindPic(resList.at(1).toInt(), resList.at(2).toInt(), resList.at(1).toInt() + 200, resList.at(2).toInt() + 6,
                     "drop_normal_right.bmp", "000000", 1.0, 0, vx, vy) != -1) {
        nx = resList.at(1).toInt() + (vx.toInt() + 10 - resList.at(1).toInt()) / 2;
        ny = resList.at(2).toInt() + 30;
    } else {
        nx = resList.at(1).toInt() + 50;
        ny = resList.at(2).toInt() + 30;
    }

    return true;
}

bool DF::getRoleCoordsInMap(int &x, int &y)
{
    QVariant vx, vy;

    if (m_dm.FindPic(m_dungeonMapX1, m_dungeonMapY1, m_dungeonMapX2, m_dungeonMapY2,
                     "dungeon_map_role.bmp", "101010", 1.0, 0, vx, vy) == -1) {
        return false;
    }

    x = vx.toInt();
    y = vy.toInt();

    return true;
}

bool DF::getRoleCoords(int &x, int &y)
{
    QVariant vx, vy;

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt();
        y = vy.toInt() + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FF00FF-000F00, 0|3|FFFFFF",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt() + 45;
        y = vy.toInt() + 12 + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 500,
                        "FFFFFF", "0|1|FF00FF-000F00, 0|2|FFFFFF, 0|3|FF00FF-000F00",
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

    hKey = 0;
    vKey = 0;

    if (hDir)
        hKey = (hDir > 0) ? m_arrowR : m_arrowL;
    if (vDir)
        vKey = (vDir > 0) ? m_arrowD : m_arrowU;

    if (hSpeed == 0) {
        if (hHeldKey) {
            sendKey(Up, hHeldKey);
            hHeldKey = 0;
        }
    } else {
        if (hKey && hHeldKey) {
            if (hKey == hHeldKey) {
                hKey = 0;
            } else {
                sendKey(Up, hHeldKey);
                hHeldKey = 0;
            }
        }

        if (hSpeed == 1) {
            if (hKey) {
                sendKey(Down, hKey);
                hHeldKey = hKey;
            }
        } else if (hSpeed ==2) {
            if (hKey) {
                sendKey(Stroke, hKey);
                sendKey(Down, hKey);
                hHeldKey = hKey;
            }
        }
    }

    if (vSpeed == 0) {
        if (vHeldKey) {
            sendKey(Up, vHeldKey);
            vHeldKey = 0;
        }
    } else {
        if (vKey && vHeldKey) {
            if (vKey == vHeldKey) {
                vKey = 0;
            } else {
                sendKey(Up, vHeldKey);
                vHeldKey = 0;
            }
        }

        if (vSpeed == 1) {
            if (vKey) {
                sendKey(Down, vKey);
                vHeldKey = vKey;
            }
        } else if (vSpeed ==2) {
            if (vKey) {
                if (!hHeldKey) {
                    sendKey(Stroke, m_arrowL);
                    sendKey(Stroke, m_arrowL);
                    sendKey(Stroke, m_arrowR);
                    sendKey(Down, m_arrowR);
                }
                sendKey(Down, vKey);
                vHeldKey = vKey;
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
    QTime stuckTimer;
    static const uint blockLength = 20;
    static const uint blockSize = blockLength * blockLength * 4;
    static const uint blockCount = 5;
    static uchar preClientBlocks[blockCount][blockSize] = {0};
    static uchar clientBlocks[blockCount][blockSize] = {0};
    static int counter = 0;

    timer.start();
    cross = false;

    // Avoid insisting picking a unpickable item
    if (counter++ > 10) {
        finished = true;
    } else {
        while (true) {
            // Timeout
            if (timer.elapsed() > 20000) {
                finished = true;
                break;
            }

            // Check if reached next section
            if (isBlackScreen(0, 0, 50, 50)) {
                cross = true;
                finished = true;
                break;
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
                moveRole(1, 0, 1, 0);
                sendKey(Stroke, "x", 50);
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

                if (stuckTimer.isNull()) {
                    // Get client color blocks
                    for (int i=0; i<blockCount; ++i) {
                        uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                        memcpy(preClientBlocks[i], data, blockSize);
                    }

                    // Start timer
                    stuckTimer.start();
                } else {
                    // Trigger checking every 50 msecs
                    if (stuckTimer.elapsed() > 50) {
                        // Get client color blocks
                        for (int i=0; i<blockCount; ++i) {
                            uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                            memcpy(clientBlocks[i], data, blockSize);
                        }

                        // Check if role is stucked
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

                        // Restart timer
                        stuckTimer.restart();
                    }
                }
            } else {
                // Horizontal moving
                if (hArrived) {
                    hDir = 0;
                } else {
                    hDir = x - roleX;
                    if (abs(hDir + hPreDir) != abs(hDir) + abs(hPreDir)) {
                        // Move over
                        hSpeed = 0;
                    } else {
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
                            }
                        }
                    }
                }

                // Vertical moving
                if (vArrived) {
                    vDir = 0;
                } else {
                    vDir = y - roleY;
                    if (abs(vDir + vPreDir) != abs(vDir) + abs(vPreDir)) {
                        // Move over
                        vSpeed = 0;
                    } else {
                        if (abs(vDir) < 10) {
                            vArrived = true;
                            vSpeed = 0;
                        } else if (abs(vDir) < 30){
                            if (vPreDir == 0) {
                                vSpeed = 1;
                            }
                        } else {
                            if (vPreDir == 0) {
                                vSpeed = 2;
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
                if (vSpeed == 0) {
                    vDir = 1;
                    vPreDir = 0;
                } else {
                    vPreDir = vDir;
                }

                moveRole(hDir, hSpeed, vDir, vSpeed);
            }
        }
    }

    moveRole(1, 0, 1, 0);

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
    QTime stuckTimer;
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
        if (timer.elapsed() > 20000) {
            return false;
        }

        // Check if reached next section
        if (isBlackScreen(0, 0, 50, 50)) {
            // Stop
            moveRole(1, 0, 1, 0);
            hPreDir = vPreDir = 0;

            // Wait until not black screen
            for (int i=0; i<100; ++i) {
                msleep(100);
                if (!isBlackScreen(0, 0, 50, 50)) {
                    return true;
                }
            }
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

            if (stuckTimer.isNull()) {
                // Get client color blocks
                for (int i=0; i<blockCount; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                    memcpy(preClientBlocks[i], data, blockSize);
                }

                // Start timer
                stuckTimer.start();
            } else {
                // Trigger checking every 50 msecs
                if (stuckTimer.elapsed() > 50) {
                    // Get client color blocks
                    for (int i=0; i<blockCount; ++i) {
                        uchar *data = (uchar *)m_dm.GetScreenData(i * blockLength, 0, i * blockLength + blockLength, blockLength);
                        memcpy(clientBlocks[i], data, blockSize);
                    }

                    // Check if role is stucked
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

                    // Restart timer
                    stuckTimer.restart();
                }
            }
        } else {
            // Horizontal moving
            if (hArrived) {
                hDir = 0;
            } else {
                hDir = x - roleX;
                if (abs(hDir + hPreDir) != abs(hDir) + abs(hPreDir)) {
                    // Move over
                    hSpeed = 0;
                } else {
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
                        }
                    }
                }
            }

            // Vertical moving
            if (vArrived) {
                vDir = 0;
            } else {
                vDir = y - roleY;
                if (abs(vDir + vPreDir) != abs(vDir) + abs(vPreDir)) {
                    // Move over
                    vSpeed = 0;
                } else {
                    if (abs(vDir) < 10) {
                        vArrived = true;
                        vSpeed = 0;
                    } else if (abs(vDir) < 30){
                        if (vPreDir == 0) {
                            vSpeed = 1;
                        }
                    } else {
                        if (vPreDir == 0) {
                            vSpeed = 2;
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
            if (vSpeed == 0) {
                vDir = 1;
                vPreDir = 0;
            } else {
                vPreDir = vDir;
            }

            moveRole(hDir, hSpeed, vDir, vSpeed);
        }
    }

    moveRole(1, 0, 1, 0);

    return false;
}

bool DF::navigateSection(int sectionIndex)
{
    bool end = false;

    if (sectionIndex < m_pathList.count()) {
        const QVariantList &sectionPathList = m_pathList.at(sectionIndex).toList();
        for (int i = 0; i < sectionPathList.count(); ++i) {
            QVariantList &position = sectionPathList.at(i).toList();
            if (position.count() < 2) {
                throw DFSettingError;
            }

            end = (i == (sectionPathList.count()-1)) ? true : false;
            if (navigate(position.first().toInt(), position.last().toInt(), end)) {
                return true;
            }
        }
    }

    return false;
}

bool DF::fightBoss()
{
    QVariant vx, vy;
    int rx, ry;
    int bx, by;

    if (!getRoleCoords(rx, ry)) {
        return true;
    }

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

    if (abs(rx- bx) < 250) {
        moveRole((bx < 400) ? 1 : -1, 2, 0, 0);
        approxSleep(100);
    } else {
        useOwnSkill();
        summonSupporter();
    }

    return true;
}

