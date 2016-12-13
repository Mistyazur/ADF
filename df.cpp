#include "df.h"

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
            sendKey(Down, hKeyHolding);
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


void DF::navigate(int x, int y)
{
    bool arrivedX = false;
    bool arrivedY = false;
    int roleX = -1;
    int roleY = -1;
    int prevRoleX = -1;
    int prevRoleY = -1;
    int directionX = 0;
    int directionY = 0;
    int speedX = -1;

    if (-1 == x)
        arrivedX = true;
    if (-1 == y)
        arrivedY = true;
    
    while (!arrivedX || !arrivedY) {
        // Get position
        if (!getRoleCoords(x, y))
            continue;
        
        if ((roleX == prevRoleX) && (roleY == prevRoleY)) {

        } else {
            // Horizontal moving
            if (!arrivedX) {
                int absOffsetX = abs(x-roleX);
                if (-1 == speedX) {
                    if (absOffsetX > 20) {
                        speedX = 3;
                        directionX = x - roleX;
                        moveRole(directionX, 0, speedX);
                    } else if (absOffsetX > 5) {
                        speedX = 2;
                        directionX = x - roleX;
                        moveRole(directionX, 0, speedX);
                    } else {
                        arrivedX = true;
                    }
                } else {
                    if (absOffsetX <= 5) {
                        if (speedX >= 2)
                            stopRole(directionX);
                        arrivedX = true;
                    } else if (absOffsetX <= 20) {
                        if (speedX == 3) {
                            // Stop
                            stopRole(directionX);
                            // Move at speed 2
                            speedX = 2;
                            directionX = x - roleX;
                            moveRole(directionX, 0, speedX);
                        }
                    }
                }
            }

            // Vertical move
            if (!arrivedY) {
                int absOffsetY = abs(y-roleY);
                if (absOffsetY > 5) {
                    if (directionY == 0) {
                        directionY = y - roleY;
                        moveRole(0, directionY, 2);
                    } else {
                        if (directionY != 0)
                            stopRole(0, directionY);
                        arrivedY = true;
                    }
                }
            }

            // Save position as previous
            prevRoleX = roleX;
            prevRoleY = roleY;
        }
    }
}

