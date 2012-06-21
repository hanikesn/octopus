#include "gui/timeline.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

TimeLine::TimeLine(int offset, QGraphicsItem * parent, Qt::WindowFlags wFlags):
    QGraphicsWidget(parent, wFlags),
    offset(offset),
    beginRange(0),    
    textBoxWidth(50),
    textBoxHeight(10),
    shortTickHeight(5),
    mediumTickHeight(10),
    largeTickHeight(15),
    pen(Qt::black, 1, Qt::SolidLine),
    largeTickAmount(2000000), // two seconds in µs
    mediumTickAmount(1000000), // one second in µs
    smallTickAmount(200000), // 0,2 seconds in µs
    timePerPx(40000), // Amount of time which one pixel represents 40 milliseconds in µs
    timeRepresentation(SECOND_FULL)
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

    QRect rect = QRect(5, geometry().height() - 15, 50, 15);
    QString text = timeRepresentation == MILLISECOND ? "ms" : "sec";
    painter->drawText(rect, text);
}

QRectF TimeLine::boundingRect() const
{
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void TimeLine::drawTicks(QPainter *painter)
{    
    currentPos = 0;
    qint64 currentTime = rangeOffset;
    currentTime -= currentTime%timePerPx;

    bottom = geometry().height() - 10;
    /* default:
      2 seconds: large tick
      1 second : medium tick
      0,2 seconds: small tick
      */
    double timeFactor = 0;
    int digitsAfterPoint = 0;
    if (timeRepresentation == SECOND_FULL)
        timeFactor = 1000000;
    else if (timeRepresentation == SECOND_PART) {
        timeFactor = 1000000;
        digitsAfterPoint = 2;
    } else
        timeFactor = 1000;

    double output  = 0.0;
    while (currentPos < geometry().width()) {
        if (currentTime % largeTickAmount == 0) {
            output = (double)currentTime / timeFactor;
            // large tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - largeTickHeight);
            QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth,
                               textBoxHeight);
            painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(output, 0, 'f',
                                                                       digitsAfterPoint));


        } else if (currentTime % mediumTickAmount == 0) {
            // medium tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - mediumTickHeight);
        } else if (currentTime % smallTickAmount == 0) {
            // small tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - shortTickHeight);
        }

        currentTime += timePerPx;
        currentPos++;
    }
}

void TimeLine::drawFrom(qint64 time)
{
    if (rangeOffset == time) return;

    rangeOffset = time;
    update(boundingRect());
}

void TimeLine::onOffsetChanged(int offset)
{
    this->offset = offset;
    update(boundingRect());
}

void TimeLine::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    qint64 max = beginRange + ((geometry().width()-offset)*timePerPx);
    emit newUpperEnd(max);
}

qint64 TimeLine::getUpperEnd(qint64 lowerEnd)
{
    beginRange = lowerEnd;
    return beginRange + ((geometry().width()-offset)*timePerPx);
}

void TimeLine::onStepSizeChanged(qint64 microSeconds)
{
    largeTickAmount = microSeconds;
    mediumTickAmount = microSeconds/2;
    smallTickAmount = largeTickAmount/10;
    timePerPx = microSeconds/50;

    if (microSeconds % 1000000 == 0)
        timeRepresentation = SECOND_FULL;
    else if (microSeconds > 100000)
        timeRepresentation = SECOND_PART;
    else
        timeRepresentation = MILLISECOND;

    update(boundingRect());
}

void TimeLine::onUpdate(QSize size)
{
    resize(size.width(), this->size().height());
}
