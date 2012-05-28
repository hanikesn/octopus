#ifndef PRESENTATIONITEM_H
#define PRESENTATIONITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QList>

#include "gui/timeline.h"

class Track;
class QGraphicsProxyWidget;

class PresentationItem : public QGraphicsItem
{

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

private:
    QGraphicsScene *parent;
    QGraphicsProxyWidget *trackToAdd;
    QList<QGraphicsProxyWidget*> tracks;    

    TimeLine *timeLine;

    /**
      * Makes sure that no gaps exist between tracks (for example after deleting a track).
      */
    void recalculatePositions();

};

#endif // PRESENTATIONITEM_H
