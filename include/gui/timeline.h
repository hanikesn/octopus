#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsWidget>
#include <QPen>


class QGraphicsItem;


class TimeLine : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit TimeLine(int offset, QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    void adjustVisibleRange(qint64 begin, qint64 end);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect();

public slots:    

private:
    qint64 beginRange, endRange;
    int offset;

    // stuff to draw ticks:
    double value;
    qint64 rangeOffset;
    int currentPos;
    int bottom;
    int textBoxWidth;
    int textBoxHeight;
    int shortTickHeight;
    int mediumTickHeight;
    int largeTickHeight;

    QPen pen;


    void drawTicks(QPainter *painter);
};

#endif // TIMELINE_H
