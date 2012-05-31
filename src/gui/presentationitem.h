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

class PresentationItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit PresentationItem(QGraphicsScene *parent = 0);
    ~PresentationItem();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void addTrack(Track *t);

    void deleteTrack(Track *t);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public slots:
    void recalcBoundingRec();

    void onRangeChanged(qint64 begin, qint64 end);

    void onChangedWindowSize(QSize size);

    void onVerticalScroll(QRectF visibleRectangle);    

signals:
    void selection(qint64 start, qint64 end);

    void exportTriggered();

private:
    QGraphicsScene *parent;
    QGraphicsProxyWidget *trackToAdd;
    QList<QGraphicsProxyWidget*> tracks;    

    TimeLine *timeLine;

    Cursor *cursor;

    QRectF boundingRectangle;

    Selection *selectedArea ;

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

    bool createSelection;
    int selectionStart, selectionEnd;

    // minmal height to cover the full presentationArea
    int minCoverHeight;
    static const int ACTIONAREAOFFSET;
};

#endif // PRESENTATIONITEM_H
