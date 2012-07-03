#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include "serializable.h"

#include <QScrollArea>

class QMouseEvent;
class QWheelEvent;
class PresentationItem;
class DataProvider;
class Track;
class Cursor;
class TimeLine;
class TimeManager;
class Cursor;
class Selection;
class RecordSelection;

class MouseEventHandler
{
public:
    virtual ~MouseEventHandler() {}
    // Functions to handle mouse input
    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) = 0;
    virtual void wheelEvent(QWheelEvent* event) = 0;
};

class PresentationArea : public QScrollArea, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationArea(const DataProvider &dataProvider,
                              TimeManager *timeManager,
                              QScrollBar* scrollbar,
                              QWidget *parent = 0);
    ~PresentationArea();

    /**
      * Propagates the save-call to the timeManager and to each of the current tracks.
      * @param qvm The map in which the necessary data needs to be stored.
      */
    void save(QVariantMap *qvm);

    /**
      * Propagates the load-call to the timeManager and to each of the current tracks.
      * @param qvm The map in which the necessary data is stored.
      */
    void load(QVariantMap *qvm);

    bool hasUnsavedChanges(){return unsavedChanges;}
    void setUnsavedChanges(bool uc);

signals:
    /**
      * This signal propagates selections 'onExport(qint64, qint64)' signal.
      * @param begin Begin of the selection.
      * @param end End of the selection.
      */
    void exportRange(qint64 begin, qint64 end);

    /**
      * This signal is emitted when a ResizeEvent occurs.
      * @param h The new height.
      */
    void changedViewHeight(int h);

    /**
      * This signal is emitted when a ResizeEvent occurs.
      * @param w The new width.
      */
    void changedViewWidth(int w);

    /**
      * This signal is emitted when the margins of the tracks are updated.
      * @param marginLeft Left margin
      * @param marginRight Right margin
      */
    void marginsChanged(int marginLeft, int marginRight);

    /**
      * This signal propagates selections 'selectionChanged(qint64, qint64)' signal.
      * @param begin Begin of the selection.
      * @param end End of the selection.
      */
    void selectionChanged(qint64 begin, qint64 end);

    /**
      * This signal propagates the recorders signal 'record(qint64, qint64, bool)' (connect is made
      * by the ViewManager) to the RecordSelection.
      * @see RecordSelection for more details.
      * @param start Start of the selection.
      * @param end End of the selection.
      * @param recording True if currently recording, false otherwise.
      */
    void record(qint64 start, qint64 end, bool recording);

private slots:
    /**
      * Shows the SourcesDialog and adds corresponding track(s) by calling 'add(QStringList)'.
      */
    void onAddTrack();

    /**
      * Deletes the specified track.
      * @param t Track to be deleted.
      */
    void onDelete(Track *t);

    void onPlotSettings();
    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted by the timeManager.
      * Updates the tracks to the new range.
      * @param begin Begin of the range.
      * @param end End of the range
      */
    void onRangeChanged(qint64 begin, qint64 end);

    void updatePlotMargins();

    /**
      * Sets 'unsavedChanges' to true when new data is received (even if it's no shown in a track).
      * This works because the maximum is growing continiously.
      * @param max New maximum
      */
    void onNewMax(qint64 max);

protected:
    /**
      * Reimplemented from QObject.
      * Determines whether a Mouse- or ResizeEvent happened.
      */
    bool eventFilter(QObject *, QEvent *);

private:    
    const DataProvider &dataProvider;
    QList<Track*> tracks;

    TimeLine *timeLine;
    Selection *selection;
    Cursor *cursor;
    Cursor* maxCursor;
    RecordSelection *recSel;

    TimeManager *timeManager;

    bool unsavedChanges;    

    MouseEventHandler* viewportMouseHandler;

    void addTrack(const QList<QString>& fullDataSeriesNames);
    void addTracks(const QList<QString>& fullDataSeriesNames);

    /**
      * Adds a track which contains graphs for everey data series in 'fullDataSeriesNames'.
      * Makes the necessary connects and adds the track widget to the layout.
      * @param fullDataSeriesNames The names of all data series to be added.
      * @return A pointer to the created track.
      */
    Track* add(const QList<QString>& fullDataSeriesNames);
};

#endif // PRESENTATIONAREA_H
