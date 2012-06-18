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
class TimeManager;

class PresentationItem : public QObject, public QGraphicsItem, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationItem(TimeLine *timeLine, TimeManager *timeManager,
                              QGraphicsScene *parent = 0);
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

    /**
      * Determines whether the specified track is currently visible.
      * @param t The track whose visibility is identified.
      * @return True if 't' is visible. False if either the track is not visible or the track is not
      *         in PresentationItems list of widgets (for example if the list ist empty).
      */
    bool isVisible(Track *t);

    enum Playstate {PLAYING, PAUSED, STOPPED};

public slots:
    /**
      * Calculates the current size of the bounding rectangle.
      */
    void recalcBoundingRec();

    /**
      * Resizes cursor, timeLine and selection to new size.      
      * Cursor and selection are only resized in their height, not their width.
      * @param size The size of the visible track-area (mainView)
      */
    void onChangedViewSize(QSize size);

    /**
      * Repositions timeLine to the top of the visible area.
      * @param visibleRectangle Current size and position of the visible track-area
      */
    void onVerticalScroll(QRectF visibleRectangle);

    void onPlay();

private slots:
    void onTimeout();

    /**
      * Makes sure no selection is visible while
      */
    void onHorizontalScroll();

signals:
    void selection(qint64 begin, qint64 end);

    void exportTriggered();

private:
    QGraphicsScene *parent;
    QGraphicsProxyWidget *trackToAdd;
    QList<QGraphicsProxyWidget*> tracks;    

    TimeLine *timeLine;
    Cursor *cursor;
    Selection *selectedArea;

    QRectF boundingRectangle, visRect;    

    bool autoScroll;

    QTimer timer;

    bool createSelection;
    int selectionStart, selectionEnd;

    // currentTime is the point of time, which is represented by the cursor.
    qint64 currentTime;

    // minmal height to cover the full presentationArea
    int minCoverHeight;

    Playstate playstate;

    TimeManager *timeMgr;

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

    int getRightBorder();
};

#endif // PRESENTATIONITEM_H
