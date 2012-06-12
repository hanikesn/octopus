#include "gui/cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include <QDebug>



Cursor::Cursor(int offset, QGraphicsScene *parent) :
    pen(Qt::red),
    brush(Qt::red),
    offset(offset)
{
    Q_UNUSED(parent)
    setGeometry(0, 0, 1, 42);
}

Cursor::~Cursor()
{
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{    
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipRect(boundingRect());

    QRectF frame(QPointF(0, 0), geometry().size());
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(frame);
}

QRectF Cursor::boundingRect() const
{    
    return QRectF(0, 0, geometry().width(), geometry().height());
}
