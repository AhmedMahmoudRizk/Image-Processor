#include "mouseMotion.h"


mouseMotion:: mouseMotion(QWidget *parent):
    QLabel(parent)
{
}

void mouseMotion::mouseMoveEvent(QMouseEvent *ev)
{
    this->x = ev->x();
    this->y = ev->y();
    emit mouse_Pos();
}

void mouseMotion::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_Pressed();
}

void mouseMotion::mouseReleaseEvent(QMouseEvent *ev)
{
    emit mouse_Released();
}


