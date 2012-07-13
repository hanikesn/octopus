#include "mainview/timeline.h"

#include "time/timemanager.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

TimeLine::TimeLine(TimeManager& timeManager, QWidget * parent):
    QWidget(parent),
    timeManager(timeManager),
    beginRange(0),    
    textBoxWidth(50),
    textBoxHeight(10),
    smallTickHeight(5),
    mediumTickHeight(10),
    largeTickHeight(15),
    offset(timeManager.getMarginLeft()),
    pen(Qt::black, 1, Qt::SolidLine),
    largeTickAmount(2000000), // two seconds in µs
    mediumTickAmount(1000000), // one second in µs
    smallTickAmount(200000), // 0.2 seconds in µs
    timePerPx(40000), // Amount of time which one pixel represents 40 milliseconds in µs
    timeRepresentation(SECOND)
{
    setObjectName("TimeLine");
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TimeLine::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRectF frame(QPointF(0,0), geometry().size());
    QGradientStops stops;

    // paint a background rect (with gradient)
    QLinearGradient gradient(frame.topLeft(), frame.topLeft() + QPointF(200,200));
    stops << QGradientStop(0.0, QColor(110, 110, 110));
    stops << QGradientStop(frame.height()/2/frame.height(), QColor(150, 150, 150));    
    stops << QGradientStop(1.0, QColor(215, 215, 215));
    gradient.setStops(stops);

    painter.setBrush(QBrush(gradient));
    painter.setPen(pen);
    painter.drawRect(frame);

    drawTicks(&painter);

    QRect rect = QRect(QPoint(0, 0), QSize(offset, geometry().height()));
    QString text = timeRepresentation == MILLISECOND ? "ms" : "sec";
    QTextOption option;
    option.setAlignment(Qt::AlignHCenter);
    option.setAlignment(Qt::AlignCenter);
    painter.drawText(rect, text, option);
}

void TimeLine::drawTicks(QPainter *painter)
{    
    offset = timeManager.getMarginLeft();
    currentPos = 0;    
    qint64 currentTime = beginRange; // lowest visible timestamp

    // determines top of bounding box for the labels underneath large ticks
    bottom = geometry().height() - 10;

    int stCounter = beginRange / smallTickAmount + 1;
    int mtCounter = beginRange / mediumTickAmount + 1;
    int ltCounter = beginRange / largeTickAmount + 1;

    // determine factor by which the currentTime is divided
    double timeFactor = 1000000; // seconds
    if (timeRepresentation == MILLISECOND)
        timeFactor = 1000; // milliseconds

    double output  = 0.0; // contains the second/millisecond which is drawn underneath large ticks
    if (currentTime % largeTickAmount == 0) {
        output = (double)currentTime / timeFactor;
        drawLargeTick(painter, output);
    } else if (currentTime % mediumTickAmount == 0)
        drawMediumTick(painter);
    else if (currentTime % smallTickAmount == 0)
        drawSmallTick(painter);

    // go from left offset to right offset
    while (currentPos + offset < geometry().width() - timeManager.getMarginRight()) {
        if (currentTime / (ltCounter * largeTickAmount) >= 1) { // large tick
            ltCounter++;
            mtCounter++;
            stCounter++;
            output = (double)currentTime / timeFactor;            
            drawLargeTick(painter, output);
        } else if (currentTime / (mtCounter * mediumTickAmount) >= 1){ // medium tick
            mtCounter++;
            stCounter++;
            drawMediumTick(painter);
        } else if (currentTime / (stCounter * smallTickAmount) >= 1) { // small tick
            stCounter++;
            drawSmallTick(painter);
        }
        currentTime += timePerPx;
        currentPos++;
    }
}

void TimeLine::drawLargeTick(QPainter *painter, double value)
{
    painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                      bottom - largeTickHeight);
    QRect rect = QRect(currentPos + offset - textBoxWidth/2, bottom, textBoxWidth,
                       textBoxHeight);
    painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(value, 0, 'f',
                                                               0));
}

void TimeLine::drawMediumTick(QPainter *painter)
{
    painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                      bottom - mediumTickHeight);
}

void TimeLine::drawSmallTick(QPainter *painter)
{
    painter->drawLine(currentPos + offset, bottom, currentPos + offset,
                      bottom - smallTickHeight);
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
    // <magic coefficient calculation>
    qint64 div = 4000000;
    qint64 quotient = (microSeconds/div) + 1;
    largeTickAmount = (quotient * div) / 2; // 2 seconds if no zoom has been applied so far
    mediumTickAmount = largeTickAmount / 2;
    smallTickAmount = mediumTickAmount / 5;
    // <\magic coefficient calculation>
    timePerPx = microSeconds/50;    

    if (microSeconds % 1000000 == 0)
        timeRepresentation = SECOND;
    else if (microSeconds > 100000)
        timeRepresentation = SECOND;
    else if (microSeconds <= 100000)
        timeRepresentation = MILLISECOND;

    update();
}

void TimeLine::updateWidth(int w)
{
    setFixedWidth(w);
}
