#include "clickablelabel.h"
#include <QtWidgets>
#include <QStack>

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent),
      horizontalOffset(0),
      verticalOffset(0),
      rotationAngle(0),
      scaleFactor(1),
      currentStepScaleFactor(1)
{
    selected = false;
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rotate = false;
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}


ClickableLabel::~ClickableLabel() {

}
void ClickableLabel::grabGestures(const QList<Qt::GestureType> &gestures)//request using gestures
{
    //! [enable gestures]
    foreach (Qt::GestureType gesture, gestures)
        grabGesture(gesture);
    //! [enable gestures]
}

void ClickableLabel::Reset()
{
    rotationAngle = 0;
    scaleFactor = 1;
    currentStepScaleFactor = 1;
    verticalOffset = 0;
    horizontalOffset = 0;
}

void ClickableLabel::addToStack()
{
    currentImage1.push(currentImage);
    labelRec1.push(defaultRect);
    horizontalOffset1.push(horizontalOffset);
    verticalOffset1.push(verticalOffset);
    rotationAngle1.push(rotationAngle);
    scaleFactor1.push(scaleFactor);
    currentStepScaleFactor1.push(currentStepScaleFactor);

}


////! [event handler]
bool ClickableLabel::event(QEvent *event)
{

    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QLabel::event(event);
}
//! [event handler]

void ClickableLabel::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    const qreal iw = currentImage.width();
    const qreal ih = currentImage.height();
    const qreal wh = height();
    const qreal ww = width();

    p.translate(ww/2, wh/2);
    p.translate(horizontalOffset, verticalOffset);
    p.rotate(rotationAngle);
    p.scale(currentStepScaleFactor * scaleFactor, currentStepScaleFactor * scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawImage(0, 0, currentImage);
    this->setPixmap(QPixmap::fromImage(currentImage));
}
//! [paint method]

//! [gesture event handler]
bool ClickableLabel::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pan = event->gesture(Qt::PanGesture))
        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}
//! [gesture event handler]

void ClickableLabel::panTriggered(QPanGesture *gesture)
{
#ifndef QT_NO_CURSOR
    switch (gesture->state()) {
        case Qt::GestureStarted:
        case Qt::GestureUpdated:
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
    }
#endif
    QPointF delta = gesture->delta();
    horizontalOffset += delta.x();
    verticalOffset += delta.y();
    update();
}

//! [pinch function]
void ClickableLabel::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (rotate) {
        qreal rotationDelta = gesture->rotationAngle() - gesture->lastRotationAngle();
        rotationAngle += rotationDelta;
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged & !rotate) {
        currentStepScaleFactor = gesture->totalScaleFactor();
    }
    if (gesture->state() == Qt::GestureFinished ) {
        if(rotate)
        {
        scaleFactor *= currentStepScaleFactor;
        currentStepScaleFactor = 1;
        //create structure and add to stack
       addToStack();
        }
        rotate = false;

    }

    update();
}
//! [pinch function]



void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *)
{
    Reset();
    addToStack();
    update();
}
void ClickableLabel::resizeEvent(QResizeEvent*)
{
    update();
}
void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    offset = event->pos();
    if(selected){
        qInfo() << "pressed```";

    if (!rubberBand){
        qInfo() << "pressed";


    }
    qInfo() << "pressed";

        rubberBand->setGeometry(QRect(offset, QSize()));
        rubberBand->show();

    }
    emit clicked();
}
void ClickableLabel::mouseMoveEvent(QMouseEvent* event) {

    if(selected){
        rubberBand->setGeometry(QRect(offset, event->pos()).normalized());
        qInfo() << "moved";
    }
    emit moved();
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit released();
}

