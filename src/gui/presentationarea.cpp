#include "gui/presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QScrollBar>

#include "gui/cursor.h"
#include "gui/sourcedialog.h"
#include "gui/track.h"

PresentationArea::PresentationArea(QGraphicsScene *scene, const DataProvider &dataProvider,
                                   QScrollBar *hScrollBar, QObject *parent):
    QObject(parent),
    dataProvider(dataProvider),
    currentViewSize(949, 1),
    selectionBegin(-1),
    selectionEnd(-1),
    unsavedChanges(false)
{
    pi = new PresentationItem(hScrollBar, scene);

    connect(this, SIGNAL(changedWindowSize(QSize)), pi, SLOT(onChangedWindowSize(QSize)));    
    connect(pi, SIGNAL(rangeChanged(qint64,qint64)), this, SLOT(onRangeChanged(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pi, SLOT(onVerticalScroll(QRectF)));
    connect(pi, SIGNAL(selection(qint64,qint64)), this, SLOT(onSelection(qint64, qint64)));
    connect(pi, SIGNAL(exportTriggered()), this, SLOT(onExportTriggered()));

    // TODO(domi): nicht vergessen :)
//    connect(dataProvider, SIGNAL(newMax(qint64)), pi, SLOT(onNewMax(qint64)));

}

PresentationArea::~PresentationArea()
{
    // Tracks werden über pi gelöscht, das über die TrackScene gelöscht wird.
}

void PresentationArea::addTrack(const QList<QString> &fullDataSeriesNames)
{
    add(fullDataSeriesNames);
}

void PresentationArea::addTracks(const QList<QString> &fullDataSeriesNames)
{
    foreach (QString dataSeries, fullDataSeriesNames) {
        QList<QString> series;
        series << dataSeries;
        add(series);
    }
}

void PresentationArea::onAddTrack()
{
    foreach (QStringList list, SourceDialog::getSources(dataProvider, "Select Data Series to be Shown")) {
        add(list);
    }
}

Track* PresentationArea::add(const QList<QString>& fullDataSeriesNames)
{    
    Track *t = new Track(dataProvider, fullDataSeriesNames);
    tracks.append(t);
    connect(t, SIGNAL(del(Track*)), this, SLOT(onDelete(Track*)));
    pi->addTrack(t);
    t->resize(currentViewSize.width(), t->size().height());

    //TODO(domi): entfernen, nur für debug-zwecke:
    pi->onNewMax(tracks.size()*30000000);
    unsavedChanges = true;
    return t;
}

void PresentationArea::onDelete(Track *t)
{
    tracks.removeAll(t);
    pi->deleteTrack(t);
    t->deleteLater();
    unsavedChanges = true;
}

void PresentationArea::onRangeChanged(qint64 begin, qint64 end)
{    
    foreach(Track *t, tracks) {
        t->setPlotRange(begin, end);
    }    
    unsavedChanges = true;
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
    if ((begin != -1) && (end != -1)) {
        selectionBegin = begin;
        selectionEnd = end;
    } else {
        // no more selection:
        selectionBegin = -1;
        selectionEnd = -1;
    }
}

void PresentationArea::save(QVariantMap *qvm)
{
    pi->save(qvm);
    // save tracks in array
    QVariantList trackList;
    foreach(Track *t, tracks){
        // each track stores its information in a map (a object in JSON)
        QVariantMap track;
        t->save(&track);
        trackList << track;
    }
    // add array of tracks to map
    qvm->insert("tracks", trackList);
}

void PresentationArea::load(QVariantMap *qvm)
{
    Track *t;
    // get array of all tracks
    QVariantList trackList = qvm->find("tracks").value().toList();

    foreach(QVariant track, trackList){
        // add new track to presentationarea set it to the current size
        t = add(QList<QString>());
        // 'load(QVariantMap)' needs a map --> put current track in a new map
        QVariantMap trackMap;
        trackMap.insert("track", track);
        t->load(&trackMap);
    }
    pi->load(qvm);
}

void PresentationArea::setUnsavedChanges(bool uc)
{
    unsavedChanges = uc;
}
