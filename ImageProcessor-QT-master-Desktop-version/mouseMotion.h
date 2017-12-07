#ifndef MOUSE_MOTION_H
#define MOUSE_MOTION_H

#include <QLabel>

#include <QEvent>
#include <QDebug>

class mouseMotion : public QLabel
{
    Q_OBJECT

public:
    explicit  mouseMotion(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    int x,y;

signals:
    void mouse_Pressed();
    void mouse_Pos();
    void mouse_Released();

public slots:



#endif // RENDERAREA_H
