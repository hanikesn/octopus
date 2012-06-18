#include "gui/timeline.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

const qint64 TimeLine::LARGETICKAMOUNT = 2000000;  // two seconds in µs
const qint64 TimeLine::MEDIUMTICKAMOUNT = 1000000; // one second in µs
const qint64 TimeLine::SMALLTICKAMOUNT = 200000; // 0,2 seconds in µs
const qint64 TimeLine::TIMEPERPX = 40000;  /* Amount of time which one pixel represents
                                              40 milliseconds in µs*/

TimeLine::TimeLine(int offset, QGraphicsItem * parent, Qt::WindowFlags wFlags):
    QGraphicsWidget(parent, wFlags),
    offset(offset),
    beginRange(0),    
    textBoxWidth(50),
    textBoxHeight(10),
    shortTickHeight(5),
    mediumTickHeight(10),
    largeTickHeight(15),
    pen(Qt::black, 1, Qt::SolidLine)
{
    setGeometry(0, 0, 946, 50);
}

void TimeLine::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipRect(boundingRect());
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QRectF frame(QPointF(0,0), geometry().size());
    QGradientStops stops;

    // paint a background rect (with gradient)
    QLinearGradient gradient(frame.topLeft(), frame.topLeft() + QPointF(200,200));
    stops << QGradientStop(0.0, QColor(60, 60,  60));
    stops << QGradientStop(frame.height()/2/frame.height(), QColor(150, 150, 150));    
    stops << QGradientStop(1.0, QColor(215, 215, 215));
    gradient.setStops(stops);

    painter->setBrush(QBrush(gradient));
    painter->setPen(pen);
    painter->drawRect(frame);

    // draw ticks:
    drawTicks(painter);
}

QRectF TimeLine::boundingRect() const
{
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void TimeLine::drawTicks(QPainter *painter)
{
    currentPos = 0;
    qint64 currentTime = rangeOffset;
    currentTime -= currentTime%TIMEPERPX;

    bottom = geometry().height() - 10;
    /*
      2 seconds: large tick
      1 second : medium tick
      0,2 seconds: small tick
      */
    double output  = 0.0;
    while (currentPos < geometry().width()) {
        if (currentTime % LARGETICKAMOUNT == 0) {           
            output = currentTime/1000000;
            // large tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - largeTickHeight);
            QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth,
                               textBoxHeight);
            painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(output, 0, 'f', 0));


        } else if (currentTime % MEDIUMTICKAMOUNT == 0) {
            // medium tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - mediumTickHeight);
        } else if (currentTime % SMALLTICKAMOUNT == 0) {
            // small tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - shortTickHeight);
        }

        currentTime += TIMEPERPX;
        currentPos++;
    }
}

void TimeLine::drawFrom(qint64 time)
{
//    if (rangeOffset == time) return;

    rangeOffset = time;
    update(boundingRect());
}

void TimeLine::setOffset(int offset)
{
    this->offset = offset;
    update(boundingRect());
}

qint64 TimeLine::getUpperEnd(qint64 lowerEnd)
{
    beginRange = lowerEnd;
    return beginRange + ((geometry().width()-offset)*TIMEPERPX);
}

//TODO(domi): %LARGETI... geht nur mit Zahlen die vielfache von LARGETICKAMOUNT sind
