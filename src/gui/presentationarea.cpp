#include "presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include "timeline.h"

#include "track.h"

PresentationArea::PresentationArea(QGraphicsScene *scene)
{
    timeLine = new TimeLine(0, 0);
    pi = new PresentationItem(timeLine, scene);
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
    timeLine->adjustVisibleRange(begin, end);
}

void PresentationArea::onVerticalScroll(QRectF visibleRectangle)
{
    pi->repositionTimeLine(visibleRectangle);
}

void PresentationArea::onChangedWindowSize(QSize size)
{
    currentViewSize = size;
    foreach(Track *t, tracks) {
        t->resize(size.width(), t->size().height());
    }
    timeLine->resize(size.width(), timeLine->size().height());
}
