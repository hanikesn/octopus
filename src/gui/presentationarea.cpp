#include "gui/presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QScrollBar>

#include "gui/cursor.h"
#include "gui/track.h"

PresentationArea::PresentationArea(QGraphicsScene *scene, const DataProvider &dataProvider,
                                   QScrollBar *hScrollBar):
    dataProvider(dataProvider),
    currentViewSize(949, 1),
    selectionBegin(-1),
    selectionEnd(-1)
{
    pi = new PresentationItem(hScrollBar, scene);

    connect(this, SIGNAL(changedWindowSize(QSize)), pi, SLOT(onChangedWindowSize(QSize)));    
    connect(pi, SIGNAL(rangeChanged(qint64,qint64)), this, SLOT(onRangeChanged(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pi, SLOT(onVerticalScroll(QRectF)));
    connect(pi, SIGNAL(selection(qint64,qint64)), this, SLOT(onSelection(qint64, qint64)));
    connect(pi, SIGNAL(exportTriggered()), this, SLOT(onExportTriggered()));

//    connect(dataProvider, SIGNAL(newMax(qint64)), pi, SLOT(onNewMax(qint64)));

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

    //TODO(domi): entfernen, nur für debug-zwecke:
    pi->onNewMax(tracks.size()*30000000);
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

void PresentationArea::onExportTriggered()
{    // propagate signal
    emit exportRange(selectionBegin, selectionEnd);
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
