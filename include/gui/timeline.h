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

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

    /**
      * Converts a point in time to the according x-position on the screen (subtracts the offset
      * specified in the constructor)
      * @param time The point in time to be converted
      */
    int convertTimeToPos(qint64 time);

    /**
      * Converts a position on the view into the corresponting point in time.
      * @param pos The position in the view to be converted. The position needs to be absolute, no
      *            offset subtracted beforehand.
      */
    qint64 convertPosToTime(int pos);

    void drawFrom(qint64 time);

    qint64 getUpperEnd(qint64 lowerEnd);

public slots:    

private:
    int offset;
    qint64 beginRange;

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


    static const qint64 LARGETICKAMOUNT;
    static const qint64 MEDIUMTICKAMOUNT;
    static const qint64 SMALLTICKAMOUNT;
    static const qint64 TIMEPERPX;

    void drawTicks(QPainter *painter);
};

#endif // TIMELINE_H
