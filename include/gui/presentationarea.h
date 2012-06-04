#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QObject>

#include "dataprovider.h"
#include "presentationitem.h"


class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class Cursor;
class QScrollBar;

class PresentationArea : public QObject
{
    Q_OBJECT
public:
    explicit PresentationArea(QGraphicsScene *scene, DataProvider *dataProvider,
                              QScrollBar *hScrollBar);
    ~PresentationArea();

signals:
    void rangeChanged(qint64 begin, qint64 end);
    void changedWindowSize(QSize size);
    void verticalScroll(QRectF visibleRectangle);
    void exportRange(qint64 begin, qint64 end);

public slots:
    void onAddTrack();
    void onDelete(Track *t);

    /**
      * Resizes tracks to new view length.
      * Propagates event (PresentationItem resizes timeLine and cursor)
      * @param size Size of the new view (it's the size of the mainView not the window size)
      */
    void onChangedWindowSize(QSize size);    

private slots:
    /**
      * Saves the range of the current selection
      * @param begin Begin of the selection
      * @param end End of the selection
      */
    void onSelection(qint64 begin, qint64 end);

    /**
      * Propagates export signal with the range of the current selection (if available)
      */
    void onExportTriggered();

    /**
      * Updates the timeLine, hScrollBar and the tracks to the new maximum timestamp.
      * @param timestamp New maximum timestamp
      */
    void onNewMax(qint64 timestamp);

    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted.
      * Updates the tracks to the new range.
      * @param begin Begin of the range.
      * @param end End of the range (usually 30s more than begin)
      */
    void onRangeChanged(qint64 begin, qint64 end);


    void horizontalScroll(int pos);    

private:    
    PresentationItem *pi;

    DataProvider *dataProvider;
    QList<Track*> tracks;

    QSize currentViewSize;

    QScrollBar *hScrollBar;

    qint64 selectionBegin, selectionEnd;    

    bool autoScroll;
    int lastSliderPos;

    static const int ACTIONAREAOFFSET;

    /**
      * Determines how far the range should go back in time (in microseconds)
      */
    static const int TIMEFRAME;
};

#endif // PRESENTATIONAREA_H
