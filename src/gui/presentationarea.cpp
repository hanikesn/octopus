#include "gui/presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QScrollBar>

#include "gui/cursor.h"
#include "gui/sourcedialog.h"
#include "gui/track.h"
#include "gui/timeline.h"
#include "timemanager.h"

PresentationArea::PresentationArea(QGraphicsScene *scene, const DataProvider &dataProvider,
                                   QScrollBar *hScrollBar, QObject *parent):
    QObject(parent),
    dataProvider(dataProvider),
    currentViewSize(949, 1),
    selectionBegin(-1),
    selectionEnd(-1),
    unsavedChanges(false),
    playstate(PresentationItem::STOPPED)
{
    timeLine = new TimeLine(52, 0, 0);
    timeManager = new TimeManager(hScrollBar, timeLine);
    pi = new PresentationItem(timeLine, timeManager, scene);


    connect(this, SIGNAL(changedViewSize(QSize)),   pi, SLOT(onChangedViewSize(QSize)));
    connect(this, SIGNAL(verticalScroll(QRectF)),   pi, SLOT(onVerticalScroll(QRectF)));
    connect(this, SIGNAL(play()),                   pi, SLOT(onPlay()));
    connect(pi, SIGNAL(selection(qint64,qint64)),   this, SLOT(onSelection(qint64, qint64)));
    connect(pi, SIGNAL(exportTriggered()),          this, SLOT(onExportTriggered()));

    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)),
            this, SLOT(onRangeChanged(qint64,qint64)));

    // TODO(domi): nicht vergessen :)
//    connect(dataProvider, SIGNAL(newMax(qint64)), timeManager, SLOT(onNewMax(qint64)));
}

PresentationArea::~PresentationArea()
{
    // Tracks werden über pi gelöscht, das über die TrackScene gelöscht wird.
    timeManager->deleteLater();
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
    // TODO(Steffi) : Hier sollte keine Preselected-Liste mitgegeben werden.
    foreach (QStringList list, SourceDialog::getSources(dataProvider, "Select Data Series to be Shown", true, dataProvider.getDataSeriesList())) {
        add(list);
    }
}

Track* PresentationArea::add(const QList<QString>& fullDataSeriesNames)
{
    Track *t = new Track(dataProvider, fullDataSeriesNames);
    connect(t, SIGNAL(optPlotMarginsChanged()), this, SLOT(updatePlotMargins()));
    connect(t, SIGNAL(del(Track*)), this, SLOT(onDelete(Track*)));

    tracks.append(t);
    updatePlotMargins();

    pi->addTrack(t);
    t->resize(currentViewSize.width(), t->size().height());
    t->setPlotRange(timeManager->getLowVisRange(), timeManager->getHighVisRange());

    //TODO(domi): entfernen, nur für debug-zwecke:
    timeManager->onNewMax(tracks.size()*30000000);
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
    foreach (Track *t, tracks) {
        if (pi->isVisible(t))
            t->setPlotRange(begin, end);
    }
    if (!tracks.isEmpty())
        unsavedChanges = true;
}

void PresentationArea::updatePlotMargins()
{
    // determine the optimal plot margin over all tracks
    int optMargin = 0;
    foreach (Track *t, tracks) {
        if (optMargin < t->optPlotMarginLeft) {
            optMargin = t->optPlotMarginLeft;
        }
    }

    setPlotMargins(optMargin);
}

void PresentationArea::setPlotMargins(int newMargin)
{
    if (!tracks.isEmpty()) {
        foreach (Track *t, tracks) {
            t->setPlotMarginLeft(newMargin);
        }
        pi->setOffsetLeft(tracks.first()->getPlotOffset() + newMargin);
    }
}

void PresentationArea::onChangedViewSize(QSize size)
{    
    currentViewSize = size;
    // resize tracks:
    foreach(Track *t, tracks) {
        t->resize(size.width(), t->size().height());
    }
    // propagate event (resizes TimeLine and Cursor in PresentationItem)
    emit changedViewSize(size);
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
    timeManager->save(qvm);
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
    timeManager->load(qvm);
}

void PresentationArea::setUnsavedChanges(bool uc)
{
    unsavedChanges = uc;
}

void PresentationArea::onPlay()
{
    /* Do stuff */
    emit play();  // propagate signal
}

