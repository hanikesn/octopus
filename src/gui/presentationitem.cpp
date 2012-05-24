#include "presentationitem.h"

#include <QGraphicsProxyWidget>

#include "track.h"

PresentationItem::PresentationItem(QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent)
{
}

PresentationItem::~PresentationItem()
{
}

QRectF PresentationItem::boundingRect() const
{
    if(this->childItems().isEmpty())
        return QRectF(0, 0, 100, 100);
    else{
        return QRectF(0, 0, this->childItems().at(0)->boundingRect().width(), childItems().size()*this->childItems().at(0)->boundingRect().height());
    }
}

void PresentationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void PresentationItem::addTrack(Track *t)
{
    trackToAdd = parent->addWidget(t);

    // TODO(domi): setPos weglassen, stattdessen Vlayout benutzen?
    trackToAdd->setPos(0, childItems().size()*t->height());
    trackToAdd->setParentItem(this);

    tracks.append(trackToAdd);
}

void PresentationItem::deleteTrack(Track *t)
{

    QGraphicsProxyWidget *del;
    foreach (del ,tracks){
        if(del->widget() == t){
            tracks.removeOne(del);
            childItems().removeOne(del);
            parent->removeItem(del);
        }
    }

}
