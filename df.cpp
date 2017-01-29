#include "df.h"

#include "JSettings/jsettings.h"

#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QTime>
#include <QDebug>

#include <Windows.h>

#undef FindWindow

#define PICK_CHANNNEL_POS 380, 450


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
    setMouseDuration(50);
    setKeyDuration(50);

    // Set mouse and key delay
    setMouseDelayDelta(0.2);
    setKeyDelayDelta(0.2);
}

DF::~DF()
{
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

bool DF::closeClient()
{
//    int hTGPWnd;
//    int hConfirmWnd;

//    hTGPWnd = m_dm.FindWindow("TWINCONTROL", "腾讯游戏平台");
//    if (hTGPWnd == 0) {
//        qDebug()<<"Close Client: Can't find tgp window";
//        return false;
//    }

//    // Activate tgp
//    if (m_dm.GetForegroundWindow() != hTGPWnd) {
//        m_dm.SetWindowState(hTGPWnd, 1);
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
//        qDebug()<<"Close Client: Can't activate tgp window";
//        return false;
//    }

//    // Bind tgp
//    m_dm.SetWindowSize(hTGPWnd, 1020, 720);
//    m_dm.BindWindow(hTGPWnd, "normal", "normal", "normal", 0);
//    msleep(1000);

//    // Close client
//    sendMouse(Left, 120, 310, 3000);

//    // Unbind tgp
//    m_dm.UnBindWindow();

//    // Confirm
//    for (int i = 0; i < 5; ++i) {
//        msleep(1000);

//        hConfirmWnd = m_dm.FindWindow("TWINCONTROL", "");
//        if (hConfirmWnd && (hConfirmWnd != hTGPWnd)) {
//            m_dm.BindWindow(hConfirmWnd, "normal", "normal", "normal", 0);
//            sendMouse(Left, 250, 170, 200);
//            m_dm.UnBindWindow();
//            return true;
//        }
//    }

//    qDebug()<<"Close Client: failed";

    QProcess::startDetached("TASKKILL /IM DNF.exe /F /T");
    msleep(2000);
    QProcess::startDetached("TASKKILL /IM Client.exe /F /T");
    msleep(3000);

    return false;
}

bool DF::startClient()
{
    int hTGPWnd;
    int hCheckingWnd;

    hTGPWnd = m_dm.FindWindow("TWINCONTROL", "腾讯游戏平台");
    if (hTGPWnd == 0) {
        qDebug()<<"Start Client: Can't find tgp window";
        return false;
    }

    // Activate tgp
    if (m_dm.GetForegroundWindow() != hTGPWnd) {
        m_dm.SetWindowState(hTGPWnd, 1);
        msleep(1000);
    }

    // wait for tgp activated
    bool ok = false;
    for (int i = 0; i < 10; ++i) {
        if (m_dm.GetForegroundWindow() == hTGPWnd) {
            ok = true;
            break;
        }
        msleep(1000);
    }
    if (!ok) {
        qDebug()<<"Start Client: Can't activate tgp window";
        return false;
    }

    // Bind tgp
    m_dm.SetWindowSize(hTGPWnd, 1020, 720);
    m_dm.BindWindow(hTGPWnd, "normal", "normal", "normal", 0);

    // Start client
    sendMouse(Left, 50, 300, 3000);
    sendMouse(Left, 900, 680, 1000);

    // Unbind tgp
    m_dm.UnBindWindow();

    // Skip checking window
    for (int i = 0; i < 5; ++i) {
        msleep(1000);

        hCheckingWnd = m_dm.FindWindow("TWINCONTROL", "");
        if (hCheckingWnd && (hCheckingWnd != hTGPWnd)) {
            m_dm.BindWindow(hCheckingWnd, "normal", "normal", "normal", 0);
            sendMouse(Left, 415, 60, 200);
            m_dm.UnBindWindow();
            break;
        }
    }

    // Wait for client
    for (int i = 0; i < 200; ++i) {
        if (window() != 0) {
            return true;
        }
        msleep(1000);
    }

    qDebug()<<"Start Client: failed";

    return false;
}

bool DF::waitForRoleList()
{
    QVariant vx, vy;

    for (int i = 0; i < 20; ++i) {
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

    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(400, 450, 470, 480, "pick_channel.bmp", "000000", 1.0, 0, vx, vy) == -1) {
            sendKey(Stroke, 27, 1000);
        } else {
            return true;
        }
    }

    return false;
}

bool DF::closeSystemMenu()
{
    QVariant vx, vy;

    for (int i = 0; i < 10; ++i) {
        if (m_dm.FindPic(400, 450, 470, 480, "pick_channel.bmp", "000000", 1.0, 0, vx, vy) == -1) {
            return true;
        } else {
            sendKey(Stroke, 27, 1000);
        }
    }

    return false;
}

bool DF::isDisconnected()
{
    QVariant vx, vy;

    if (m_dm.FindPic(300, 200, 500, 400, "disconnected.bmp", "0F0F0F", 1.0, 0, vx, vy) == -1) {
        return false;
    }

//    if (m_dm.FindPic(300, 200, 500, 400, "announcement.bmp", "000000", 1.0, 0, vx, vy) == -1) {
//        return false;
//    }

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
    sendMouse(LeftDown, 580, 290, 500);
    sendMouse(Move, 580, 100, 500);
    sendMouse(LeftUp, -1, -1, 2500);

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
    sendMouse(Left, 400, 550, 1000);

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

    sendMouse(Left, PICK_CHANNNEL_POS, 1000);

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
    sendKey(Stroke, "N", 500);
    sendMouse(Right, x, y, 500);
    sendKey(Stroke, "N", 500);

    approxSleep(time);
}

void DF::sellEquipment()
{
    QVariant vx, vy;
    int ox, oy;
    int x, y;
    bool found = false;

    // Wait for selling
    for (int i = 0; i < 100; ++i) {
        if (m_dm.FindPic(0, 300, 400, 600, "sell.bmp", "000000", 1.0, 1, vx, vy) != -1) {
            found = true;
            break;
        }
        msleep(200);
    }
    if (!found) {
        qDebug()<<"sellEquipment error";
        return;
    }

    // Click sell button
    sendMouse(Left, vx, vy, 300);

    // Search sort button
    if (m_dm.FindPic(0, 300, 800, 600, "sort.bmp", "000000", 1.0, 1, vx, vy) != -1) {
        sendMouse(Left, vx.toInt() - 190, vy.toInt() - 240, 100);  // Click equipment button
        sendMouse(Left, vx.toInt(), vy.toInt(), 100);  // Click sort button

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
            if (m_dm.FindPic(0, 0, 800, 400, "unique.bmp|legendary.bmp|epic.bmp", "000000", 1.0, 0, vx, vy) == -1) {
                sendMouse(Left, x, y, 200);

                sendKey(Stroke, 32, 100);
                sendKey(Stroke, 32, 100);
            }
        }
    }
}

bool DF::initDungeonSettings(const QString &dungeon)
{
    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    settings.beginGroup(dungeon);

    // Get count of role that'll be automated
    m_roleCount = settings.value("role_count", 0).toInt();

    // Get last role index that was automating
    QDateTime lastDateTime = settings.value("last_time").toDateTime();
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QDateTime thresholdDateTime;
    thresholdDateTime.setDate(lastDateTime.date());
    thresholdDateTime.setTime(QTime(6, 0, 0));
    if (lastDateTime.time().hour() >= 6) {
        thresholdDateTime = thresholdDateTime.addDays(1);
    }

    if (currentDateTime >= thresholdDateTime) {
        settings.setValue("last_time", currentDateTime);
        settings.setValue("role_index", 0);
        settings.sync();
    }

    m_lastRoleIndex = settings.value("role_index", 0).toInt();

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

bool DF::updateRoleIndex(const QString &dungeon)
{
    QSettings settings(QApplication::applicationDirPath() + "/Dungeon.ini", QSettings::IniFormat);

    m_lastRoleIndex += 1;
    if (m_lastRoleIndex >= m_roleCount) {
        return false;
    }

    settings.beginGroup(dungeon);

    settings.setValue("role_index", m_lastRoleIndex);
    settings.setValue("last_time", QDateTime::currentDateTime());

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
    if (!m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy))
        return false;

    m_roleOffsetY = 430 - vy.toInt();
    qDebug()<<"Role offset y: "<<m_roleOffsetY;

    return true;
}

bool DF::isBlackScreen(int x1, int y1, int x2, int y2)
{
    int blackCount = m_dm.GetColorNum(x1, y1, x2, y2, "000000", 1.0);
    int totalCount = (x2-x1)*(y2-y1);
    if (blackCount < totalCount*0.8)
        return false;
    return true;
}

bool DF::enterDungeon(int index, int difficulty, bool leftEntrance)
{
    QVariant x, y;

    // Wait for splash disappeared and unlocking colourless crystall
    approxSleep(45000, 0);

    int directionKey = leftEntrance ? m_arrowL : m_arrowR;

    sendKey(Down, directionKey, 3000);
    sendKey(Up, directionKey, 100);
    for (int i=0; i<10; ++i) {
        if (m_dm.FindPic(CLIENT_RECT, "options_back_to_town.bmp", "000000", 1.0, 0, x, y) != -1) {
            goto EnterDungeon;
        }
        msleep(1000);
    }

    return false;

EnterDungeon:
    // Pick dungeon
    for (int i=0; i<index; ++i)
        sendKey(Stroke, m_arrowU, 200);

    // Pick difficulty
    for (int i=0; i<8; ++i)
        sendKey(Stroke, m_arrowL, 50);
    for (int i=0; i<difficulty; ++i)
        sendKey(Stroke, m_arrowR, 200);

    // Commit
    sendKey(Stroke, 32, 500);

    // Wait
    for (int i = 0; i < 20; ++i) {
        if (isInDungeon())
            return true;
        msleep(1000);
    }

    return false;
}

bool DF::reenterDungeon()
{
    // Reenter
    sendKey(Stroke, 27, 1500);
    sendKey(Stroke, 121, 500);

    // Wait
    for (int i = 0; i < 20; ++i) {
        if (isInDungeon()) {
            return true;
        }
        msleep(1000);
    }

    return false;
}

bool DF::isInDungeon()
{
    QVariant x, y;

    if (m_dm.FindPic(770, 0, 800, 30, "dungeon_in.bmp", "000000", 1.0, 0, x, y) == -1)
        return false;

    return true;
}

bool DF::isRoleDead()
{
    QVariant vx, vy;

    if (m_dm.FindPic(330, 420, 360, 450, "dead.bmp", "0F0F0F", 1.0, 0, vx, vy) == -1)
        return false;

    return true;
}

bool DF::isDungeonEnded()
{
    QVariant x, y;

    if (m_dm.FindPic(CLIENT_RECT, "dungeon_end.bmp", "000000", 0.9, 2, x, y) == -1)
        return false;

    return true;
}

bool DF::isNoDungeonPoint()
{
    if (m_dm.GetColor(339, 553) == "000000") {
        return true;
    }

    return false;
}

bool DF::summonSupporter()
{
//    if (m_dm.GetColor(695, 510).toUpper() == "7F7B35") {
        sendKey(Stroke, "z", 100);
        sendKey(Stroke, 9, 100);  // Tab

        return true;
//    }

        return false;
}

void DF::buff()
{
    sendKey(Stroke, m_arrowU, 50);
    sendKey(Stroke, m_arrowD, 50);
    sendKey(Stroke, 32, 1000);

    sendKey(Stroke, m_arrowD, 50);
    sendKey(Stroke, m_arrowU, 50);
    sendKey(Down, 32, 2000);
    sendKey(Up, 32, 100);
}

void DF::rectifySectionIndex(int &sectionIndex)
{
    int x, y;

    bool ok = false;
    for (int i = 0; i < 10; ++i) {
        if (getRoleCoordsInMap(x, y)) {
            ok = true;
            break;
        }
        approxSleep(500);
    }
    if (!ok) {
        qDebug()<<"RecitySectionIndex: getRoleCoordsInMap failed";
        throw DFRESTART;
    }

    QVariantList node = QVariantList({x , y});
    int rectifiedSectionIndex = m_nodeList.indexOf(node);
    if (rectifiedSectionIndex != -1)
        sectionIndex = rectifiedSectionIndex;
}

bool DF::isSectionClear(const QString &brightColor)
{
    int x, y;
    ulong beforeBlocks[4][196] = {0};


    if (!getRoleCoordsInMap(x, y))
        return false;

    ulong *data = nullptr;
    ulong size = 196;
    int brightColorCountMin = 50;
    ulong count;

    memcpy(beforeBlocks[0], (uchar *)m_dm.GetScreenData(x-24, y-4, x-10, y+10), size*sizeof(ulong));
    memcpy(beforeBlocks[1], (uchar *)m_dm.GetScreenData(x+12, y-4, x+26, y+10), size*sizeof(ulong));
    memcpy(beforeBlocks[2], (uchar *)m_dm.GetScreenData(x-6, y-22, x+8, y-8), size*sizeof(ulong));
    memcpy(beforeBlocks[3], (uchar *)m_dm.GetScreenData(x-6, y+14, x+8, y+28), size*sizeof(ulong));

    for (int i=0; i<5; ++i) {
        msleep(50);

        if (m_dm.GetColorNum(x-24, y-4, x-10, y+10, brightColor, 1.0) > brightColorCountMin) {
            count = 0;
            data = (ulong *)m_dm.GetScreenData(x-24, y-4, x-10, y+10);
            for (ulong j=0; j<size; ++j) {
                if (*(data+j) != beforeBlocks[0][j])
                    ++count;
            }
            if (count == size) {
                return true;
            }
        }

        if (m_dm.GetColorNum(x+12, y-4, x+26, y+10, brightColor, 1.0) > brightColorCountMin) {
            count = 0;
            data = (ulong *)m_dm.GetScreenData(x+12, y-4, x+26, y+10);
            for (ulong j=0; j<size; ++j) {
                if (*(data+j) != beforeBlocks[1][j])
                    ++count;
            }
            if (count == size) {
                return true;
            }
        }

        if (m_dm.GetColorNum(x-6, y-22, x+8, y-8, brightColor, 1.0) > brightColorCountMin) {
            count = 0;
            data = (ulong *)m_dm.GetScreenData(x-6, y-22, x+8, y-8);
            for (ulong j=0; j<size; ++j) {
                if (*(data+j) != beforeBlocks[2][j])
                    ++count;
            }
            if (count == size) {
                return true;
            }
        }

        if (m_dm.GetColorNum(x-6, y+14, x+8, y+28, brightColor, 1.0) > brightColorCountMin) {
            count = 0;
            data = (ulong *)m_dm.GetScreenData(x-6, y+14, x+8, y+28);
            for (ulong j=0; j<size; ++j) {
                if (*(data+j) != beforeBlocks[3][j])
                    ++count;
            }
            if (count == size) {
                return true;
            }
        }
    }

    return false;
}

bool DF::getTrophyCoords(int &x, int &y, bool &pickable)
{
    QVariant vx, vy;

    int index = m_dm.FindPic(0, 0, 800, 480, "trophy_pickable.bmp|trophy.bmp", "3F3F3F", 1.0, 1, vx, vy);
    if (index != -1) {
        if (index == 0)
            pickable = true;
        else
            pickable = false;

        x = vx.toInt() + 50;
        y = vy.toInt() + 30;

        return true;
    }

    return false;
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

    if (m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt();
        y = vy.toInt() + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FF00FF-000F00, 0|3|FFFFFF",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt() + 45;
        y = vy.toInt() + 12 + m_roleOffsetY;

        return true;
    }

    if (m_dm.FindMultiColor(0, 100, 800, 400,
                        "FFFFFF", "0|1|FF00FF-000F00, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy)) {
        x = vx.toInt() - 44;
        y = vy.toInt() - 12 + m_roleOffsetY;

        return true;
    }

    return false;
}

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
                sendKey(Stroke, hHeldKey, 20);
                sendKey(Down, hHeldKey, 20);
            }
            if (vHeldKey) {
                if (!hHeldKey) {
                    sendKey(Stroke, m_arrowL, 20);
                    sendKey(Stroke, m_arrowL, 20);
                    sendKey(Stroke, m_arrowR, 20);
                    sendKey(Down, m_arrowR, 20);
                }
                sendKey(Down, vHeldKey, 20);
                if (!hHeldKey) {
                    sendKey(Up, m_arrowR, 20);
                }
            }
        }
    }
}

void DF::pickTrophies(bool &done)
{
    static int counter = 0;
    bool hArrived = false;
    bool vArrived = false;
    int preRoleX = -1;
    int preRoleY = -1;
    int roleX = -1;
    int roleY = -1;
    int hPreDir = 0;
    int vPreDir = 0;
    int hDir = 0;
    int vDir = 0;
    QTime timer;
    QTime stuckTimer;
    uchar preClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};
    int x, y;
    int sectionIndex;
    bool bossRoomArrived;
    bool pickable;

    timer.start();

    // Avoid insisting picking a unpickable item
    if (counter++ > 10) {
        qDebug()<<"pickTrophies: Counter triggered";
        done = true;
        counter = 0;
        return;
    }

    while (true) {
        // Timeout
        if (timer.elapsed() > 20000) {
            done = true;
            break;
        }

        // Check if reached next section
        if (isBlackScreen(0, 0, 50, 50)) {
            // Stop
            moveRole(1, 1);
            hPreDir = vPreDir = 0;

            // Wait until not black screen
            for (int i=0; i<200; ++i) {
                msleep(50);
                if (!isBlackScreen(0, 0, 50, 50)) {
                    break;
                }
            }

            // Get back to last section
            msleep(500);
            rectifySectionIndex(sectionIndex);
            if (navigateSection(sectionIndex, bossRoomArrived)) {
                done = true;
                break;
            } else {
                qDebug()<<"Pick trophies: navigateSection failed";
                throw DFRESTART;
            }
        }

        // Get position of role
        if (!getRoleCoords(roleX, roleY)) {
            if ((hPreDir == 0) && (vPreDir == 0)) {
                if (qrand() % 2 == 0) {
                    moveRole(1, 0, 2);
                    approxSleep(200);
                    moveRole(1, 0);
                } else {
                    moveRole(-1, 0, 2);
                    approxSleep(200);
                    moveRole(-1, 0);
                }
            }
            msleep(10);
            continue;
        }

        // Get postion of trophy
        if (!getTrophyCoords(x, y, pickable)) {
//            qDebug()<<"PickTrophies: No Trophy";
            done = true;
            break;
        }

        // Already stand on trophy
        if (pickable) {
//            qDebug()<<"PickTrophies: Stand On";
            moveRole(1, 1);
            approxSleep(200);
            sendKey(Stroke, "x", 100);
            done = false;
            break;
        }

        // Arrived
        if (hArrived && vArrived) {
//            qDebug()<<"PickTrophies: Arrived";
            sendKey(Stroke, "x", 100);
            done = true;
            break;
        }

        if (((hPreDir != 0) || (vPreDir != 0))  // Moving
            && ((roleX == preRoleX) && (roleY == preRoleY))) {
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
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(preClientBlocks[i], data, 6400);
                }

                // Start timer
                stuckTimer.start();
            } else {
                // Trigger checking every 100 msecs
                if (stuckTimer.elapsed() > 100) {
                    // Get client color blocks
                    for (int i=0; i<10; ++i) {
                        uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                        memcpy(clientBlocks[i], data, 6400);
                    }

                    // Check if role is stucked
                    bool stucked = false;
                    for (int i=0; i<10; ++i) {
                        if (memcmp(clientBlocks[i], preClientBlocks[i], 6400) == 0) {
                            stucked = true;
                            break;
                        }
                    }
                    if (stucked) {
//                        qDebug()<<"PickTrophies: Stuck";
                        done = true;
                        break;
                    }

                    // Save client blocks for checking next time
                    memcpy(preClientBlocks, clientBlocks, 64000);

                    // Restart timer
                    stuckTimer.restart();
                }
            }
        } else {
            // Horizontal moving
            if (!hArrived) {
                hDir = x-roleX;
                if (abs(hDir) <= 5) {
                    moveRole(1, 0);
                    hPreDir = 0;
                    hArrived = true;
                } else if (abs(hDir) <= 20) {
                    if (hPreDir == 0) {
                        moveRole(hDir, 0, 2);
                    } else {
                        moveRole(1, 0);
                        hPreDir = 0;
                    }
                } else {
                    if (hPreDir == 0) {
                        moveRole(hDir, 0, 3);
                        hPreDir = hDir;
                    } else {
                        if (abs(hDir+hPreDir) != abs(hDir)+abs(hPreDir)) {
                            moveRole(1, 0);
                            hPreDir = 0;
                        }
                    }
                }
            }

            // Vertical moving
            if (!vArrived) {
                vDir = y-roleY;
                if (abs(vDir) <= 5) {
                    moveRole(0, 1);
                    vPreDir = 0;
                    vArrived = true;
                } else if (abs(vDir) <= 20) {
                    if (vPreDir == 0) {
                        moveRole(0, vDir, 2);
                    } else {
                        moveRole(0, 1);
                        vPreDir = 0;
                    }
                } else {
                    if (vPreDir == 0) {
                        moveRole(0, vDir, 3);
                        vPreDir = vDir;
                    } else {
                        if (abs(vDir+vPreDir) != abs(vDir)+abs(vPreDir)) {
                            moveRole(0, 1);
                            vPreDir = 0;
                        }
                    }
                }
            }

            // Save position as previous
            preRoleX = roleX;
            preRoleY = roleY;
        }

        msleep(1);
    }

    moveRole(1, 1);
    approxSleep(100);

    if (done)
        counter = 0;
}

bool DF::navigate(int x, int y, bool end)
{
    int checkBlackScreenCount = 0;
    bool hArrived = false;
    bool vArrived = false;
    int preRoleX = -1;
    int preRoleY = -1;
    int roleX = -1;
    int roleY = -1;
    int hPreDir = 0;
    int vPreDir = 0;
    int hDir = 0;
    int vDir = 0;
    QTime timer;
    QTime stuckTimer;
    uchar preClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};
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
            moveRole(1, 1);
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
                if (checkBlackScreenCount++ < 200) {
                    msleep(50);
                    continue;
                }
            }

            // end
            break;
        }

        // Get position
        if (!getRoleCoords(roleX, roleY)) {
            if ((hPreDir == 0) && (vPreDir == 0)) {
                if (qrand() % 2 == 0) {
                    moveRole(1, 0, 2);
                    approxSleep(200);
                    moveRole(1, 0);
                } else {
                    moveRole(-1, 0, 2);
                    approxSleep(200);
                    moveRole(-1, 0);
                }
            }
            msleep(10);
            continue;
        }

        if (((hPreDir != 0) || (vPreDir != 0))  // Moving
            && ((roleX == preRoleX) && (roleY == preRoleY))) {
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
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(preClientBlocks[i], data, 6400);
                }

                // Start timer
                stuckTimer.start();
            } else {
                // Trigger checking every 100 msecs
                if (stuckTimer.elapsed() > 100) {
                    // Get client color blocks
                    for (int i=0; i<10; ++i) {
                        uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                        memcpy(clientBlocks[i], data, 6400);
                    }

                    // Check if role is stucked
                    for (int i=0; i<10; ++i) {
                        if (memcmp(clientBlocks[i], preClientBlocks[i], 6400) == 0) {
                            moveRole(1, 1);
                            hPreDir = vPreDir = 0;
                            return false;
                        }
                    }

                    // Save client blocks for checking next time
                    memcpy(preClientBlocks, clientBlocks, 64000);

                    // Restart timer
                    stuckTimer.restart();
                }
            }
        } else {

            // Horizontal moving
            if (!hArrived) {
                hDir = x-roleX;
                if (abs(hDir) <= 5) {
                    moveRole(1, 0);
                    hPreDir = 0;
                    hArrived = true;
                } else if (abs(hDir) <= 20) {
                    if (hPreDir == 0) {
                        moveRole(hDir, 0, 1);
                    } else {
                        moveRole(1, 0);
                        hPreDir = 0;
                    }
                } else {
                    if (hPreDir == 0) {
                        moveRole(hDir, 0, 3);
                        hPreDir = hDir;
                    } else {
                        if (abs(hDir+hPreDir) != abs(hDir)+abs(hPreDir)) {
                            moveRole(1, 0);
                            hPreDir = 0;
                        }
                    }
                }
            }

            // Vertical moving
            if (!vArrived) {
                vDir = y-roleY;
                if (abs(vDir) <= 5) {
                    moveRole(0, 1);
                    vPreDir = 0;
                    vArrived = true;
                } else if (abs(vDir) <= 20) {
                    if (vPreDir == 0) {
                        moveRole(0, vDir, 1);
                    } else {
                        moveRole(0, 1);
                        vPreDir = 0;
                    }
                } else {
                    if (vPreDir == 0) {
                        moveRole(0, vDir, 3);
                        vPreDir = vDir;
                    } else {
                        if (abs(vDir+vPreDir) != abs(vDir)+abs(vPreDir)) {
                            moveRole(0, 1);
                            vPreDir = 0;
                        }
                    }
                }
            }

            // Save position as previous
            preRoleX = roleX;
            preRoleY = roleY;
        }

        msleep(1);
    }

    approxSleep(100);

    return false;
}

bool DF::navigateSection(int sectionIndex, bool &bossRoomArrived)
{
    bool end = false;

    if (sectionIndex < m_pathList.count()) {
        if (sectionIndex == m_pathList.count()-1) {
            bossRoomArrived = true;
        } else {
            bossRoomArrived = false;
        }

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
        summonSupporter();
        return true;
    }

    bx = vx.toInt() + 50;
    by = vy.toInt() + 150;

//    qDebug()<<rx<<ry<<"|"<<bx<<by;

//    if (abs(ry- by) < 200) {
//        moveRole(0, (by < 350) ? 1 : -1, 2);
//        approxSleep(100);
//    } else {
//        summonSupporter();
//    }

    if (abs(rx- bx) < 250) {
        moveRole((bx < 400) ? 1 : -1, 0, 3);
        approxSleep(100);
    } else {
        summonSupporter();
    }

    return true;
}

