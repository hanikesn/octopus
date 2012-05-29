#ifndef PRESENTATIONITEM_H
#define PRESENTATIONITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QList>

#include "gui/timeline.h"

class Track;
class QGraphicsProxyWidget;

class PresentationItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit PresentationItem(TimeLine *timeLine, QGraphicsScene *parent = 0);
    ~PresentationItem();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void addTrack(Track *t);

    void deleteTrack(Track *t);

    /**
      * Repositions the timeline to the top of the visible area.
      */
    void repositionTimeLine(QRectF visibleRectangle);    

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:
    void cursorPosChanged(int pos);

private:
    QGraphicsScene *parent;
    QGraphicsProxyWidget *trackToAdd;
    QList<QGraphicsProxyWidget*> tracks;    

    TimeLine *timeLine;

    QRectF boundingRectangle;

    /**
      * Makes sure that no gaps exist between tracks (for example after deleting a track).
      */
    void recalculatePositions();

};

#endif // PRESENTATIONITEM_H
