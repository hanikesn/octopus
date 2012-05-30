#include "presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>

#include "cursor.h"
#include "track.h"

const int PresentationArea::ACTIONAREAOFFSET = 52;

PresentationArea::PresentationArea(QGraphicsScene *scene):
    currentViewSize(930, 1)
{
    pi = new PresentationItem(scene);
    connect(this, SIGNAL(changedWindowSize(QSize)), pi, SLOT(onChangedWindowSize(QSize)));
    connect(this, SIGNAL(rangeChanged(qint64,qint64)), pi, SLOT(onRangeChanged(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pi, SLOT(onVerticalScroll(QRectF)));
}

PresentationArea::~PresentationArea()
{
}

void PresentationArea::onAddTrack()
{
    Track *t = new Track;
    tracks.append(t);
    connect(t, SIGNAL(del(Track*)), this, SLOT(onDelete(Track*)));
    pi->addTrack(t);       
    t->resize(currentViewSize.width(), t->size().height());   
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
    emit rangeChanged(begin, end);
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
