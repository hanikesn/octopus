#include "presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "track.h"

PresentationItem::PresentationItem(TimeLine *timeLine, QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),
    boundingRectangle(0, 0, timeLine->size().width(), timeLine->size().height())
{        
    this->timeLine = timeLine;
    this->timeLine->setParentItem(this);
    this->timeLine->setZValue(1);
}

PresentationItem::~PresentationItem()
{
}

QRectF PresentationItem::boundingRect() const
{
    if(childItems().isEmpty())
        return QRectF(0, 0, 100, 100);
    else{
        return boundingRectangle;
    }
}

void PresentationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{

}

void PresentationItem::addTrack(Track *t)
{
    //TODO(domi): was passiert, wenn man am Anfang das Fenster kleiner zieht?
    boundingRectangle.setHeight(boundingRectangle.height() + t->size().height());
    if(t->size().width() > boundingRectangle.width())
        boundingRectangle.setWidth(t->size().width());

    int yPos = timeLine->size().height() + 5; // + 5 for border
    trackToAdd = parent->addWidget(t);
    trackToAdd->setPos(0, yPos + (childItems().size()-1)*t->height());
    trackToAdd->setParentItem(this);
    tracks.append(trackToAdd);

    parent->setSceneRect(boundingRectangle);
}

void PresentationItem::deleteTrack(Track *t)
{
    boundingRectangle.setHeight(boundingRectangle.height() - t->size().height());

    QGraphicsProxyWidget *del;
    foreach (del, tracks){
        if(del->widget() == t){
            tracks.removeAll(del);
            childItems().removeAll(del);
            parent->removeItem(del);
        }
    }

    recalculatePositions();
    parent->setSceneRect(boundingRectangle);
}

void PresentationItem::recalculatePositions()
{
    int yPos = timeLine->size().height() + 5; // + 5 for border
    foreach(QGraphicsProxyWidget *proxyTrack, tracks){
        proxyTrack->setPos(0, yPos);
        yPos += proxyTrack->widget()->height();
    }
}

void PresentationItem::repositionTimeLine(QRectF visibleRectangle)
{
    timeLine->setPos(0, visibleRectangle.y()-1);

}

void PresentationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PresentationItem::mousePressEvent: " << event->pos();
}

void PresentationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    emit cursorPosChanged(event->pos().x());
}

void PresentationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PresentationItem::mouseMoveEvent: " << event->lastPos()  << event->pos();
}

void PresentationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PresentationItem::mouseDoubleClickEvent: " << event->pos();
}
