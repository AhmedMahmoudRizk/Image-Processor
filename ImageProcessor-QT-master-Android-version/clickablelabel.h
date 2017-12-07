#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QStack>
#include <QRubberBand>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QGestureEvent;
//class QPanGesture;
class QPinchGesture;
//class QSwipeGesture;
QT_END_NAMESPACE

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
    int x,y;
    QPoint offset;
    bool selected;
    bool rotate;
    QRubberBand* rubberBand;
    void grabGestures(const QList<Qt::GestureType> &gestures);
    QImage currentImage;
    QRect defaultRect;


    qreal horizontalOffset;
    qreal verticalOffset;
    qreal rotationAngle;
    qreal scaleFactor;
    qreal currentStepScaleFactor;

    QStack<QImage> currentImage1;
    QStack<QRect> labelRec1;
    QStack<qreal> horizontalOffset1;
    QStack<qreal> verticalOffset1;
    QStack<qreal>rotationAngle1;
    QStack<qreal>scaleFactor1;
    QStack<qreal> currentStepScaleFactor1;


    QStack<QImage> currentImage2;
    QStack<QRect> labelRec2;
    QStack<qreal> horizontalOffset2;
    QStack<qreal> verticalOffset2;
    QStack<qreal>rotationAngle2;
    QStack<qreal>scaleFactor2;
    QStack<qreal> currentStepScaleFactor2;


    void Reset();
    void addToStack();


signals:
    void clicked();
    void moved();
    void released();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    bool gestureEvent(QGestureEvent *event);
    void panTriggered(QPanGesture*);
    void pinchTriggered(QPinchGesture*);


};

#endif // CLICKABLELABEL_H
