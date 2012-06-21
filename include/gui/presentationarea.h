#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QScrollArea>
#include <QSize>
#include <QRectF>

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
