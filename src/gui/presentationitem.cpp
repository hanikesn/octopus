#include "presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>

#include "track.h"

PresentationItem::PresentationItem(TimeLine *timeLine, QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent)
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
    if(this->childItems().isEmpty())
        return QRectF(0, 0, 100, 100);
    else
        return QRectF(0, 0, this->childItems().at(0)->boundingRect().width(), childItems().size()*this->childItems().at(0)->boundingRect().height());
}

void PresentationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void PresentationItem::addTrack(Track *t)
{
    int yPos = timeLine->size().height() + 5; // + 5 for border
    trackToAdd = parent->addWidget(t);
    trackToAdd->setPos(0, yPos + (childItems().size()-1)*t->height());
    trackToAdd->setParentItem(this);
    tracks.append(trackToAdd);

    QRectF parentSceneRect = parent->sceneRect();
    parentSceneRect.setHeight(parentSceneRect.height() + t->height());
    parent->setSceneRect(parentSceneRect);

    qDebug() << "# Tracks: "<< tracks.size();
}

void PresentationItem::deleteTrack(Track *t)
{
    QGraphicsProxyWidget *del;
    foreach (del, tracks){
        if(del->widget() == t){
            tracks.removeAll(del);
            childItems().removeAll(del);
            parent->removeItem(del);
        }
    }
    QRectF parentSceneRect = parent->sceneRect();
    parentSceneRect.setHeight(parentSceneRect.height() - t->height());
    parent->setSceneRect(parentSceneRect);

    recalculatePositions();
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
