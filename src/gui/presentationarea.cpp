#include "gui/presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QScrollBar>

#include "gui/cursor.h"
#include "gui/track.h"

const int PresentationArea::ACTIONAREAOFFSET = 52;
const int PresentationArea::TIMEFRAME = 30000000;

PresentationArea::PresentationArea(QGraphicsScene *scene, DataProvider *dataProvider, QScrollBar *hScrollBar):
    dataProvider(dataProvider),
    currentViewSize(949, 1),
    selectionBegin(-1),
    selectionEnd(-1),
    hScrollBar(hScrollBar),
    autoScroll(false),
    lastSliderPos(0)
{
    pi = new PresentationItem(scene);

    hScrollBar->setSingleStep(1);
    hScrollBar->setPageStep(30);
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(0);

    connect(this, SIGNAL(changedWindowSize(QSize)), pi, SLOT(onChangedWindowSize(QSize)));
    connect(this, SIGNAL(rangeChanged(qint64,qint64)), pi, SLOT(onRangeChanged(qint64,qint64)));
    connect(this, SIGNAL(rangeChanged(qint64,qint64)), this, SLOT(onRangeChanged(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pi, SLOT(onVerticalScroll(QRectF)));
    connect(pi, SIGNAL(selection(qint64,qint64)), this, SLOT(onSelection(qint64,qint64)));
    connect(pi, SIGNAL(exportTriggered()), this, SLOT(onExportTriggered()));
    connect(hScrollBar, SIGNAL(sliderMoved(int)), this, SLOT(horizontalScroll(int)));
    connect(hScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll(int)));

//    connect(dataProvider, SIGNAL(newMax(qint64)), this, SLOT(onNewMax(qint64)));

}

PresentationArea::~PresentationArea()
{
}

void PresentationArea::onAddTrack()
{
    Track *t = new Track(dataProvider);
    tracks.append(t);
    connect(t, SIGNAL(del(Track*)), this, SLOT(onDelete(Track*)));
    pi->addTrack(t);       
    t->resize(currentViewSize.width(), t->size().height());
    t->setPlotRange(hScrollBar->value()*1000000, (hScrollBar->value()+30)*1000000);


    //TODO(domi): entfernen, nur für debug-zwecke:
    onNewMax(tracks.size()*TIMEFRAME);
}

void PresentationArea::onDelete(Track *t)
{
    tracks.removeAll(t);
    pi->deleteTrack(t);
}

void PresentationArea::onRangeChanged(qint64 begin, qint64 end)
{    
    foreach(Track *t, tracks) {
        t->setPlotRange(begin, end);
    }    
}

void PresentationArea::onChangedWindowSize(QSize size)
{
    currentViewSize = size;
    // resize tracks:
    foreach(Track *t, tracks) {
        t->resize(size.width(), t->size().height());
    }
    // propagate event (resizes TimeLine and Cursor in PresentationItem)
    emit changedWindowSize(size);
}

void PresentationArea::onSelection(qint64 begin, qint64 end)
{
    if((begin != -1) && (end != -1)){
        selectionBegin = begin;
        selectionEnd = end;
    }else{
        // no more selection:
        selectionBegin = -1;
        selectionEnd = -1;
    }
}

void PresentationArea::onExportTriggered()
{
    // propagate signal
    emit exportRange(selectionBegin, selectionEnd);
}

void PresentationArea::onNewMax(qint64 timestamp)
{
    int max = timestamp/1000000; // max in seconds
    int min = 0;

    hScrollBar->setMaximum(max - hScrollBar->pageStep());
    hScrollBar->setMinimum(min);

    if(autoScroll){
        hScrollBar->setValue(hScrollBar->maximum());
        qint64 lowerRange = timestamp <= TIMEFRAME ? 0 : timestamp - TIMEFRAME;
        emit rangeChanged(lowerRange, timestamp);
    }
}

void PresentationArea::horizontalScroll(int pos)
{
    qint64 lowerRange = pos*1000000;
    emit rangeChanged(lowerRange, lowerRange + TIMEFRAME);
    lastSliderPos = pos;
}
