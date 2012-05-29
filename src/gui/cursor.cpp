#include "cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include <QDebug>



Cursor::Cursor(QGraphicsScene *parent) :
    pen(Qt::red),
    brush(Qt::red),
    minHeight(712)
{
    setGeometry(0, 0, 1, minHeight);
}

Cursor::~Cursor()
{
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipRect(boundingRect());

    QRectF frame(QPointF(0, 0), geometry().size());
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(frame);
}

QRectF Cursor::boundingRect()
{
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void Cursor::setMinHeight(int mh)
{
    minHeight = mh;
}



