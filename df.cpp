#include "df.h"

#include <QDebug>

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

bool DF::getRoleCoords(int &x, int &y)
{
    QVariant vx, vy;
    if (!m_dm.FindMultiColor(0, 0, 800, 600,
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
    int speedX = -1;
    int counter = 0;
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
            qDebug()<<"Failed";
            continue;
        }

        if ((roleX == prevRoleX) && (roleY == prevRoleY)) {
            if (0 == (counter % 20)) {
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
            }

            counter++;
        } else {
            // Horizontal moving
            if (!arrivedX) {
                int absOffsetX = abs(x-roleX);
                if (-1 == speedX) {
                    if (absOffsetX > 20) {
                        speedX = 3;
                        hDirection = x - roleX;
                        moveRole(hDirection, 0, speedX);
                    } else if (absOffsetX > 5) {
                        speedX = 2;
                        hDirection = x - roleX;
                        moveRole(hDirection, 0, speedX);
                    } else {
                        arrivedX = true;
                    }
                } else {
                    if (absOffsetX <= 5) {
                        if (speedX >= 2) {
                            qDebug()<<1<<hDirection;
                            stopRole(hDirection);
                        }
                        arrivedX = true;
                    } else if (absOffsetX <= 20) {
                        if (speedX == 3) {
                            // Stop
                            stopRole(hDirection);
                            // Move at speed 2
                            speedX = 2;
                            hDirection = x - roleX;
                            moveRole(hDirection, 0, speedX);
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
                    if (vDirection != 0)
                        stopRole(0, vDirection);
                    arrivedY = true;
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

