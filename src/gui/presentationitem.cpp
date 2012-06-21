#include "gui/presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QScrollBar>
#include <QGraphicsView>

#include "gui/track.h"
#include "gui/selection.h"
#include "gui/cursor.h"
#include "gui/timeline.h"
#include "timemanager.h"

PresentationItem::PresentationItem(TimeLine *timeLine, TimeManager *timeManager,
                                   QWidget *parent) :
    QWidget(parent),
    timeLine(timeLine),
    boundingRectangle(0, 0, 0, 0),
    visRect(0, 0, 100, 672),
    offsetLeft(52),    
    createSelection(false),
    minCoverHeight(672),
    timeMgr(timeManager)
{
    cursor = new Cursor(timeManager, this);

    // TODO hardcoded weg
    cursor->updateCoverHeight(100);
    cursor->updateMaxHeight(100);

    selectedArea = new Selection(timeMgr, this);

    boundingRectangle.setWidth(timeLine->size().width());
    boundingRectangle.setHeight(timeLine->size().height());

    connect(selectedArea, SIGNAL(onExport(qint64,qint64)),    this, SIGNAL(onExport(qint64,qint64)));
    connect(timeMgr, SIGNAL(rangeChanged(qint64,qint64)), selectedArea, SLOT(update()));

    connect(timeMgr, SIGNAL(currentTimeChanged(qint64)), cursor, SLOT(setTime(qint64)));
    connect(timeMgr, SIGNAL(rangeChanged(qint64,qint64)), cursor, SLOT(update()));
    connect(this, SIGNAL(update(QSize)),                  timeLine, SLOT(onUpdate(QSize)));
    connect(this, SIGNAL(update(QSize)),                  selectedArea, SLOT(update()));

    recalcBoundingRec();
    recalcPositions();

    setOffsetLeft(52);
}

PresentationItem::~PresentationItem()
{
}

QRectF PresentationItem::boundingRect() const
{
    return boundingRectangle;
}

void PresentationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void PresentationItem::addTrack(Track *t)
{    
    t->setPlotRange(timeMgr->getLowVisRange(), timeMgr->getHighVisRange());

    boundingRectangle.setHeight(boundingRectangle.height() + t->size().height());
    if(t->size().width() > boundingRectangle.width())
        boundingRectangle.setWidth(t->size().width());

    tracks.append(t);

    recalcPositions();
}

void PresentationItem::removeTrack(Track *t)
{
    boundingRectangle.setHeight(boundingRectangle.height() - t->size().height());
    //parent->setSceneRect(boundingRectangle);

    tracks.removeAll(t);
    layout()->removeWidget(t);

    recalcPositions();
}

void PresentationItem::setOffsetLeft(int offset)
{    
    timeLine->setOffset(offset);
    cursor->updateOffset(offset);
    offsetLeft = offset;
}

void PresentationItem::recalcPositions()
{
    /*int yPos = timeLine->size().height() + 5; // + 5 for border
    foreach(QGraphicsProxyWidget *proxyTrack, tracks){
        proxyTrack->setPos(0, yPos);
        yPos += proxyTrack->widget()->height();
    }

    cursor->updateCoverHeight(getMinCoverHeight());
    emit update(QSize(visRect.width(), visRect.height()));*/
}

void PresentationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) &&
            (QApplication::keyboardModifiers() == Qt::ShiftModifier) &&
            (event->pos().x() >= offsetLeft) &&
            (timeMgr->getPlaystate() != TimeManager::PLAYING)) {
        createSelection = true;
        selectedArea->show();
        selectedArea->setSelectionBegin(timeMgr->convertPosToTime(event->pos().x()));
    }
}

void PresentationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(createSelection){        
        createSelection = false;
        selectionEnd = event->pos().x() < offsetLeft ? offsetLeft : event->pos().x();

        selectedArea->setSelectionEnd(timeMgr->convertPosToTime(selectionEnd));
        cursor->setVisible(false);
    } else if (event->pos().x() >= offsetLeft) {
        qint64 currentTime = timeMgr->convertPosToTime(event->pos().x()- offsetLeft);
        timeMgr->setTime(currentTime);
        selectedArea->hide();
    }
}

void PresentationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((QApplication::keyboardModifiers() != Qt::ShiftModifier)){
        createSelection = false;
    } else if(event->pos().x() >= offsetLeft) {
        selectedArea->setSelectionEnd(timeMgr->convertPosToTime(event->pos().x()));
    }
}

void PresentationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    // Nothing to do...
}

void PresentationItem::recalcBoundingRec()
{
    /*// If there are no tracks height/width are the size of the timeLine
    if(tracks.isEmpty()){
        boundingRectangle.setHeight(minCoverHeight);
        boundingRectangle.setWidth(visRect.width());
        parent->setSceneRect(boundingRectangle);        
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
    parent->setSceneRect(boundingRectangle);*/
}

void PresentationItem::onChangedViewSize(QSize size)
{
    //TODO(domi): damit erscheint am Anfang kein vertikaler Scrollbalken --> Ursache finden!
    minCoverHeight = size.height()-2;
    visRect.setHeight(size.height());
    visRect.setWidth(size.width());    

    recalcBoundingRec();
    emit update(size);  // triggers the resize in cursor, timeLine, selectedArea
    timeMgr->updateRange();    
}

void PresentationItem::onVerticalScroll(QRectF visibleRectangle)
{    
    timeLine->setPos(0, visibleRectangle.y()-1);    
    visRect = visibleRectangle;
    // will trigger a rangeChanged() --> previously invisible tracks will update their range
    timeMgr->updateRange();
}

int PresentationItem::getRightBorder()
{
    /*if (parent->views().at(0)->verticalScrollBar()->isVisible()) { // there is a scrollbar
        // subtract size of scrollbar
        return boundingRectangle.width() -
                parent->views().at(0)->verticalScrollBar()->size().width() - 6;
    }
    else
        return boundingRectangle.width() - 3;*/
    return 0;
}

bool PresentationItem::isVisible(Track *t)
{
    return true;
    /*int minYPos = visRect.y();
    int maxYPos = visRect.y() + minCoverHeight;
    int trackMinY = 0;
    int trackMaxY = 0;

    foreach (QGraphicsProxyWidget *gpw, tracks){
        if (gpw->widget() == t) {
            trackMinY = gpw->pos().y();
            trackMaxY = trackMinY + gpw->size().height();

            if (((trackMinY >= minYPos) && (trackMinY <= maxYPos)) || // top of the track is visible
                    ((trackMaxY >= minYPos) && (trackMaxY <= maxYPos)))  // or bottom visible
                return true;
            else
                return false;
        }
    }
    return false; // return false in case there are no tracks or the specified track couldn't be found*/
}
