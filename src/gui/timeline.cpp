#include "gui/timeline.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

TimeLine::TimeLine(int offset, QGraphicsItem * parent, Qt::WindowFlags wFlags):
    offset(offset),
    QGraphicsWidget(parent, wFlags),
    beginRange(0),
    endRange(30000000),
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
    beginRange = begin;
    endRange = end;
    update(boundingRect());
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
    painter->drawRoundedRect(frame, 10.0, 10.0);

    // draw ticks:
    drawTicks(painter);
}

QRectF TimeLine::boundingRect(){    
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void TimeLine::drawTicks(QPainter *painter)
{
    // range per pixel ("value" of a pixel)
    value = (endRange - beginRange + 1) / (geometry().width() - offset);
//    qDebug() << "TimeLine::drawTicks()  range-per-pixel: " << value;
    // if the range doesn't start at 0 we need to add a offset
    rangeOffset = beginRange;
    currentPos = 0;
    bottom = geometry().height() - 10;
    double output  = 0.0;
    while(currentPos < geometry().width()){
        output = ((currentPos * value) + rangeOffset)/1000000;
        if(currentPos % 50 == 0){
            // large tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset, bottom - largeTickHeight);
            QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth, textBoxHeight);
            if(value < 0.02){
                painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(output, 0, 'f', 3));
            }else{                
                painter->drawText(rect, Qt::AlignCenter, QString::number((int)output));
            }

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
