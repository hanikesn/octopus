#include "timeline.h"

#include <QPainter>

#include <QDebug>

TimeLine::TimeLine(QGraphicsItem * parent, Qt::WindowFlags wFlags):
    QGraphicsWidget(parent, wFlags)
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
}


QRectF TimeLine::boundingRect(){    
    return QRectF(0, 0, geometry().width(), geometry().height());
}
