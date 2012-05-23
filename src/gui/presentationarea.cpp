#include "presentationarea.h"

#include "track.h"

#include <QVBoxLayout>
#include <QGraphicsProxyWidget>

PresentationArea::PresentationArea(QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),
    height(100),
    width(100)
{
}

PresentationArea::~PresentationArea()
{
}

QRectF PresentationArea::boundingRect() const
{
    qreal penWidth = 1;
    if(this->childItems().isEmpty())
        return QRectF(0, 0, width, height);
    else{
        return QRectF(0, 0, this->childItems().at(0)->boundingRect().width(), childItems().size()*this->childItems().at(0)->boundingRect().height());
    }
}

void PresentationArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void PresentationArea::onAddTrack()
{
    Track *t = new Track;
    tracks.append(t);

    QGraphicsProxyWidget *proxy = parent->addWidget(t);
    proxy->setPos(0, childItems().size()*t->height());
    proxy->setParentItem(this);
}


void PresentationArea::setHeight(qreal height)
{
    this->height = height;
}

void PresentationArea::setWidth(qreal width)
{
    this->width = width;
}

void PresentationArea::onDelete(Track *t)
{
    tracks.removeAll(t);
    // TODO(Domi): Track löschen
}

void PresentationArea::onRangeChanged(qint64 begin, qint64 end)
{
    foreach(Track *t, tracks) {
        t->setPlotRange(begin, end);
    }
}
