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
    int roleX = originX+offsetX*row;
    int roleY = originY+offsetY*column;
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
        if (m_dm.FindPic(CLIENT_RECT, "back_to_town.bmp", "000000", 1.0, 0, x, y) != -1) {
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

bool DF::isSectionClear()
{
    QVariant x, y;

    if (m_dm.FindPic(720, 45, 800, 105, "section_clear.bmp", "2F4F3F", 1.0, 0, x, y) == -1)
        return false;

    return true;
}

bool DF::getRoleCoords(int &x, int &y)
{
    QVariant vx, vy;
    if (!m_dm.FindMultiColor(0, 100, 800, 400,
                        "FF00FF-000100", "0|1|FFFFFF, 0|2|FFFFFF, 0|3|FF00FF-000100",
                        1.0, 0,
                        vx, vy))
        return false;

    x = vx.toInt();
    y = vy.toInt();

    return true;
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
    if (horizontal > 0)
        sendKey(Up, m_arrowR);
    else if (horizontal < 0)
        sendKey(Up, m_arrowL);
    if (vertical > 0)
        sendKey(Up, m_arrowD);
    else if (vertical < 0)
        sendKey(Up, m_arrowU);

    mdsleep(200);
}

bool DF::navigate(int x, int y)
{
    bool arrivedX = false;
    bool arrivedY = false;
    int roleX = -1;
    int roleY = -1;
    int prevRoleX = -1;
    int prevRoleY = -1;
    int hDirection = 0;
    int vDirection = 0;
    int speed = 0;
    QTime timer;
    uchar prevClientBlocks[10][6400] = {0};
    uchar clientBlocks[10][6400] = {0};

    if (-1 == x)
        arrivedX = true;
    if (-1 == y)
        arrivedY = true;

    while (!arrivedX || !arrivedY) {
        // Check if reached next section
        int blackCount = m_dm.GetColorNum(0, 0, 50, 50, "000000", 1.0);
        if (blackCount > 200) {
            if (0 != hDirection)
                stopRole(hDirection);
            if (0 != vDirection)
                stopRole(0, vDirection);
            return true;
        }

        // Get position
        if (!getRoleCoords(roleX, roleY)) {
            continue;
        }

        if ((roleX == prevRoleX) && (roleY == prevRoleY)) {
            // Start timer
            if (timer.isNull()) {
                timer.start();
                continue;
            }

            // Trigger checking every 500 msecs
            if (timer.elapsed() > 500) {
                // Get client color blocks
                for (int i=0; i<10; ++i) {
                    uchar *data = (uchar *)m_dm.GetScreenData(i*40, 0, i*40+40, 40);
                    memcpy(clientBlocks[i], data, 6400);
                }

                // Check if role is stucked
                for (int i=0; i<10; ++i) {
                    if (memcmp(clientBlocks[i], prevClientBlocks[i], 6400) == 0) {
                        if (0 != hDirection)
                            stopRole(hDirection);
                        if (0 != vDirection)
                            stopRole(0, vDirection);
                        return false;
                    }
                }

                // Save client blocks for checking next time
                memcpy(prevClientBlocks, clientBlocks, 64000);

                // Restart timer
                timer.restart();
            }
        } else {
            // Horizontal moving
            if (!arrivedX) {
                int absOffsetX = abs(x-roleX);
                if (0 == speed) {
                    if (absOffsetX > 20) {
                        speed = 3;
                        hDirection = x-roleX;
                        moveRole(hDirection, 0, speed);
                    } else if (absOffsetX > 5) {
                        speed = 2;
                        hDirection = x-roleX;
                        moveRole(hDirection, 0, speed);
                    } else {
                        arrivedX = true;
                    }
                } else {
                    if (absOffsetX <= 5) {
                        arrivedX = true;
                        if (speed >= 2)
                            stopRole(hDirection);
                    } else if (absOffsetX <= 20) {
                        if (speed == 3) {
                            // Stop
                            stopRole(hDirection);

                            // Move at speed 2
                            speed = 2;
                            hDirection = x-roleX;
                            moveRole(hDirection, 0, speed);
                        }
                    }
                }
            }

            // Vertical move
            if (!arrivedY) {
                int absOffsetY = abs(y-roleY);
                if (absOffsetY > 5) {
                    if (vDirection == 0) {
                        vDirection = y - roleY;
                        moveRole(0, vDirection, 2);
                    }
                } else {
                    arrivedY = true;
                    if (vDirection != 0)
                        stopRole(0, vDirection);
                }
            }

            // Save position as previous
            prevRoleX = roleX;
            prevRoleY = roleY;
        }

        msleep(10);
    }

    return false;
}

