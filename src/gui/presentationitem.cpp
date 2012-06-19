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
                                   QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),    
    timeLine(timeLine),
    boundingRectangle(0, 0, 0, 0),
    visRect(0, 0, 100, 672),
    offsetLeft(52),
    autoScroll(false),
    createSelection(false),
    currentTime(0),
    minCoverHeight(672),
    playstate(STOPPED),
    timeMgr(timeManager)
{
    timeLine->setParentItem(this);
    timeLine->setZValue(0.9);

    cursor = new Cursor(offsetLeft, timeManager, this, 0);
    cursor->setParentItem(this);
    cursor->setPos(offsetLeft, 0);
    cursor->setZValue(1.0);
    cursor->resize(1, minCoverHeight);

//    selectedArea = new Selection(parent);
        selectedArea = new Selection(this);
    selectedArea->setZValue(1.0);
    selectedArea->setHeight(minCoverHeight);

    boundingRectangle.setWidth(timeLine->size().width());
    boundingRectangle.setHeight(timeLine->size().height());

    timer.setSingleShot(false);
    timer.setInterval(timeMgr->getTimeoutIntervall());

    connect(selectedArea, SIGNAL(exportTriggered()),    this, SIGNAL(exportTriggered()));
    connect(&timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));
    connect(timeMgr, SIGNAL(horizontalScroll()),        this, SLOT(onHorizontalScroll()));

    connect(this, SIGNAL(update(QSize)),                     cursor, SLOT(onUpdate(QSize)));
    connect(this, SIGNAL(update(QSize)),                     timeLine, SLOT(onUpdate(QSize)));
    connect(this, SIGNAL(update(QSize)),                     selectedArea, SLOT(onUpdate(QSize)));

    recalcBoundingRec();
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

    int yPos = timeLine->size().height() + 5; // + 5 for border
    QGraphicsProxyWidget *trackToAdd = parent->addWidget(t);
    trackToAdd->setPos(0, yPos + (tracks.size())*t->height());
    trackToAdd->setParentItem(this);
    tracks.append(trackToAdd);

    parent->setSceneRect(boundingRectangle);

    resizeCursorAndSelection();
}

void PresentationItem::removeTrack(Track *t)
{
    boundingRectangle.setHeight(boundingRectangle.height() - t->size().height());
    parent->setSceneRect(boundingRectangle);

    QGraphicsProxyWidget *del;
    foreach (del, tracks){
        if(del->widget() == t){     
            tracks.removeAll(del);            
            parent->removeItem(del);
            // We should quit the loop, because we have modified the list
            break;
        }
    }    
    recalcPositions();
    resizeCursorAndSelection();
}

void PresentationItem::setOffsetLeft(int offset)
{    
    qint64 cursorTime = cursor->getCurrentTime();

    timeLine->setOffset(offset);
    cursor->setOffset(offset);
    cursor->moveToTime(cursorTime);

    offsetLeft = offset;
}

void PresentationItem::recalcPositions()
{
    int yPos = timeLine->size().height() + 5; // + 5 for border
    foreach(QGraphicsProxyWidget *proxyTrack, tracks){
        proxyTrack->setPos(0, yPos);
        yPos += proxyTrack->widget()->height();
    }
}

void PresentationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) &&
            (QApplication::keyboardModifiers() == Qt::ShiftModifier) &&
            (event->pos().x() >= offsetLeft) &&
            (playstate != PLAYING)) {
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
        selectionEnd = event->pos().x() < offsetLeft ? offsetLeft : event->pos().x();

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

        qint64 lowRange = timeMgr->getLowVisRange() +
                timeMgr->difference(0, begin - offsetLeft);
        qint64 highRange = lowRange + timeMgr->difference(begin, begin + width);
        emit selection(lowRange, highRange);
    } else if (event->pos().x() >= offsetLeft) {
        currentTime = timeMgr->convertPosToTime(event->pos().x()- offsetLeft);
        cursor->changePos(event->pos().x());
        hideSelection();
    }
}

void PresentationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((QApplication::keyboardModifiers() != Qt::ShiftModifier)){
        createSelection = false;
    } else if(event->pos().x() >= offsetLeft) {
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
    Q_UNUSED(event)
    // Nothing to do...
}

void PresentationItem::recalcBoundingRec()
{
    // If there are no tracks height/width are the size of the timeLine
    if(tracks.isEmpty()){        
        boundingRectangle.setHeight(minCoverHeight);
        boundingRectangle.setWidth(timeLine->size().width());        
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
    parent->setSceneRect(boundingRectangle);    
}

void PresentationItem::onChangedViewSize(QSize size)
{
    //TODO(domi): damit erscheint am Anfang kein vertikaler Scrollbalken --> Ursache finden!
    minCoverHeight = size.height()-2;
    visRect.setHeight(size.height());
    visRect.setWidth(size.width());    

    recalcBoundingRec();        
    timeMgr->updateRange();
    emit update(size);  // triggers the resize in cursor, timeLine, selectedArea
}

void PresentationItem::onVerticalScroll(QRectF visibleRectangle)
{    
    timeLine->setPos(0, visibleRectangle.y()-1);    
    visRect = visibleRectangle;
    // will trigger a rangeChanged() --> previously invisible tracks will update their range
    timeMgr->updateRange();
}

void PresentationItem::resizeCursorAndSelection()
{
    recalcBoundingRec();
    if(boundingRectangle.height() > minCoverHeight){
        cursor->resize(1, boundingRectangle.height());
        selectedArea->setHeight(boundingRectangle.height());
    }
}

void PresentationItem::hideSelection()
{
    if (!selectedArea->isVisible()) return; // if selection is already invisible there is nothing to do.
    selectedArea->setVisible(false);
    selectedArea->setWidth(0);
    selectedArea->setHeight(0);    
    // others need to know that there is no selection active any more
    emit selection(-1, -1);
}

int PresentationItem::getRightBorder()
{
    if (parent->views().at(0)->verticalScrollBar()->isVisible()) { // there is a scrollbar
        // subtract size of scrollbar
        return boundingRectangle.width() -
                parent->views().at(0)->verticalScrollBar()->size().width() - 6;
    }
    else
        return boundingRectangle.width() - 3;
}

void PresentationItem::save(QVariantMap *qvm)
{
    qvm->insert("cursorPos", cursor->pos().toPoint().x());
}

void PresentationItem::load(QVariantMap *qvm)
{    
    cursor->changePos(qvm->find("cursorPos").value().toInt());
}

bool PresentationItem::isVisible(Track *t)
{
    int minYPos = visRect.y();
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
    return false; // return false in case there are no tracks or the specified track couldn't be found
}

void PresentationItem::onTimeout()
{
    currentTime += timeMgr->getTimeoutUpdateIntervall();
    int cursorPos = timeMgr->convertTimeToPos(currentTime) + offsetLeft;
    if (cursorPos <= getRightBorder()) {
        // determine position for currentTime + updateIntervall
        cursor->changePos(cursorPos);
    } else if (currentTime > timeMgr->getHighVisRange()) {
        // currentTime is further then the currently visible range
        if (cursor->isVisible()) cursor->setVisible(false);
    } else {
        timeMgr->addRange(timeMgr->getTimeoutUpdateIntervall());
        cursor->setVisible(true);
    }
}

void PresentationItem::onHorizontalScroll()
{
    hideSelection();
    int pos = timeMgr->convertTimeToPos(currentTime);
    if (pos == -1) {
        cursor->setVisible(false);
        return;
    }

    if (playstate != PLAYING) {
        cursor->changePos(pos + offsetLeft);
    } else if (pos + offsetLeft < visRect.width()) { // if cursor is within the visible range
        cursor->changePos(pos + offsetLeft);
    }
}

void PresentationItem::onPlay()
{
    switch(playstate)
    {
    case PLAYING:
        playstate = PAUSED;
        timer.stop();
        break;
    case PAUSED:
        playstate = PLAYING;
        if (currentTime > timeMgr->getHighVisRange() || currentTime < timeMgr->getLowVisRange()) {
            timeMgr->center(currentTime);
            cursor->moveToTime(currentTime);
        }
        timer.start();        
        break;
    case STOPPED:
        playstate = PLAYING;
        if (currentTime > timeMgr->getHighVisRange() || currentTime < timeMgr->getLowVisRange()) {
            timeMgr->center(currentTime);
            cursor->moveToTime(currentTime);
        } else
            currentTime = cursor->getCurrentTime();
        timer.start();        
        break;
    }
}
