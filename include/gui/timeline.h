#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsWidget>
#include <QPen>


class QGraphicsItem;
class PresentationItem;


class TimeLine : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit TimeLine(int offset, QGraphicsItem *parent, Qt::WindowFlags wFlags = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

    void drawFrom(qint64 time);
    void setOffset(int offset);

    qint64 getUpperEnd(qint64 lowerEnd);

public slots:
    void onUpdate(QSize size);

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
