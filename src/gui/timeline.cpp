#include "timeline.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

TimeLine::TimeLine(int offset, QGraphicsItem * parent, Qt::WindowFlags wFlags):
    offset(offset),
    QGraphicsWidget(parent, wFlags),
    beginRange(0),
    endRange(1000),
    textBoxWidth(50),
    textBoxHeight(10),
    shortTickHeight(5),
    mediumTickHeight(10),
    largeTickHeight(15),
    pen(Qt::black, 1, Qt::SolidLine)
{
    setGeometry(0, 0, 946, 50);
}

void TimeLine::adjustVisibleRange(qint64 begin, qint64 end)
{

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

    //stops << QGradientStop(((frame.height() + h)/2 )/frame.height(), QColor(157, 195,  55));
    stops << QGradientStop(1.0, QColor(215, 215, 215));
    gradient.setStops(stops);
    painter->setBrush(QBrush(gradient));
    painter->drawRoundedRect(frame, 10.0, 10.0);
    painter->setPen(pen);

    // draw ticks:
    drawTicks(painter);
}

QRectF TimeLine::boundingRect(){    
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void TimeLine::setRange(qint64 start, qint64 end)
{
    beginRange = start;
    endRange = end;
}

void TimeLine::drawTicks(QPainter *painter)
{
    // range per pixel ("value" of a pixel)
    value = (endRange - beginRange + 1) / (geometry().width() - offset);
    qDebug() << "TimeLine::drawTicks()  range-per-pixel: " << value;
    // if the range doesn't start at 0 we need to add a offset
    rangeOffset = beginRange;
    currentPos = 0;
    bottom = geometry().height() - 10;

    while(currentPos < geometry().width()){
        if(currentPos % 50 == 0){
            // large tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset, bottom - largeTickHeight);
            QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth, textBoxHeight);
            int output = (int) (currentPos * value) + rangeOffset;
            painter->drawText(rect, Qt::AlignCenter, QString::number(output));
        }else if(currentPos % 25 == 0){
            // medium tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset, bottom - mediumTickHeight);
        }else if(currentPos % 5 == 0){
            // short tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset, bottom - shortTickHeight);
        }
        // draw a tick every 5 pixels
        currentPos += 5;
    }
}
