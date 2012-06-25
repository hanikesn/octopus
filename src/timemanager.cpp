#include "timemanager.h"

#include "gui/timeline.h"

#include <QDebug>
#include <QScrollBar>
#include <QTimer>

TimeManager::TimeManager(QScrollBar *hScrollBar, QObject* parent):
    QObject(parent),
    lowVisRange(0),
    highVisRange(30000000),
    timePerPx(40000),
    maximum(0),
    stepSize(2000000), // 2 seconds
    playing(false),
    autoScroll(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this))
{
    hScrollBar->setSingleStep(1);

    timer->setSingleShot(false);
    timer->setInterval(40);
    connect(timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));

    connect(hScrollBar, SIGNAL(valueChanged(int)),      this, SLOT(horizontalScroll(int)));
}

qint64 TimeManager::convertPosToTime(int pos)
{
    qint64 result = lowVisRange + ((pos - offsetLeft) * timePerPx);
    return result;
}

int TimeManager::convertTimeToPos(qint64 time)
{
    if (time < lowVisRange) return -1;

    qint64 currentTime = lowVisRange;
    int pos = 0;
    while(currentTime < time) {
        currentTime += timePerPx;
        pos++;
    }
    return pos + offsetLeft;
}

void TimeManager::setRange(qint64 begin, qint64 end)
{
    updateScrollBar(false);
    emit rangeChanged(begin, end);
}

void TimeManager::load(QVariantMap *qvm)
{
    QVariantMap visibleArea = qvm->find("visibleArea").value().toMap();
    lowVisRange = visibleArea["low"].toLongLong();
    highVisRange = visibleArea["high"].toLongLong();

    emit rangeChanged(lowVisRange, highVisRange);

    // step-size of scrollbar is 1 second --> left border of timeline is always a full second
    // so we can set the value of the scrollbars slider to the second visRangeLow represents
    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/getTimePerPx());
    hScrollBar->blockSignals(false);


    currentTime =  qvm->find("cursorPos").value().toLongLong();
    emit currentTimeChanged(currentTime);
}

void TimeManager::save(QVariantMap *qvm)
{
    QVariantMap visibleArea;
    visibleArea.insert("low", lowVisRange);
    visibleArea.insert("high", highVisRange);
    qvm->insert("visibleArea", visibleArea);
    qvm->insert("cursorPos", currentTime);
}

void TimeManager::center(qint64 timestamp)
{
    qint64 range = highVisRange - lowVisRange;

    lowVisRange = timestamp - range/2;
    highVisRange = timestamp + range/2;
    setRange(lowVisRange, highVisRange);
}

void TimeManager::setTime(qint64 time)
{
    currentTime = time;
    emit currentTimeChanged(time);
}

qint64 TimeManager::getZoomFactor(bool zoomOut)
{
    qint64 currentStepSize = stepSize;
    if (currentStepSize <= 200000) { // <= 200ms stepsize: 25ms
        if (currentStepSize == 200000 && zoomOut)
            return 50000;
        return 25000;
    } else if (currentStepSize <= 1000000) { // <= 1000ms stepsize 50ms
        if (currentStepSize == 1000000  && zoomOut)
            return 200000;
        return 50000;
    } else if (currentStepSize <= 5000000) { // <= 5000ms stepsize 200ms
        if (currentStepSize == 5000000 && zoomOut)
            return 1000000;
        return 200000;
    } else // > 5000ms stepsize 1000ms
        return 1000000;
}

void TimeManager::updateScrollBar(bool scroll)
{
    int max = getMaximum() / getTimePerPx();
    hScrollBar->setMaximum(max);

    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/getTimePerPx());
    hScrollBar->blockSignals(false);

    if (scroll) {
        qint64 timeFrame = highVisRange - lowVisRange;
        hScrollBar->setValue(hScrollBar->maximum());
        qint64 lowerRange = maximum < timeFrame ? 0 : maximum - timeFrame;
        // emit the signal directly to prevent a loop
        emit rangeChanged(lowerRange, maximum);
    }
}

void TimeManager::onNewMax(qint64 timestamp)
{
    if (timestamp < maximum)
        return;

    maximum = timestamp;
    updateScrollBar(autoScroll);
}

void TimeManager::onZoomIn()
{
    qint64 newStepSize = stepSize - getZoomFactor(false);
    if (newStepSize <= 0) return;
    stepSize = newStepSize;

    timePerPx = stepSize / 50;
    // initiate redraw according to new range
    onNewWidth(width);
}

void TimeManager::onZoomOut()
{
    stepSize += getZoomFactor(true);
    timePerPx = stepSize / 50;
    // initiate redraw according to new range
    onNewWidth(width);
}

void TimeManager::horizontalScroll(int pos)
{
    qint64 range = highVisRange - lowVisRange;
    lowVisRange = pos * getTimePerPx();
    highVisRange = lowVisRange + range;
    setRange(lowVisRange, highVisRange);
}

void TimeManager::onTimeout()
{
    currentTime += getTimePerPx();
    if (currentTime > getMaximum()) { // stop playing
        playing = true;
        timer->stop();
        return;
    }

    // We need to adjust the range slightly so that the cursor stays visible
    if(currentTime>highVisRange- getTimePerPx()) {
        qint64 range = highVisRange - lowVisRange;
        highVisRange = currentTime + getTimePerPx();
        lowVisRange = highVisRange - range;
        setRange(lowVisRange, highVisRange);
    }

    emit currentTimeChanged(currentTime);
}

void TimeManager::onOffsetChanged(int offset)
{
    offsetLeft = offset;
    onNewWidth(width);
}

void TimeManager::onNewWidth(int w)
{
    width = w;
    hScrollBar->setPageStep(w);
    highVisRange = lowVisRange + timePerPx * (w - offsetLeft);
    setRange(lowVisRange, highVisRange);
}

void TimeManager::onPlay()
{
    if(playing)
    {
        playing = false;
        timer->stop();
    } else {
        playing = true;
        if (currentTime > getHighVisRange() || currentTime < getLowVisRange()) {
            center(currentTime);
            emit currentTimeChanged(currentTime);
        }
        timer->start();
    }
}
