#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QWidget>
#include <QSize>
#include <QRectF>
#include <QScrollArea>

#include "serializable.h"

class PresentationItem;
class DataProvider;
class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class Cursor;
class QScrollBar;
class TimeLine;
class TimeManager;
class Cursor;
class Selection;

class PresentationArea : public QScrollArea, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationArea(const DataProvider &dataProvider,
                              QScrollBar *hScrollBar, QWidget *parent = 0);
    ~PresentationArea();

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

    bool hasUnsavedChanges(){return unsavedChanges;}
    void setUnsavedChanges(bool uc);

    /**
      * If shift-button is pressed, a selection is started. Otherwise nothing happens.
      * @param event The mousePressEvent to be processed.
      */
    void mousePressEvent(QMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' it ends at the current position of
      * the event (x-position). The corresponding timestamps are calculated and the
      * 'selection()'-signal is emitted.
      * If no new selection has been started this shows the cursor at the events position.
      * @param event The mouseReleaseEvent to be processed.
      */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' and the shift-button is pressed
      * the selected area is increased in size to the events position (x-position).
      * Otherwise, nothing happens.
      * @param event The mouseMoveEvent to be processed.
      */
    void mouseMoveEvent(QMouseEvent *event);

    /**
      * As we have no interaction for the double click, this function does nothing.
      * @param event The mouseDoubleClickEvent to be processed.
      */
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void changedViewSize(QSize size);
    void verticalScroll(QRectF visibleRectangle);
    void exportRange(qint64 begin, qint64 end);
    void play();
    void zoomIn();
    void zoomOut();

public slots:
    void onAddTrack();
    void onDelete(Track *t);

    /**
      * Resizes tracks to new view length.
      * Propagates event (PresentationItem resizes timeLine and cursor)
      * @param size Size of the new view (it's the size of the mainView not the window size)
      */
    void onChangedViewSize(QSize size);

    void onPlay();

private slots:
    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted.
      * Updates the tracks to the new range.
      * @param begin Begin of the range.
      * @param end End of the range (usually 30s more than begin)
      */
    void onRangeChanged(qint64 begin, qint64 end);

    void updatePlotMargins();

private:    
    const DataProvider &dataProvider;
    QList<Track*> tracks;

    QSize currentViewSize;

    int offsetLeft;

    bool createSelection;

    bool unsavedChanges;

    TimeLine *timeLine;
    Cursor *cursor;
    Selection *selection;

    TimeManager *timeManager;

    void addTrack(const QList<QString>& fullDataSeriesNames);
    void addTracks(const QList<QString>& fullDataSeriesNames);
    Track* add(const QList<QString>& fullDataSeriesNames);

    void setPlotMargins(int newMargin);
};

#endif // PRESENTATIONAREA_H
