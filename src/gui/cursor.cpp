#include "gui/cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include "timemanager.h"
#include "gui/presentationitem.h"

#include <QDebug>

Cursor::Cursor(int offsetLeft, TimeManager *timeManager, PresentationItem *presentationItem,
               QGraphicsScene *parent) :
    pen(Qt::red),
    brush(Qt::red),
    offsetLeft(offsetLeft),
    timeMgr(timeManager),
    presentationItem(presentationItem)
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

void Cursor::changePos(int pos)
{
    if (pos < offsetLeft) return;
    setVisible(true);
    setPos(pos, 0);
}

void Cursor::moveToTime(qint64 time)
{
    int newPos = timeMgr->convertTimeToPos(time);
    if (newPos != -1)
        changePos(newPos + offsetLeft);
}

qint64 Cursor::getCurrentTime()
{
    return timeMgr->convertPosToTime(pos().x() - offsetLeft);
}

void Cursor::onUpdate()
{
    if (presentationItem->getMinCoverHeight() > presentationItem->boundingRect().height())
        resize(1, presentationItem->getMinCoverHeight());
    else
        resize(1, presentationItem->boundingRect().height());
}
