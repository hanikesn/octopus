#include "mainview/presentationarea.h"

#include "datamodel/dataprovider.h"
#include "dialogs/plotsettingsdialog.h"
#include "dialogs/sourcedialog.h"
#include "mainview/cursor.h"
#include "mainview/selection.h"
#include "mainview/recordselection.h"
#include "mainview/timeline.h"
#include "mainview/tracks/track.h"
#include "time/timemanager.h"

#include <QScrollBar>
#include <QVBoxLayout>

class EventHandler : public MouseEventHandler
{
public:
    EventHandler(TimeManager& timeManager, Cursor &cursor, Selection& selection, QScrollBar* scrollbar)
        : timeManager(timeManager),
          selection(selection),
          cursor(cursor),
          scrollbar(scrollbar),
          createSelection(false),
          dragging(false)
    {}

    /**
      * If shift-button is pressed, a selection is started. Otherwise nothing happens.
      * @param event The mousePressEvent to be processed.
      */
    void mousePressEvent(QMouseEvent *event)
    {
        if(timeManager.isPlaying())
            return;

        if(!timeManager.isValidPos(event->pos().x()))
            return;

        if ((event->button() == Qt::LeftButton) &&
                (event->modifiers() == Qt::ShiftModifier)) {
            createSelection = true;
            selection.show();
            qint64 time = timeManager.convertPosToTime(event->pos().x());
            selection.setSelectionBegin(time);
            selection.setSelectionEnd(time);
            event->accept();
        }

        if(event->button() == Qt::MidButton) {
            dragging = true;
            dragLastPos = event->pos();
            event->accept();
        }
    }

    /**
      * If a selection has been started at 'mousePressEvent()' it ends at the current position of
      * the event (x-position). The corresponding timestamps are calculated and the
      * 'selection()'-signal is emitted.
      * If no new selection has been started this shows the cursor at the events position.
      * @param event The mouseReleaseEvent to be processed.
      */
    void mouseReleaseEvent(QMouseEvent *event)
    {
        dragging = false;

        if(event->button() != Qt::LeftButton)
            return;

        if(createSelection){
            createSelection = false;
            int selectionEnd = timeManager.clipPos(event->pos().x());

            selection.setSelectionEnd(timeManager.convertPosToTime(selectionEnd));
            cursor.setVisible(false);
        } else if (timeManager.isValidPos(event->pos().x())) {
            qint64 currentTime = timeManager.convertPosToTime(event->pos().x());
            timeManager.setTime(currentTime);
            selection.hide();
        }

        event->accept();
    }

    /**
      * If a selection has been started at 'mousePressEvent()' and the shift-button is pressed
      * the selected area is increased in size to the events position (x-position).
      * Otherwise, nothing happens.
      * @param event The mouseMoveEvent to be processed.
      */
    void mouseMoveEvent(QMouseEvent *event)
    {
        if(dragging) {
            scrollbar->setValue(scrollbar->value() + (dragLastPos.x() - event->pos().x())*10);

            dragLastPos = event->pos();
            event->accept();
        }

        if(createSelection &&
                event->modifiers() == Qt::ShiftModifier) {
            selection.setSelectionEnd(timeManager.convertPosToTime(timeManager.clipPos(event->pos().x())));
            event->accept();
        }
    }

    virtual void wheelEvent(QWheelEvent* event)
    {
        if(event->orientation() == Qt::Horizontal) {
            scrollbar->event(event);
        } else if(event->orientation() == Qt::Vertical &&
                  event->modifiers() == Qt::ControlModifier &&
                  timeManager.isValidPos(event->pos().x()))
        {
            timeManager.zoom(event->delta(), timeManager.convertPosToTime(event->pos().x()));
            event->accept();
        }
    }

    /**
      * As we have no interaction for the double click, this function does nothing.
      * @param event The mouseDoubleClickEvent to be processed.
      */
    void mouseDoubleClickEvent(QMouseEvent *event) {Q_UNUSED(event)}
private:
    TimeManager& timeManager;
    Selection& selection;
    Cursor& cursor;
    QScrollBar* scrollbar;

    bool createSelection;
    bool dragging;
    QPoint dragLastPos;
};

PresentationArea::PresentationArea(const DataProvider &dataProvider,
                                   TimeManager *timeManager,
                                   QScrollBar* scrollbar,
                                   QWidget *parent):
    QScrollArea(parent),
    dataProvider(dataProvider),
    timeManager(timeManager),
    unsavedChanges(false)
{
    setObjectName("PresentationArea");
    timeLine = new TimeLine(*timeManager, viewport());
    selection = new Selection(timeManager, viewport());
    cursor = new Cursor(Qt::red, timeManager, viewport());
    maxCursor = new Cursor(Qt::gray, timeManager, viewport());
    recSel = new RecordSelection(timeManager, viewport());

    setWidget(new QWidget(this));
    setWidgetResizable(true);

    widget()->setLayout(new QVBoxLayout());
    widget()->layout()->setMargin(0);
    widget()->layout()->setSpacing(0);
    // This is needed because we embed the layout in a scroll area
    widget()->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QWidget* placeholder = new QWidget();
    placeholder->setFixedHeight(timeLine->size().height());
    widget()->layout()->addWidget(placeholder);

    timeLine->raise();
    selection->raise();
    maxCursor->raise();
    cursor->raise();
    recSel->raise();

    viewportMouseHandler = new EventHandler(*timeManager, *cursor, *selection, scrollbar);
    viewport()->installEventFilter(this);

    connect(timeManager, SIGNAL(currentTimeChanged(qint64)), cursor, SLOT(setTime(qint64)));

    connect(this, SIGNAL(changedViewHeight(int)), cursor, SLOT(updateHeight(int)));
    connect(this, SIGNAL(changedViewHeight(int)), maxCursor, SLOT(updateHeight(int)));
    connect(this, SIGNAL(changedViewHeight(int)), selection, SLOT(updateHeight(int)));
    connect(this, SIGNAL(changedViewHeight(int)), recSel, SLOT(updateHeight(int)));

    connect(this, SIGNAL(marginsChanged(int,int)), timeManager, SLOT(onMarginsChanged(int,int)));
    connect(this, SIGNAL(changedViewWidth(int)), timeLine, SLOT(updateWidth(int)));
    connect(this, SIGNAL(changedViewWidth(int)), timeManager, SLOT(onNewWidth(int)));

    connect(this, SIGNAL(record(qint64,qint64,bool)), recSel, SLOT(onRecord(qint64,qint64,bool)));

    connect(selection, SIGNAL(onExport(qint64,qint64)),         this, SIGNAL(exportRange(qint64,qint64)));
    connect(selection, SIGNAL(selectionChanged(qint64,qint64)), this, SIGNAL(selectionChanged(qint64,qint64)));
    connect(selection, SIGNAL(zoomIn(qint64,qint64)),           timeManager, SLOT(setRange(qint64,qint64)));

    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)),this, SLOT(onRangeChanged(qint64,qint64)));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)),timeLine, SLOT(onRangeChanged(qint64,qint64)));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), cursor, SLOT(onUpdate()));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), maxCursor, SLOT(onUpdate()));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), selection, SLOT(onUpdate()));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), recSel, SLOT(onUpdate()));

    connect(timeManager, SIGNAL(newMax(qint64)), maxCursor, SLOT(setTime(qint64)));
    connect(timeManager, SIGNAL(newMax(qint64)), recSel, SLOT(setSelectionEnd(qint64)));

    connect(&dataProvider, SIGNAL(newMax(qint64)), timeManager, SLOT(onNewMax(qint64)));
    connect(&dataProvider, SIGNAL(newMax(qint64)), this, SLOT(onNewMax(qint64)));

    timeManager->onMarginsChanged(50, 0);
}

PresentationArea::~PresentationArea()
{
    delete viewportMouseHandler;
}

/**
 * @brief PresentationArea::eventFilter
 *
 * We need to check the resize events of the viewport
 */
bool PresentationArea::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == viewport())
    {
        switch(event->type())
        {
        case QEvent::Resize:
        {
            QResizeEvent* resizeEvent = dynamic_cast<QResizeEvent*>(event);
            emit changedViewHeight(resizeEvent->size().height());
            emit changedViewWidth(resizeEvent->size().width());
            break;
        }
        case QEvent::MouseMove:
            viewportMouseHandler->mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonPress:
            viewportMouseHandler->mousePressEvent(dynamic_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonRelease:
            viewportMouseHandler->mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonDblClick:
            viewportMouseHandler->mouseDoubleClickEvent(dynamic_cast<QMouseEvent*>(event));
            break;
        case QEvent::Wheel:
            // We need to detect wether we handled the event
            event->ignore();
            viewportMouseHandler->wheelEvent(dynamic_cast<QWheelEvent*>(event));
            // If we handled it we need to stop the progpagation or else the area will be scrolled
            if(event->isAccepted())
                return true;
            break;
        default:
            break;
        }
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
    foreach (QStringList list, SourceDialog::getSources(dataProvider, "Select Data Series to be Shown")) {
        add(list);
    }
}

void PresentationArea::onPlotSettings()
{
    PlotSettings preset;
    QStringList dataSeriesNames;
    foreach (AbstractDataSeries *series, dataProvider.getDataSeries()) {
        dataSeriesNames.append(series->fullName());
        preset.setOffset(series->fullName(), series->offset());
        preset.setScaleType(series->fullName(), series->defaultScaleType);
    }

    PlotSettings settings = PlotSettingsDialog::getSettings(dataSeriesNames, preset);

    if (!settings.isEmpty()) {
        foreach (QString name, dataSeriesNames) {
            AbstractDataSeries *series = dataProvider.getDataSeries(name);
            if (series) {
                series->setOffset(settings.offset(name));
                series->defaultScaleType = settings.scaleType(name);
            }
        }
    }
}

Track* PresentationArea::add(const QList<QString>& fullDataSeriesNames)
{
    Track *t = new Track(dataProvider, fullDataSeriesNames);
    connect(t, SIGNAL(optPlotMarginsChanged()), this, SLOT(updatePlotMargins()));
    connect(t, SIGNAL(del(Track*)), this, SLOT(onDelete(Track*)));

    tracks.append(t);

    t->setPlotRange(timeManager->getLowVisRange(), timeManager->getHighVisRange());

    widget()->layout()->addWidget(t);
    // Layout the widget before we try to calculate sizes
    t->layout()->activate();

    updatePlotMargins();

    // We need to raise them, because otherwise the tracks will be on top
    timeLine->raise();
    selection->raise();
    maxCursor->raise();
    cursor->raise();
    recSel->raise();

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
        t->setPlotRange(begin, end);
    }
}

void PresentationArea::updatePlotMargins()
{
    if (tracks.isEmpty())
        return;

    // determine the optimal plot margin over all tracks
    int optMargin = 0;
    foreach (Track *t, tracks) {
        if (optMargin < t->getOptPlotMarginLeft()) {
            optMargin = t->getOptPlotMarginLeft();
        }
    }

    foreach (Track *t, tracks) {
        t->setPlotMarginLeft(optMargin);
    }

    emit marginsChanged(tracks.first()->getMarginLeft(), tracks.first()->getMarginRight());
}

void PresentationArea::onNewMax(qint64 /*max*/)
{
    unsavedChanges = true;
}

void PresentationArea::save(QVariantMap *qvm, qint64 begin, qint64 end)
{
    timeManager->save(qvm, begin, end);
    // save tracks in array
    QVariantList trackList;
    foreach(Track *t, tracks){
        // each track stores its information in a map (a object in JSON)
        QVariantMap track;
        t->save(&track, begin, end);
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
        // add new track to presentation area and set it to the current size
        t = add(QList<QString>());
        // 'load(QVariantMap)' needs a map --> put current track in a new map
        QVariantMap trackMap;
        trackMap.insert("track", track);
        t->load(&trackMap);
    }
    timeManager->load(qvm);

    // no unsaved changes directly after loading
    // need to reset the tracks, too, because the time manager's signals may
    // have affected them.
    setUnsavedChanges(false);
}

bool PresentationArea::hasUnsavedChanges()
{
    foreach (Track *t, tracks) {
        unsavedChanges |= t->hasUnsavedChanges();
    }

    return unsavedChanges;
}

void PresentationArea::setUnsavedChanges(bool uc)
{
    unsavedChanges = uc;

    foreach (Track *t, tracks) {
        t->setUnsavedChanges(uc);
    }
}
