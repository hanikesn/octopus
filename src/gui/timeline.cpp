#include "gui/timeline.h"

#include <QPainter>
#include <QPen>
#include "timemanager.h"
#include "measure.h"

#include <QDebug>

TimeLine::TimeLine(TimeManager& timeManager, QWidget * parent):
    QWidget(parent),
    timeManager(timeManager),
    beginRange(0),    
    textBoxWidth(50),
    textBoxHeight(10),
    shortTickHeight(5),
    mediumTickHeight(10),
    largeTickHeight(15),
    pen(Qt::black, 1, Qt::SolidLine),
    largeTickAmount(2000000), // two seconds in µs
    mediumTickAmount(1000000), // one second in µs
    smallTickAmount(200000), // 0,2 seconds in µs
    timePerPx(40000), // Amount of time which one pixel represents 40 milliseconds in µs
    timeRepresentation(SECOND_FULL)
{
    setObjectName("TimeLine");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    resize(946, 50);
}

void TimeLine::paintEvent(QPaintEvent *)
{
    MEASURE("TIMELINE");
    QPainter painter(this);

    QRectF frame(QPointF(0,0), geometry().size());
    QGradientStops stops;

    // paint a background rect (with gradient)
    QLinearGradient gradient(frame.topLeft(), frame.topLeft() + QPointF(200,200));
    stops << QGradientStop(0.0, QColor(60, 60,  60));
    stops << QGradientStop(frame.height()/2/frame.height(), QColor(150, 150, 150));    
    stops << QGradientStop(1.0, QColor(215, 215, 215));
    gradient.setStops(stops);

    painter.setBrush(QBrush(gradient));
    painter.setPen(pen);
    painter.drawRect(frame);

    // draw ticks:
    drawTicks(&painter);

    QRect rect = QRect(5, geometry().height() - 15, 50, 15);
    QString text = timeRepresentation == MILLISECOND ? "ms" : "sec";
    painter.drawText(rect, text);
}

void TimeLine::drawTicks(QPainter *painter)
{    
    int offset = timeManager.getOffset();
    currentPos = 0;
    qint64 currentTime = beginRange;
    currentTime -= currentTime%timePerPx;

    bottom = geometry().height() - 10;
    /* default:
      2 seconds: large tick
      1 second : medium tick
      0,2 seconds: small tick
      */
    double timeFactor = 0;
    int digitsAfterPoint = 0;
    if (timeRepresentation == SECOND_FULL)
        timeFactor = 1000000;
    else if (timeRepresentation == SECOND_PART) {
        timeFactor = 1000000;
        digitsAfterPoint = 2;
    } else
        timeFactor = 1000;

    double output  = 0.0;
    while (currentPos < geometry().width()) {
        if (currentTime % largeTickAmount == 0) {
            output = (double)currentTime / timeFactor;
            // large tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - largeTickHeight);
            QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth,
                               textBoxHeight);
            painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(output, 0, 'f',
                                                                       digitsAfterPoint));


        } else if (currentTime % mediumTickAmount == 0) {
            // medium tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - mediumTickHeight);
        } else if (currentTime % smallTickAmount == 0) {
            // small tick
            painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                              bottom - shortTickHeight);
        }

        currentTime += timePerPx;
        currentPos++;
    }
}

void TimeLine::onRangeChanged(qint64 begin, qint64 end)
{
    Q_UNUSED(end)
    beginRange = begin;
    onStepSizeChanged(timeManager.getStepSize());
    update();
}

void TimeLine::onStepSizeChanged(qint64 microSeconds)
{
    largeTickAmount = microSeconds;
    mediumTickAmount = microSeconds/2;
    smallTickAmount = largeTickAmount/10;
    timePerPx = microSeconds/50;

    if (microSeconds % 1000000 == 0)
        timeRepresentation = SECOND_FULL;
    else if (microSeconds > 100000)
        timeRepresentation = SECOND_PART;
    else
        timeRepresentation = MILLISECOND;

    update();
}

void TimeLine::updateWidth(int w)
{
    setFixedWidth(w);
}
