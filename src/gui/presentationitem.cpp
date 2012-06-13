#include "gui/presentationitem.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QScrollBar>

#include "gui/track.h"
#include "gui/selection.h"
#include "gui/cursor.h"
#include "gui/timeline.h"

const int PresentationItem::ACTIONAREAOFFSET = 52;
const int PresentationItem::TIMEFRAME = 30000000;

PresentationItem::PresentationItem(QScrollBar *hScrollBar, QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    parent(parent),
    boundingRectangle(0, 0, 0, 0),    
    hScrollBar(hScrollBar),
    autoScroll(false),
    createSelection(false),
    visRangeLow(0),
    visRangeHigh(TIMEFRAME),
    minCoverHeight(672),
    playstate(STOPPED)
{            
    timeLine = new TimeLine(ACTIONAREAOFFSET, this, 0);
    timeLine->setZValue(1.0);
    timeLine->drawFrom(0);

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

    hScrollBar->setSingleStep(1);
    hScrollBar->setPageStep(30);
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(0);

    timer.setSingleShot(false);
    timer.setInterval(40);

    connect(selectedArea, SIGNAL(exportTriggered()), this, SIGNAL(exportTriggered()));
    connect(hScrollBar, SIGNAL(sliderMoved(int)), this, SLOT(horizontalScroll(int)));
    connect(hScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll(int)));
    connect(this, SIGNAL(rangeChanged(qint64,qint64)), this, SLOT(onRangeChanged(qint64,qint64)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));    

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
    t->setPlotRange(hScrollBar->value()*1000000, (hScrollBar->value()+30)*1000000);
    t->setOffset(ACTIONAREAOFFSET);

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

        /* TODO(domi):- Falls es TimeMgr Klasse gibt, kann man sich "value" sparen --> jeder Pixel
                        bekommt festen Wert.
                      - Berechnung neu machen, TIMEFRAME stimmt nichtmehr  */
        qint64 lowRange = hScrollBar->value() * 1000000;
        qint64 highRange = lowRange + TIMEFRAME;
        double value = (highRange - lowRange + 1)/(boundingRectangle.width() - ACTIONAREAOFFSET);

        // delta1 is the distance between the left side of the selection and the begin of the graph.
        qint64 delta1 = (begin - ACTIONAREAOFFSET) * value;
        lowRange += delta1;

        // delta2 is the length of the selection
        qint64 delta2 = width*value;
        highRange = lowRange + delta2;

        selectionStart = lowRange;
        selectionEnd = highRange;

        emit selection(selectionStart, selectionEnd);
    } else if (playstate != PLAYING) {
        changeCursorPos(event->pos().x());
        showCursor();
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

void PresentationItem::changeCursorPos(int pos)
{
    if(pos < ACTIONAREAOFFSET) return;
    cursor->setPos(pos, 0);
}

void PresentationItem::onChangedWindowSize(QSize size)
{
    //TODO(domi): damit erscheint am Anfang kein vertikaler Scrollbalken --> Ursache finden!
    minCoverHeight = size.height()-2;

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
    emit rangeChanged(visRangeLow, timeLine->getUpperEnd(visRangeLow));
}

void PresentationItem::onRangeChanged(qint64 begin, qint64 end)
{
    visRangeLow = begin;
    visRangeHigh = end;
}

void PresentationItem::onVerticalScroll(QRectF visibleRectangle)
{    
    timeLine->setPos(0, visibleRectangle.y()-1);
}

void PresentationItem::resizeCursorAndSelection()
{
    recalcBoundingRec();
    if(boundingRectangle.height() > minCoverHeight){
        cursor->resize(1, boundingRectangle.height());
        selectedArea->setHeight(boundingRectangle.height());
    }
}

void PresentationItem::onNewMax(qint64 timestamp)
{
    int max = timestamp/1000000; // max in seconds
    int min = 0;

    hScrollBar->setMaximum(max - hScrollBar->pageStep());
    hScrollBar->setMinimum(min);

    if(autoScroll){

        //TODO(domi) autoscroll auf neue grenzen bei rangeChanged() anpassen TIMEFRAME wäre dann h-l
        hScrollBar->setValue(hScrollBar->maximum());
        qint64 lowerRange = timestamp <= TIMEFRAME ? 0 : timestamp - TIMEFRAME;
        emit rangeChanged(lowerRange, timestamp);
    }
}

void PresentationItem::horizontalScroll(int pos)
{
    qint64 lowerRange = pos*1000000;
    emit rangeChanged(lowerRange, timeLine->getUpperEnd(lowerRange));
    timeLine->drawFrom(lowerRange);
    // remove selection:
    showCursor();
}

void PresentationItem::showCursor()
{
    selectedArea->setVisible(false);
    selectedArea->setWidth(0);
    selectedArea->setHeight(0);
    cursor->setVisible(true);
    // others need to know that there is no selection active any more
    emit selection(-1, -1);
}

void PresentationItem::save(QVariantMap *qvm)
{
    qvm->insert("cursorPos", cursor->pos().toPoint().x());
    QVariantMap visibleArea;
    visibleArea.insert("low", visRangeLow);
    visibleArea.insert("high", visRangeHigh);
    qvm->insert("visibleArea", visibleArea);
}

void PresentationItem::load(QVariantMap *qvm)
{
    cursor->setPos(qvm->find("cursorPos").value().toInt(), 0);

    QVariantMap visibleArea = qvm->find("visibleArea").value().toMap();
    visRangeHigh = visibleArea["high"].toLongLong();
    visRangeLow = visibleArea["low"].toLongLong();

    emit rangeChanged(visRangeLow, visRangeHigh);

    // step-size of scrollbar is 1 second --> left border of timeline is always a full second
    // so we can set the value of the scrollbars slider to the second visRangeLow represents
    hScrollBar->setValue(visRangeLow/1000000);
}

void PresentationItem::onTimeout()
{
    //TODO(domi): Auf Rundungsfehler achten
    //TODO(domi): magic numbers entfernen    
    if(cursor->pos().x() < boundingRectangle.width() - 12){// cursor hasn't reached right border yet
        // determine position for currentTime + 20ms
        currentTime += 40000;
        changeCursorPos(timeLine->convertTimeToPos(currentTime) + ACTIONAREAOFFSET);
    }else{
        visRangeLow += 40000;
        emit rangeChanged(visRangeLow, timeLine->getUpperEnd(visRangeLow));
        timeLine->drawFrom(visRangeLow);
        hScrollBar->setValue(visRangeLow/1000000);
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
        currentTime = timeLine->convertPosToTime(cursor->pos().x());
        timer.start();        
        break;
    case STOPPED:
        playstate = PLAYING;
        currentTime = timeLine->convertPosToTime(cursor->pos().x());
        timer.start();        
        break;
    }
}

//void PresentationItem::setPlayState(const Playstate& p)
//{
////    playstate = p;
//}
