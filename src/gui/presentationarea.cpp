#include "presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>

#include "cursor.h"
#include "timeline.h"
#include "track.h"

const int PresentationArea::ACTIONAREAOFFSET = 52;

PresentationArea::PresentationArea(QGraphicsScene *scene):
    currentViewSize(930, 1)
{
    timeLine = new TimeLine(0, 0);    
    pi = new PresentationItem(timeLine, scene);
    cursor = new Cursor(scene);    
    scene->addItem(cursor);
    cursor->setPos(ACTIONAREAOFFSET, 0);

    connect(pi, SIGNAL(cursorPosChanged(int)), this, SLOT(onCursorPosChanged(int)));
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

    if(pi->boundingRect().height() > cursor->getMinHeight())
        cursor->resize(1, pi->boundingRect().height());
    else
        cursor->resize(1, cursor->getMinHeight());
}

void PresentationArea::onDelete(Track *t)
{
    tracks.removeAll(t);
    pi->deleteTrack(t);

    if(pi->boundingRect().height() > cursor->getMinHeight())
        cursor->resize(1, pi->boundingRect().height());
    else
        cursor->resize(1, cursor->getMinHeight());
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
    // resize tracks:
    foreach(Track *t, tracks) {
        t->resize(size.width(), t->size().height());
    }
    // resize timeLine
    timeLine->resize(size.width(), timeLine->size().height());

    // resize cursor
    cursor->setMinHeight(size.height());
    if(size.height() > pi->boundingRect().height())
        cursor->resize(1, size.height());
}

void PresentationArea::onCursorPosChanged(int pos)
{
    if(pos < ACTIONAREAOFFSET) return;
    cursor->setPos(pos, 0);
}
