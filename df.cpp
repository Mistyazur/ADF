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
}

int DF::window()
{
    return m_dm.FindWindow("地下城与勇士", "地下城与勇士");
}

bool DF::bind(bool underground)
{
    int hWnd = window();
    if (0 == hWnd)
        return false;

    if (underground)
        return m_dm.BindWindow(hWnd, "dx2", "dx", "dx", 0);

    return m_dm.BindWindow(hWnd, "normal", "normal", "normal", 0);
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
    sendMouse(Left, x, y, 1000);
    sendKey(Stroke, "N", 1000);
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
        sendKey(Stroke, m_arrowU, 500);

    // Pick difficulty
    for (int i=0; i<4; ++i)
        sendKey(Stroke, m_arrowL, 500);
    for (int i=0; i<difficulty; ++i)
        sendKey(Stroke, m_arrowR, 500);

    // Commit
    sendKey(Stroke, 32, 500);
    sendKey(Stroke, 32, 500);

    return true;
}

bool DF::reenterDungeon()
{
    QVariant x, y;

    if (m_dm.FindPic(CLIENT_RECT, "dungeon_end_cmd.bmp", "000000", 1.0, 2, x, y) == -1)
        return false;

    sendKey(Stroke, 27, 500);
    sendKey(Stroke, 121, 500);

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

//bool DF::isSectionClear(bool isFirstSection)
//{
//    static QTime *t = nullptr;
//    QVariant x, y;

//    // First section maybe not has clear effect
//    // So we assume it's clear, if it has costed 30 secs
//    if (isFirstSection) {
//        if (!t) {
//            t = new QTime();
//            t->start();
//        } else {
//            if (t->elapsed() > 30000)
//                return true;
//        }
//    } else {
//        if (t) {
//            delete t;
//            t = nullptr;
//        }
//    }

//    // Check clear effect
//    if (m_dm.FindPic(720, 45, 800, 105, "section_clear.bmp", "000000", 0.9, 0, x, y) == -1)
//        return false;

//    return true;
//}

bool DF::isSectionClear(bool isFirstSection)
{
    static QTime *t = nullptr;
    QVariant vx, vy;
    int x, y;
    uchar beforeBlocks[4][160] = {0};
    uchar afterBlocks[4][160] = {0};

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

    // Check clear effect
    if (m_dm.FindPic(600, 45, 800, 200, "dungeon_map_role.bmp", "000000", 0.9, 0, vx, vy) == -1) {
        qDebug()<<"isSectionClear: Failed role on map";
        return false;
    }

    x = vx.toInt();
    y = vy.toInt();
//    qDebug()<<x<<y;

    QString aveColor = m_dm.GetAveRGB(x-4, y+10, x-2, y+12);
//    qDebug()<<aveColor;
//    qDebug()<<m_dm.GetColorNum(x-24, y-4, x-10, y+10, aveColor+"-070707", 1.0);
//    qDebug()<<m_dm.GetColorNum(x+12, y-4, x+26, y+10, aveColor+"-070707", 1.0);
//    qDebug()<<m_dm.GetColorNum(x-6, y-22, x+8, y-8, aveColor+"-070707", 1.0);
//    qDebug()<<m_dm.GetColorNum(x-6, y+14, x+8, y+28, aveColor+"-070707", 1.0);

    if (m_dm.GetColorNum(x-24, y-4, x-10, y+10, aveColor+"-070707", 1.0) > 15)
        return true;
    if (m_dm.GetColorNum(x+12, y-4, x+26, y+10, aveColor+"-070707", 1.0) > 15)
        return true;
    if (m_dm.GetColorNum(x-6, y-22, x+8, y-8, aveColor+"-070707", 1.0) > 15)
        return true;
    if (m_dm.GetColorNum(x-6, y+14, x+8, y+28, aveColor+"-070707", 1.0) > 15)
        return true;

    return false;
}

bool DF::getRoleCoords(int &x, int &y)
{
    QVariant vx, vy;
    if (!m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000F00", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000F00",
                        1.0, 0,
                        vx, vy))
        return false;

    x = vx.toInt();
    y = vy.toInt();

    return true;
}

void DF::hideDropName(bool enable)
{
    static bool enabled = false;

    if (enabled) {
        if (!enable) {
            sendKey(Down, "/", 300);
            sendKey(Up, "/", 30);
            enabled = false;
        }
    } else {
        if (enable) {
            sendKey(Down, "/", 300);
            sendKey(Up, "/", 30);
            enabled = true;
        }
    }
}

void DF::moveRole(int horizontal, int vertical, int speed)
// speed :  1 - click direction key
//          2 - hold direction key
//          3 - double click then hold direction key
{
    int hKeyHolding = 0;
    int vKeyHolding = 0;

    if (0 != horizontal) {
        hKeyHolding = (horizontal > 0) ? m_arrowR : m_arrowL;
        switch (speed) {
        case 1:
            sendKey(Stroke, hKeyHolding);
            break;
        case 2:
            sendKey(Down, hKeyHolding);
            break;
        case 3:
            sendKey(Stroke, hKeyHolding, 30);
            sendKey(Down, hKeyHolding);
            break;
        }
    }

    if (0 != vertical) {
        vKeyHolding = (vertical > 0) ? m_arrowD : m_arrowU;
        switch (speed) {
        case 1:
            sendKey(Stroke, vKeyHolding);
            break;
        case 2:
        case 3:
            sendKey(Down, vKeyHolding);
            break;
        }
    }
}

void DF::stopRole(int horizontal, int vertical)
{
    if (horizontal > 0) {
        while (m_dm.GetKeyState(m_arrowR) == 1)
            sendKey(Up, m_arrowR);
    } else if (horizontal < 0) {
        while (m_dm.GetKeyState(m_arrowL) == 1)
            sendKey(Up, m_arrowL);
    }
    if (vertical > 0) {
        while (m_dm.GetKeyState(m_arrowD) == 1)
            sendKey(Up, m_arrowD);
    } else if (vertical < 0) {
        while (m_dm.GetKeyState(m_arrowU) == 1)
            sendKey(Up, m_arrowU);
    }

    mdsleep(100);
}

bool DF::navigate(int x, int y)
{
    bool hArrived = false;
    bool vArrived = false;
    int hDirection = 0;
    int vDirection = 0;
    int roleX = -1;
    int roleY = -1;
    int preRoleX = -1;
    int preRoleY = -1;
    int vOffset = 0;
    int vAbsOffset = 0;
    int hOffset = 0;
    int hAbsOffset = 0;
    QTime timer;
    uchar preClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};

    if (-1 == x)
        hArrived = true;
    if (-1 == y)
        vArrived = true;

    while (!hArrived || !vArrived) {
        // Check if reached next section
        int blackCount = m_dm.GetColorNum(0, 0, 50, 50, "000000", 1.0);
        if (blackCount > 200) {
            if (0 != hDirection)
                stopRole(hDirection);
            if (0 != vDirection)
                stopRole(0, vDirection);

            for (int i=0; i<100; ++i) {
                blackCount = m_dm.GetColorNum(0, 0, 50, 50, "000000", 1.0);
                if (blackCount <= 200)
                    return true;
                msleep(100);
            }
            qDebug()<<"black screen too long";
        }

        // Get position
        if (!getRoleCoords(roleX, roleY)) {
//            qDebug()<<"Failed get coords";
            continue;
        }

        if ((roleX == preRoleX) && (roleY == preRoleY)) {
            // Start timer
            if (timer.isNull()) {
                timer.start();
                continue;
            }

            // Trigger checking every 300 msecs
            if (timer.elapsed() > 300) {
                // Get client color blocks
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(clientBlocks[i], data, 6400);
                }

                // Check if role is stucked
                for (int i=0; i<10; ++i) {
                    if (memcmp(clientBlocks[i], preClientBlocks[i], 6400) == 0) {
                        if (0 != hDirection)
                            stopRole(hDirection);
                        if (0 != vDirection)
                            stopRole(0, vDirection);
                        return false;
                    }
                }

                // Save client blocks for checking next time
                memcpy(preClientBlocks, clientBlocks, 64000);

                // Restart timer
                timer.restart();
            }
        } else {
            // Horizontal moving
            if (!hArrived) {
                hOffset = x-roleX;
                hAbsOffset = abs(hOffset);

                if (hAbsOffset <= 5) {
                    hArrived = true;
                } else if (hAbsOffset <= 20) {
                    if (hDirection == 0) {
                        moveRole(hOffset, 0, 1);
                    } else {
                        stopRole(hDirection);
                        hDirection = 0;
                    }
                } else {
                    if (hDirection == 0) {
                        hDirection = hOffset;
                        moveRole(hDirection, 0, 3);
                    } else {
                        if (abs(hOffset+hDirection) != hAbsOffset+abs(hDirection)) {
                            stopRole(hDirection);
                            hDirection = 0;
                        }
                    }
                }
            }

            // Vertical moving
            if (!vArrived) {
                vOffset = y-roleY;
                vAbsOffset = abs(vOffset);

                if (vAbsOffset <= 5) {
                    vArrived = true;
                } else if (vAbsOffset <= 20) {
                    if (vDirection == 0) {
                        moveRole(0, vOffset, 1);
                    } else {
                        stopRole(0, vDirection);
                        vDirection = 0;
                    }
                } else {
                    if (vDirection == 0) {
                        vDirection = vOffset;
                        moveRole(0, vDirection, 2);
                    } else {
                        if (abs(vOffset+vDirection) != vAbsOffset+abs(vDirection)) {
                            stopRole(0, vDirection);
                            vDirection = 0;
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

