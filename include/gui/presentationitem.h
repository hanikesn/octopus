#ifndef PRESENTATIONITEM_H
#define PRESENTATIONITEM_H

#include <QGraphicsItem>
#include <QWidget>
#include <QList>

#include "serializable.h"

class QGraphicsScene;
class Track;
class QGraphicsProxyWidget;
class Selection;
class Cursor;
class TimeLine;
class QScrollBar;
class TimeManager;

class PresentationItem : public QWidget
{
    Q_OBJECT
public:
    explicit PresentationItem(TimeLine *timeLine, TimeManager *timeManager,
                              QWidget *parent = 0);
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
      * @param t The track to be removed
      */
    void removeTrack(Track *t);

    /**
      * Determines whether the specified track is currently visible.
      * @param t The track whose visibility is identified.
      * @return True if 't' is visible. False if either the track is not visible or the track is not
      *         in PresentationItems list of widgets (for example if the list ist empty).
      */
    bool isVisible(Track *t);

    int getMinCoverHeight()  {return minCoverHeight;}

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


    /**
     * Sets the left offset of the time line to the given value and updates time line and cursor.
     */
    void onOffsetChanged(int offset);

signals:
    void onExport(qint64,qint64);

    void update(QSize size);

    void updateVisibleSize(QSize size);

    void offsetChanged(int);

private:
    QList<Track*> tracks;

    TimeLine *timeLine;

    QRectF boundingRectangle, visRect;    
    int offsetLeft;

    bool createSelection;
    int selectionStart, selectionEnd;

    // minmal height to cover the full presentationArea
    int minCoverHeight;

    TimeManager *timeMgr;

    /**
      * Makes sure that no gaps exist between tracks (for example after deleting a track).
      */
    void recalcPositions();

    /**
      * Sets the selection invisible.
      * Emits selection(-1, -1) to update every objects selection-parameters.
      */
    void hideSelection();

    int getRightBorder();
};

#endif // PRESENTATIONITEM_H
