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
