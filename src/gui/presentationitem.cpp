#include "gui/presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "gui/track.h"
#include "gui/selection.h"
#include "gui/cursor.h"
#include "gui/timeline.h"

const int PresentationItem::ACTIONAREAOFFSET = 52;

PresentationItem::PresentationItem(QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),
    boundingRectangle(0, 0, 0, 0),
    createSelection(false),
    minCoverHeight(712)
{            
    timeLine = new TimeLine(ACTIONAREAOFFSET, this, 0);
    timeLine->setZValue(1.0);

    cursor = new Cursor(ACTIONAREAOFFSET, parent);    
    cursor->setParentItem(this);
    cursor->setPos(ACTIONAREAOFFSET, 0);
    cursor->setZValue(1.1);
    cursor->resize(1, minCoverHeight);

    selectedArea = new Selection(parent);
    selectedArea->setZValue(1.0);
    selectedArea->setHeight(minCoverHeight);

    boundingRectangle.setWidth(timeLine->size().width());
    boundingRectangle.setHeight(timeLine->size().height());

    connect(selectedArea, SIGNAL(exportTriggered()), this, SIGNAL(exportTriggered()));
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

    resizeCursorAndSelection();
}

void PresentationItem::deleteTrack(Track *t)
{
    boundingRectangle.setHeight(boundingRectangle.height() - t->size().height());
    parent->setSceneRect(boundingRectangle);

    QGraphicsProxyWidget *del;
    foreach (del, tracks){
        if(del->widget() == t){
            tracks.removeAll(del);
            childItems().removeAll(del);
            parent->removeItem(del);
        }
    }

    recalcPositions();
    resizeCursorAndSelection();
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
            (QApplication::keyboardModifiers() == Qt::ShiftModifier) && (event->pos().x() >= ACTIONAREAOFFSET)){
        createSelection = true;
        selectionStart = event->pos().x();
        int selectionHeight = boundingRectangle.height() > minCoverHeight ?
                    boundingRectangle.height() : minCoverHeight;
        selectedArea->setHeight(selectionHeight);
        selectedArea->setVisible(true);
    }
}

void PresentationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(createSelection){        
        createSelection = false;
        selectionEnd = event->pos().x() < ACTIONAREAOFFSET ? ACTIONAREAOFFSET : event->pos().x();

        int width = 0;
        int begin = 0;
        if(selectionEnd > selectionStart){
            width = selectionEnd - selectionStart + 1;
            begin = selectionStart;
        }else{
            width = selectionStart - selectionEnd + 1;
            begin = selectionEnd;            
        }

        selectedArea->setWidth(width);        
        selectedArea->setPos(begin, 0);
        selectedArea->update(selectedArea->boundingRect());
        cursor->setVisible(false);        

        emit selection(begin, begin + width - 1);
    }else{
        selectedArea->setVisible(false);
        selectedArea->setWidth(0);
        selectedArea->setHeight(0);
        cursor->setVisible(true);
        cursorPosChanged(event->pos().x());
        // others need to know that there is no selection active any more
        emit selection(-1, -1);        
    }
}

void PresentationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((QApplication::keyboardModifiers() != Qt::ShiftModifier)){
        createSelection = false;
    } else if(event->pos().x() >= ACTIONAREAOFFSET) {
        selectionEnd = event->pos().x();
        int width = 0;
        int begin = 0;
        if(selectionEnd > selectionStart){
            width = selectionEnd - selectionStart + 1;
            begin = selectionStart;
        }else{
            width = selectionStart - selectionEnd + 1;
            begin = selectionEnd;
        }

        selectedArea->setWidth(width);
        selectedArea->update(selectedArea->boundingRect());
        selectedArea->setPos(begin, 0);
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
        boundingRectangle.setHeight(minCoverHeight);
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

void PresentationItem::onChangedWindowSize(QSize size)
{
    minCoverHeight = size.height();

    // resize cursor, timeLine, selectedArea
    if(boundingRectangle.height() > minCoverHeight){
        cursor->resize(1, boundingRectangle.height());
        selectedArea->setHeight(boundingRectangle.height());
        timeLine->resize(size.width(), timeLine->size().height());
    }else{
        cursor->resize(1, minCoverHeight);
        selectedArea->setHeight(minCoverHeight);
        timeLine->resize(size.width(), timeLine->size().height());
    }

    recalcBoundingRec();
}

void PresentationItem::onRangeChanged(qint64 begin, qint64 end)
{
    timeLine->adjustVisibleRange(begin, end);
}

void PresentationItem::onVerticalScroll(QRectF visibleRectangle)
{
    repositionTimeLine(visibleRectangle);
}

void PresentationItem::resizeCursorAndSelection()
{
    recalcBoundingRec();
    if(boundingRectangle.height() > minCoverHeight){
        cursor->resize(1, boundingRectangle.height());
        selectedArea->setHeight(boundingRectangle.height());
    }
}
