#ifndef PRESENTATIONITEM_H
#define PRESENTATIONITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QList>
#include <QTimer>

#include "serializable.h"

class Track;
class QGraphicsProxyWidget;
class Selection;
class Cursor;
class TimeLine;
class QScrollBar;

class PresentationItem : public QObject, public QGraphicsItem, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationItem(QScrollBar *hScrollBar, QGraphicsScene *parent = 0);
    ~PresentationItem();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
      * Creates a QGraphicsProxyWidget and adds it to the scene.
      * Updates the bounding rectangle and resizes cursor and selection.
      * @param t The track to be added
      */
    void addTrack(Track *t);

    /**
      * Looks for the corresponding QGraphicsProxyWidget and removes it from the scene.
      * Updates the bounding rectangle and resizes cursor and selection.
      * @param t The track to be deleted
      */
    void deleteTrack(Track *t);

    /**
      * If shift-button is pressed, a selection is started. Otherwise nothing happens.
      * @param event The mousePressEvent to be processed.
      */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' it ends at the current position of
      * the event (x-position). The corresponding timestamps are calculated and the
      * 'selection()'-signal is emitted.
      * If no new selection has been started this shows the cursor at the events position.
      * @param event The mouseReleaseEvent to be processed.
      */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' and the shift-button is pressed
      * the selected area is increased in size to the events position (x-position).
      * Otherwise, nothing happens.
      * @param event The mouseMoveEvent to be processed.
      */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
      * As we have no interaction for the double click, this function does nothing.
      * @param event The mouseDoubleClickEvent to be processed.
      */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);    

    void save(QVariantMap *qvm);

    void load(QVariantMap *qvm);

    bool isVisible(Track *t);

    enum Playstate {PLAYING, PAUSED, STOPPED};

//    void setPlayState(const Playstate& p);

public slots:
    /**
      * Calculates the current size of the bounding rectangle.
      */
    void recalcBoundingRec();

    /**
      * Resizes cursor, timeLine and selection to new size.
      * Tracks won't get more height instead they are adjusted in their width.
      * TimeLine and selection are only resized in their height, not their width.
      * @param size The size of the visible track-area (mainView)
      */
    void onChangedViewSize(QSize size);

    /**
      * Repositions timeLine to the top of the visible area.
      * @param visibleRectangle Current size and position of the visible track-area
      */
    void onVerticalScroll(QRectF visibleRectangle);

    /**
      * Updates the timeLine, hScrollBar and the tracks to the new maximum timestamp.
      * @param timestamp New maximum timestamp
      */
    void onNewMax(qint64);

    void onPlay();

private slots:
    void horizontalScroll(int);

    /**      
      * Saves current visible range in visRangeLow and visRangeHigh
      * @param begin Begin of the visible range
      * @param end End of the visible range
      */
    void onRangeChanged(qint64 begin, qint64 end);


    void onTimeout();

signals:
    void selection(qint64 begin, qint64 end);

    void exportTriggered();

    void rangeChanged(qint64 begin, qint64 end);

private:
    QGraphicsScene *parent;
    QGraphicsProxyWidget *trackToAdd;
    QList<QGraphicsProxyWidget*> tracks;    

    TimeLine *timeLine;
    Cursor *cursor;
    Selection *selectedArea;

    QRectF boundingRectangle, visRect;

    QScrollBar *hScrollBar;

    bool autoScroll;

    QTimer timer;

    bool createSelection;
    int selectionStart, selectionEnd;
    qint64 visRangeLow, visRangeHigh, currentTime;

    // minmal height to cover the full presentationArea
    int minCoverHeight;

    Playstate playstate;

    static const int ACTIONAREAOFFSET;

    /**
      * Determines how far the range should go back in time (in microseconds)
      */
    static const int TIMEFRAME;

    /**
      * Makes sure that no gaps exist between tracks (for example after deleting a track).
      */
    void recalcPositions();

    /**
      * Sets the cursor to a specified coordinate.
      * @param pos The position to which the cursor is set.
      */
    void changeCursorPos(int pos);

    void resizeCursorAndSelection();

    /**
      * Sets the selection invisible and shows the cursor.
      * Emits selection(-1, -1) to update every objects selection-parameters.
      */
    void showCursor();
};

#endif // PRESENTATIONITEM_H
