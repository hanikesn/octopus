#include "gui/cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include "timemanager.h"
#include "gui/presentationitem.h"

#include <QDebug>

Cursor::Cursor(TimeManager *timeManager, QWidget *parent) :
    QWidget(parent),
    pen(Qt::red),
    brush(Qt::red),
    offsetLeft(0),
    currentTime(0),
    timeMgr(timeManager),
    coverHeight(0),
    maxHeight(0)

{
    update();
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

void Cursor::updateOffset(int offset)
{
    offsetLeft = offset;
    update();
}

QRectF Cursor::boundingRect() const
{    
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void Cursor::setTime(qint64 time)
{
    currentTime = time;
    update();
}

void Cursor::update()
{
    resize(1.0,qMin(coverHeight, maxHeight));

    int newPos = timeMgr->convertTimeToPos(currentTime);
    if(newPos == -1)
        setVisible(false);
    else
        setVisible(true);

    // TODO REF
    //setPos(newPos + offsetLeft, 0);
}

qint64 Cursor::getTime()
{
    return currentTime;
}


void Cursor::updateCoverHeight(int height)
{
    coverHeight = height;
}

void Cursor::updateMaxHeight(int height)
{
    maxHeight = height;
}
