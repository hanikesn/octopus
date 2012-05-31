#include "presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "track.h"
#include "selection.h"
#include "cursor.h"
#include "timeline.h"

const int PresentationItem::ACTIONAREAOFFSET = 52;

PresentationItem::PresentationItem(QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),
    boundingRectangle(0, 0, 0, 0),
    createSelection(false)
{        
    // setup timeLine
    timeLine = new TimeLine(this, 0);    
    timeLine->setZValue(1.0);

    // setup cursor
    cursor = new Cursor(ACTIONAREAOFFSET, parent);    
    cursor->setParentItem(this);
    cursor->setPos(ACTIONAREAOFFSET, 0);
    cursor->setZValue(1.1);

    selection = new Selection(parent);
    selection->setZValue(0.9);

    boundingRectangle.setWidth(timeLine->size().width());
    boundingRectangle.setHeight(timeLine->size().height());
}

PresentationItem::~PresentationItem()
{
}

QRectF PresentationItem::boundingRect() const
{
    if(childItems().isEmpty())
        return QRectF(0, 0, 100, 100);
    else
        return boundingRectangle;    
}

void PresentationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{

}

void PresentationItem::addTrack(Track *t)
{    
    boundingRectangle.setHeight(boundingRectangle.height() + t->size().height());
    if(t->size().width() > boundingRectangle.width())
        boundingRectangle.setWidth(t->size().width());

    int yPos = timeLine->size().height() + 5; // + 5 for border
    trackToAdd = parent->addWidget(t);
    trackToAdd->setPos(0, yPos + (tracks.size())*t->height());
    trackToAdd->setParentItem(this);
    tracks.append(trackToAdd);

    parent->setSceneRect(boundingRectangle);

    resizeCursor();
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

    recalcPositions();
    parent->setSceneRect(boundingRectangle);
    resizeCursor();
}

void PresentationItem::recalcPositions()
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
    if((event->button() == Qt::LeftButton) &&
            (QApplication::keyboardModifiers() == Qt::ShiftModifier)){
        createSelection = true;
        selectionStart = event->pos().x();
    }
}

void PresentationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(createSelection){
        selectionEnd = event->pos().x();
        createSelection = false;

        int diff = 0;
        int begin = 0;
        if(selectionEnd > selectionStart){
            diff = selectionEnd - selectionStart + 1;
            begin = selectionStart;
        }else{
            diff = selectionStart - selectionEnd + 1;
            begin = selectionEnd;
        }

        selection->setHeight(boundingRectangle.height());
        selection->setWidth(diff);
        selection->setPos(begin, 0);
        selection->setVisible(true);
        cursor->setVisible(false);
    }else{
        selection->setVisible(false);
        cursor->setVisible(true);
        cursorPosChanged(event->pos().x());
    }
}

void PresentationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier){
        createSelection = false;
    }
}

void PresentationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Nothing to do...
}

void PresentationItem::recalcBoundingRec()
{
    // If there are no tracks height/width are the size of the timeLine
    if(tracks.isEmpty()){
        boundingRectangle.setHeight(timeLine->size().height());
        boundingRectangle.setWidth(timeLine->size().width());
        return;
    }

    int height = timeLine->size().height();
    int width = timeLine->size().width();

    if(tracks.at(0)->size().width() > width)
        width = tracks.at(0)->size().width();

    foreach(QGraphicsProxyWidget *track, tracks){
        height += track->size().height();
    }

    boundingRectangle.setHeight(height);
    boundingRectangle.setWidth(width);
    parent->setSceneRect(boundingRectangle);
}

void PresentationItem::cursorPosChanged(int pos)
{
    if(pos < ACTIONAREAOFFSET) return;
    cursor->setPos(pos, 0);
}

void PresentationItem::resizeCursor()
{
    if(boundingRectangle.height() > cursor->getMinHeight())
        cursor->resize(1, boundingRectangle.height());
    else
        cursor->resize(1, cursor->getMinHeight());
}

void PresentationItem::onChangedWindowSize(QSize size)
{
    timeLine->resize(size.width(), timeLine->size().height());

    // resize cursor
    cursor->setMinHeight(size.height());
    recalcBoundingRec();
    resizeCursor();
}

void PresentationItem::onRangeChanged(qint64 begin, qint64 end)
{
    timeLine->adjustVisibleRange(begin, end);
}

void PresentationItem::onVerticalScroll(QRectF visibleRectangle)
{
    repositionTimeLine(visibleRectangle);
}
