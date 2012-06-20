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

    void setStepSize(qint64 microSeconds);
    qint64 getStepSize() {return largeTickAmount;}

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

    qint64 largeTickAmount;
    qint64 mediumTickAmount;
    qint64 smallTickAmount;
    qint64 timePerPx;

    enum TimeRepresentation {SECOND_PART, SECOND_FULL, MILLISECOND};
    TimeRepresentation timeRepresentation;

    void drawTicks(QPainter *painter);
};

#endif // TIMELINE_H
