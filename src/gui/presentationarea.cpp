#include "gui/presentationarea.h"

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QScrollBar>
#include <QMessageBox>

#include "gui/selection.h"
#include "gui/presentationitem.h"
#include "dataprovider.h"
#include "gui/cursor.h"
#include "gui/sourcedialog.h"
#include "gui/track.h"
#include "gui/timeline.h"
#include "timemanager.h"
#include "gui/TrackHolder.h"

PresentationArea::PresentationArea(const DataProvider &dataProvider,
                                   QScrollBar *hScrollBar, QWidget *parent):
    QScrollArea(parent),
    dataProvider(dataProvider),
    offsetLeft(0),
    unsavedChanges(false),
    recording(false),
    recordStart(0),
    recordEnd(0),
    currentMax(0)
{
    setObjectName("PresentationArea");
    timeManager = new TimeManager(hScrollBar, this);
    timeLine = new TimeLine(52, viewport());
    selection = new Selection(timeManager, viewport());
    cursor = new Cursor(timeManager, viewport());

    setWidget(new TrackHolder(*timeManager, *cursor, *selection, this));
    setWidgetResizable(true);

    // TODO hardcoded weg
    cursor->setFixedHeight(100);

    viewport()->installEventFilter(this);

    connect(timeManager, SIGNAL(currentTimeChanged(qint64)), cursor, SLOT(setTime(qint64)));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), cursor, SLOT(update()));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), selection, SLOT(update()));

    connect(this, SIGNAL(play()),                   timeManager, SLOT(onPlay()));
    connect(this, SIGNAL(zoomIn()),                 timeManager, SLOT(onZoomIn()));
    connect(this, SIGNAL(zoomOut()),                timeManager, SLOT(onZoomOut()));

    connect(this, SIGNAL(changedViewHeight(int)), cursor, SLOT(updateHeight(int)));
    connect(this, SIGNAL(changedViewHeight(int)), selection, SLOT(updateHeight(int)));

    connect(this, SIGNAL(changedViewWidth(int)), timeLine, SLOT(updateWidth(int)));

    connect(selection, SIGNAL(onExport(qint64,qint64)),          this, SIGNAL(exportRange(qint64,qint64)));

    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)),this, SLOT(onRangeChanged(qint64,qint64)));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)),timeLine, SLOT(onRangeChanged(qint64,qint64)));

    connect(timeManager, SIGNAL(stepSizeChanged(qint64)), timeLine, SLOT(onStepSizeChanged(qint64)));

    connect(timeLine, SIGNAL(newUpperEnd(qint64)), timeManager, SLOT(onNewUpperEnd(qint64)));

    connect(&dataProvider, SIGNAL(newMax(qint64)), timeManager, SLOT(onNewMax(qint64)));
    connect(&dataProvider, SIGNAL(newMax(qint64)), this, SLOT(onNewMax(qint64)));
}

PresentationArea::~PresentationArea()
{
}

/**
 * @brief PresentationArea::eventFilter
 *
 * We need to check the resize events of the viewport
 */
bool PresentationArea::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == viewport() && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = dynamic_cast<QResizeEvent*>(event);
        emit changedViewHeight(resizeEvent->size().height());
        emit changedViewWidth(resizeEvent->size().width());
    }
    return QWidget::eventFilter(obj, event);
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

    widget()->layout()->addWidget(t);

    // We need to raise them, because otherwise the tracks will be on top
    timeLine->raise();
    selection->raise();
    cursor->raise();

    t->setPlotRange(timeManager->getLowVisRange(), timeManager->getHighVisRange());

    unsavedChanges = true;
    return t;
}

void PresentationArea::onDelete(Track *t)
{
    tracks.removeAll(t);
    t->deleteLater();
    unsavedChanges = true;
}

void PresentationArea::onRangeChanged(qint64 begin, qint64 end)
{            
    foreach (Track *t, tracks) {
        //if (pi->isVisible(t))
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

void PresentationArea::onNewMax(qint64 max)
{
    currentMax = max;
}

void PresentationArea::setPlotMargins(int newMargin)
{
    /*if (!tracks.isEmpty()) {
        foreach (Track *t, tracks) {
            t->setPlotMarginLeft(newMargin);
        }
        pi->setOffsetLeft(tracks.first()->getPlotOffset() + newMargin);
        emit offsetChanged(tracks.first()->getPlotOffset() + newMargin);
    }*/
}

int PresentationArea::showRecordDialog()
{
    QMessageBox msg;
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Ok);
    msg.setIcon(QMessageBox::Information);
    msg.setButtonText(QMessageBox::Save, tr("Save"));
    msg.setButtonText(QMessageBox::Discard, tr("Discard"));
    msg.setButtonText(QMessageBox::Ok, tr("Continue"));
    msg.setDefaultButton(QMessageBox::Save);
    msg.setText(tr("Do you wish to save the currently recorded data, discard the complete recording or continue the recording?"));
    int result = msg.exec();
    return result;
}

void PresentationArea::save(QVariantMap *qvm)
{
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

void PresentationArea::onRecord()
{
    if (recording) {
        recordEnd = currentMax;
        // end recording, dialog n stuff
        int result = showRecordDialog();
        if (result == QMessageBox::Save) {
            //TODO: speichern, nicht nur exportieren.
            emit saveProject(recordStart, recordEnd);
        } else if (result == QMessageBox::Discard) // discard recording
            recording = false;
        else if (result == QMessageBox::Ok) // go on with the recording
            recording = true;
    } else {
        recordStart = currentMax;
        recording = true;
    }

}
