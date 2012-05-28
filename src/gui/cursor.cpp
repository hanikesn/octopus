#include "cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include <QDebug>

Cursor::Cursor(QGraphicsScene *parent) :
    pen(Qt::red),
    brush(Qt::red)
{
    currentPos = 0;
    setGeometry(currentPos, 0, 1, 712);
}

Cursor::~Cursor()
{
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipRect(boundingRect());

    QRectF frame(QPointF(currentPos, 0), geometry().size());
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(frame);
}

QRectF Cursor::boundingRect()
{
    return QRectF(currentPos, 0, geometry().width(), geometry().height());
}

void Cursor::setCursorPos(int pos)
{
    currentPos = pos;
}


