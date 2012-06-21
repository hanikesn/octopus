#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsWidget>
#include <QPen>

#include <QWidget>

class QGraphicsItem;
class PresentationItem;


class TimeLine : public QWidget
{
    Q_OBJECT
public:    
    explicit TimeLine(int offset, QWidget *parent);

    void paintEvent(QPaintEvent *);

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
