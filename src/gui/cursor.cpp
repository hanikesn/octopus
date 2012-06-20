#include "gui/cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include "timemanager.h"
#include "gui/presentationitem.h"

#include <QDebug>

Cursor::Cursor(int offsetLeft, TimeManager *timeManager, PresentationItem *presentationItem,
               QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    pen(Qt::red),
    brush(Qt::red),
    offsetLeft(offsetLeft),
    currentTime(0),
    timeMgr(timeManager),
    presentationItem(presentationItem)

{
    setZValue(1.0);
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

void Cursor::onOffsetChanged(int offset)
{
    offsetLeft = offset;
    update();
}

QRectF Cursor::boundingRect() const
{    
    return QRectF(0, 0, geometry().width(), geometry().height());
}

void Cursor::changePos(int pos)
{
    if (pos < offsetLeft) return;
    setVisible(true);
    setPos(pos, 0);
    currentTime = timeMgr->convertPosToTime(pos - offsetLeft);
}

void Cursor::setTime(qint64 time)
{
    currentTime = time;
    update();
}

void Cursor::update()
{
    int newPos = timeMgr->convertTimeToPos(currentTime);
    if(newPos == -1)
        setVisible(false);
    else
        setVisible(true);
    changePos(newPos + offsetLeft);
}

qint64 Cursor::getTime()
{
    return currentTime;
}

// TODO richtige größe bekommen
void Cursor::onUpdateSize(QSize size)
{
    Q_UNUSED(size)
    if (presentationItem->getMinCoverHeight() > presentationItem->boundingRect().height())
        resize(1, presentationItem->getMinCoverHeight());
    else
        resize(1, presentationItem->boundingRect().height());
}

