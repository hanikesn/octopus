#ifndef PRESENTATIONITEM_H
#define PRESENTATIONITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QList>


class Track;
class QGraphicsProxyWidget;
class Selection;
class Cursor;
class TimeLine;
class QScrollBar;

class PresentationItem : public QObject, public QGraphicsItem
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

public slots:
    /**
      * Calculates the current size of the bounding rectangle.
      */
    void recalcBoundingRec();

    /**
      * Updates timeLine to the visible range
      * @param begin Begin of the visible range
      * @param end End of the visible range
      */
    void onRangeChanged(qint64 begin, qint64 end);

    /**
      * Resizes cursor, timeLine and selection to new size.
      * Tracks won't get more height instead they are adjusted in their width.
      * TimeLine and selection are only resized in their height, not their width.
      * @param size The size of the visible track-area (mainView)
      */
    void onChangedWindowSize(QSize size);


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

private slots:
    void horizontalScroll(int);

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

    QRectF boundingRectangle;    

    QScrollBar *hScrollBar;

    bool autoScroll;

    /**
      * Makes sure that no gaps exist between tracks (for example after deleting a track).
      */
    void recalcPositions();

    void cursorPosChanged(int pos);

    /**
      * Repositions the timeline to the top of the visible area.
      */
    void repositionTimeLine(QRectF visibleRectangle);

    void resizeCursorAndSelection();

    /**
      * Sets the selection invisible and shows the cursor.
      * Emits selection(-1, -1) to update every objects selection-parameters.
      */
    void showCursor();

    bool createSelection;
    int selectionStart, selectionEnd;

    // minmal height to cover the full presentationArea
    int minCoverHeight;
    static const int ACTIONAREAOFFSET;

    /**
      * Determines how far the range should go back in time (in microseconds)
      */
    static const int TIMEFRAME;
};

#endif // PRESENTATIONITEM_H
