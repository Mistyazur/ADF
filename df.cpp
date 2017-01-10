#include "df.h"

#include <QDebug>
#include <QTime>


DF::DF()
{
    // Default arrow keys
    m_arrowL = 37;
    m_arrowU = 38;
    m_arrowR = 39;
    m_arrowD = 40;

    // Set mouse and key delay
    setMouseDelayDelta(0.2);
    setKeyDelayDelta(0.2);
}

int DF::window()
{
    return m_dm.FindWindow("地下城与勇士", "地下城与勇士");
}

bool DF::bind(bool underground)
{
    bool ret = false;

    int hWnd = window();
    if (0 == hWnd)
        return ret;

    if (underground)
        ret = m_dm.BindWindow(hWnd, "dx2", "dx", "dx", 101);
    else
        ret = m_dm.BindWindow(hWnd, "normal", "normal", "normal", 101);

    approxSleep(2000);
    return ret;
}

void DF::unbind()
{
    m_dm.UnBindWindow();
}

void DF::setArrowKey(int left, int up, int right, int down)
{
    m_arrowL = left;
    m_arrowU = up;
    m_arrowR = right;
    m_arrowD = down;
}

void DF::switchRole(int index)
{
    const int originalX = 128;
    const int originalY = 190;
    const int offsetX = 120;
    const int offsetY = 210;
    int row = index%4;
    int column = index/4;

    // Reset scroll bar
    sendMouse(LeftDown, 580, 290, 500);
    sendMouse(Move, 580, 100, 500);
    sendMouse(LeftUp, -1, -1, 500);

    // Scroll
    if (column > 1) {
        column = 1;
        for (int i=0; i < column-1; ++i)
            sendMouse(Left, 580, 495, 1000);
    }

    // Pick role
    int roleX = originalX+offsetX*row;
    int roleY = originalY+offsetY*column;
    sendMouse(Left, roleX, roleY, 1000);
    sendMouse(Left, roleX, roleY, 1000);

    // Game start
    sendMouse(Left, 400, 550, 1000);
    sendMouse(Left, 400, 550, 1000);
}

void DF::teleport(const QString &destination)
{
    QVariant x, y;

    // Check current location
    if (m_dm.FindPic(CLIENT_RECT, destination, "000000", 1.0, 0, x, y) != -1)
        return;

    // Teleport
}

void DF::navigateOnMap(int x, int y)
{
    sendKey(Stroke, "N", 1000);
    sendMouse(Right, x, y, 1000);
    sendKey(Stroke, "N", 1000);
}

bool DF::initRoleOffset()
{
    QVariant vx, vy;
    if (!m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy))
        return false;

    m_roleOffsetY = 442 - vy.toInt();
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
    sendKey(Stroke, 32, 500);

    // Wait
    for (int i = 0; i < 10; ++i) {
        if (dungeonIn())
            return true;
        msleep(1000);
    }

    return false;
}

bool DF::dungeonIn()
{
    QVariant x, y;

    if (m_dm.FindPic(770, 0, 800, 30, "dungeon_in.bmp", "000000", 1.0, 0, x, y) != -1) {
        return true;
    }

    return false;
}

bool DF::dungeonEnd()
{
    QVariant x, y;

    if (m_dm.FindPic(CLIENT_RECT, "dungeon_end.bmp", "000000", 0.9, 2, x, y) == -1)
        return false;

    moveRole(1, 1);

    approxSleep(3000, 0.5);

    // Pick trophies
    sendKey(Stroke, 189, 600);  // -
    sendKey(Down, "x", 3000);
    sendKey(Up, "x");

    while (m_dm.FindPic(CLIENT_RECT, "sell.bmp", "000000", 1.0, 0, x, y) == -1) {
        approxSleep(1000);
    }

    // Reenter
    sendKey(Stroke, 27, 1500);
    sendKey(Stroke, 121, 200);

    // Wait
    for (int i = 0; i < 20; ++i) {
        if (dungeonIn()) {
            return true;
        }
        msleep(1000);
    }

    qDebug()<<"error: restart game";

    return true;
}

bool DF::summonSupporter()
{
//    if (m_dm.GetColor(695, 510).toUpper() == "7F7B35") {
        sendKey(Stroke, 9, 100);
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

bool DF::isSectionClear(int x1, int y1, int x2, int y2,
                    const QString &brightColor,
                    bool isFirstSection)
{
    static QTime *t = nullptr;
    int x, y;
    ulong beforeBlocks[4][196] = {0};

    // First section maybe not has clear effect
    // So we assume it's clear, if it has costed 30 secs
    if (isFirstSection) {
        if (!t) {
            t = new QTime();
            t->start();
        } else {
            if (t->elapsed() > 30000)
                return true;
        }
    } else {
        if (t) {
            delete t;
            t = nullptr;
        }
    }

    if (!getRoleCoordsInMap(x1, y1, x2, y2, x, y))
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
        msleep(20);

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

bool DF::getTrophyCoords(int &x, int &y, bool &standOn)
{
    QVariant vx, vy;

    int index = m_dm.FindPic(0, 0, 800, 480, "trophy_pickable.bmp|trophy.bmp", "3F3F3F", 1.0, 0, vx, vy);
    if (index != -1) {
        if (index == 0)
            standOn = true;
        else
            standOn = false;

        x = vx.toInt() + 50;
        y = vy.toInt() + 40;

        return true;
    }

    return false;
}

bool DF::pickTrophies()
{
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
    QTime blockTimer;
    QTime timeoutTimer;
    uchar preClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};
    QVariant vx, vy;
    int x, y;
    bool standOn;

    timeoutTimer.start();

    while (true) {
        if (timeoutTimer.elapsed() > 10000)
            break;
        if (hArrived && vArrived) {
            moveRole(1, 1);
            approxSleep(200);
            sendKey(Stroke, "x", 200);

            return true;
        }

        // Get position
        if (!getRoleCoords(roleX, roleY)) {
            continue;
        }
        if (!getTrophyCoords(x, y, standOn)) {
            break;
        }
        if (standOn) {
            moveRole(1, 1);
            approxSleep(200);
            sendKey(Stroke, "x", 200);

            return true;
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

            if (blockTimer.isNull()) {
                // Get client color blocks
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(preClientBlocks[i], data, 6400);
                }

                // Start timer
                blockTimer.start();
            } else {
                // Trigger checking every 100 msecs
                if (blockTimer.elapsed() > 100) {
                    // Get client color blocks
                    for (int i=0; i<10; ++i) {
                        uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                        memcpy(clientBlocks[i], data, 6400);
                    }

                    // Check if role is stucked
                    for (int i=0; i<10; ++i) {
                        if (memcmp(clientBlocks[i], preClientBlocks[i], 6400) == 0) {
                            moveRole(1, 1);
                            return false;
                        }
                    }

                    // Save client blocks for checking next time
                    memcpy(preClientBlocks, clientBlocks, 64000);

                    // Restart timer
                    blockTimer.restart();
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
                } else {
                    if (hPreDir == 0) {
                        moveRole(hDir, 0, 2);
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
                } else {
                    if (vPreDir == 0) {
                        moveRole(0, vDir, 2);
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

    return false;
}

bool DF::getRoleCoordsInMap(int x1, int y1, int x2, int y2, int &x, int &y)
{
    QVariant vx, vy;

    if (m_dm.FindPic(x1, y1, x2, y2, "dungeon_map_role.bmp", "101010", 1.0, 0, vx, vy) == -1) {
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
    uchar preClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};
    QVariant vx, vy;

    if (-1 == x)
        hArrived = true;
    if (-1 == y)
        vArrived = true;

    while (true) {
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
            continue;
        }
//        qDebug()<<"Pos: "<<roleX<<roleY;

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

            if (timer.isNull()) {
                // Get client color blocks
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(preClientBlocks[i], data, 6400);
                }

                // Start timer
                timer.start();
            } else {
                // Trigger checking every 100 msecs
                if (timer.elapsed() > 100) {
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
                    timer.restart();
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

    return false;
}

