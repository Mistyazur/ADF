#ifndef DF_H
#define DF_H

#include "DmPlugin/dmprivate.h"

class DF : public DmPrivate
{
public:
    DF();
protected:
    int m_arrowL;
    int m_arrowU;
    int m_arrowR;
    int m_arrowD;

    int window();
    bool bind(bool underground=false);
    
    void setArrowKey(int left, int up, int right, int down);
    void navigate(int x=-1, int y=-1);
    bool getRoleCoords(int &x, int &y);
    void moveRole(int horizontal=0, int vertical=0, int speed=0);
    void stopRole(int horizontal=0, int vertical=0);
};

#endif // DF_H
