#ifndef TIMELINE_H
#define TIMELINE_H

#include <QPen>

#include <QWidget>

class QGraphicsItem;
class PresentationItem;
class TimeManager;

class TimeLine : public QWidget
{
    Q_OBJECT
public:    
    explicit TimeLine(TimeManager& timeManager, QWidget *parent);

    qint64 getStepSize() {return largeTickAmount;}

public slots:
    void onRangeChanged(qint64 begin, qint64 end);

    void updateWidth(int w);

protected:
    void paintEvent(QPaintEvent *);

private:
    void onStepSizeChanged(qint64 microSeconds);

    TimeManager& timeManager;

    qint64 beginRange;

    // stuff to draw ticks:
    double value;
    int currentPos;
    int bottom;
    int textBoxWidth;
    int textBoxHeight;
    int smallTickHeight;
    int mediumTickHeight;
    int largeTickHeight;
    int offset;
    QPen pen;

    // Number of microseconds between two large ticks.
    qint64 largeTickAmount;
    // Number of microseconds between a medium tick an the next large tick (largeTickAmount/2)
    qint64 mediumTickAmount;
    // Number of microseconds between two small ticks.
    qint64 smallTickAmount;

    qint64 timePerPx;

    enum TimeRepresentation {SECOND, MILLISECOND};
    TimeRepresentation timeRepresentation;

    void drawTicks(QPainter *painter);

    void drawLargeTick(QPainter *painter, double value);
    void drawMediumTick(QPainter *painter);
    void drawSmallTick(QPainter *painter);
};

#endif // TIMELINE_H
