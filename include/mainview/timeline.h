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

    /**
      * @return The amount of time which is between two large ticks (in microseconds)
      */
    qint64 getStepSize() {return largeTickAmount;}

public slots:

    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted by the timeManager.
      * Updates the beginRange(lowest timestamp on the timeline) and stepSize and initiates a
      * (re-)draw.
      * @param begin Begin of the range.
      * @param end End of the range. (unused)
      */
    void onRangeChanged(qint64 begin, qint64 end);

    /**
      * Called from the PresentationArea when a resize occurs.
      * Sets a fixed with for the timeLine.
      * @param w The new width of the timeLine.
      */
    void updateWidth(int w);

protected:

    /**
      * Override of the QWidget function. (Custom drawing)
      */
    void paintEvent(QPaintEvent *);

private:

    /**
      * Calculates the time which passes between to ticks.
      */
    void onStepSizeChanged(qint64 microSeconds);

    TimeManager& timeManager;

    // lowest visible timestamp
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
    QPen pen; // contains tick color

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

    /**
      * Convenience function to draw a large tick with a specified value in the label underneath it.
      * @param painter The painter wich draws the tick (a line) and the labels.
      * @param value The value which needs to be in the label.
      */
    void drawLargeTick(QPainter *painter, double value);

    /**
      * Convenience function to draw a medium tick.
      * @param painter The painter wich draws the tick (a line).
      */
    void drawMediumTick(QPainter *painter);

    /**
      * Convenience function to draw a small tick.
      * @param painter The painter wich draws the tick (a line).
      */
    void drawSmallTick(QPainter *painter);
};

#endif // TIMELINE_H
