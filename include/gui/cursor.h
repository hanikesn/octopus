#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsWidget>
#include <QPen>


class TimeManager;
class PresentationItem;

class Cursor : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit Cursor(int offsetLeft, TimeManager *timeManager,  PresentationItem *presentationItem,
                    QGraphicsScene *parent = 0);
    ~Cursor();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

    void setOffset(int offset) {offsetLeft = offset;}

    /**
      * Sets the cursor to a specified coordinate and calls 'setVisible(true)'.
      * If pos < leftOffset this function has no effect.
      * @param pos The position to which the cursor is set.
      */
    void changePos(int pos);

    void moveToTime(qint64 time);

    qint64 getCurrentTime();

public slots:
    void onUpdate(QSize size);

    void onZoomed();

private:    
    QPen pen;    
    QBrush brush;
    int offsetLeft;

    qint64 currentTime;

    TimeManager *timeMgr;
    PresentationItem *presentationItem;
};

#endif // CURSOR_H
