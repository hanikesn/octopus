#include "timemanager.h"

#include "gui/timeline.h"

#include <QDebug>
#include <QScrollBar>
#include <QTimer>

TimeManager::TimeManager(QScrollBar *hScrollBar, TimeLine *timeLine):
    lowVisRange(0),
    highVisRange(0),
    timePerPx(40000),
    maximum(0),
    timeoutUpdateIntervall(40000),
    timeoutIntervall(40),
    playstate(PAUSED),
    autoScroll(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this)),
    timeLine(timeLine)
{
    hScrollBar->setSingleStep(1);
    hScrollBar->setPageStep(30);
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(0);

    timer->setSingleShot(false);
    timer->setInterval(timeoutIntervall);
    connect(timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));

    connect(hScrollBar, SIGNAL(sliderMoved(int)),       this, SLOT(horizontalScroll(int)));
    connect(hScrollBar, SIGNAL(valueChanged(int)),      this, SLOT(horizontalScroll(int)));
    connect(this, SIGNAL(rangeChanged(qint64,qint64)),  this, SLOT(onRangeChanged(qint64,qint64)));

    highVisRange = timeLine->getUpperEnd(0);    
}

TimeManager::~TimeManager()
{
}

qint64 TimeManager::convertPosToTime(int pos)
{
    qint64 result = lowVisRange + (pos * timePerPx);
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
    return pos;
}

void TimeManager::addRange(qint64 delta)
{
    if (delta <= 0) return;
    emit rangeChanged(lowVisRange + delta, highVisRange + delta);

    // no signals necessary if we trigger them ourselves
    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/1000000);
    hScrollBar->blockSignals(false);
}

void TimeManager::updateRange()
{
    highVisRange = getUpperEnd(lowVisRange);
    emit rangeChanged(lowVisRange, highVisRange);
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
    hScrollBar->setValue(lowVisRange/1000000);
    hScrollBar->blockSignals(false);
}

void TimeManager::save(QVariantMap *qvm)
{
    QVariantMap visibleArea;
    visibleArea.insert("low", lowVisRange);
    visibleArea.insert("high", highVisRange);
    qvm->insert("visibleArea", visibleArea);
}

qint64 TimeManager::difference(int pos1, int pos2)
{
    if (pos2 < pos1){
        qint64 tmp = pos2;
        pos2 = pos1;
        pos1 = tmp;
    }
    return (pos2 - pos1) * timePerPx;
}

void TimeManager::center(qint64 timestamp)
{
    qint64 range = highVisRange - lowVisRange;

    lowVisRange = timestamp - range/2;
    highVisRange = timestamp + range/2;
    emit rangeChanged(lowVisRange, highVisRange);
    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/1000000);
    hScrollBar->blockSignals(false);
}

void TimeManager::changeTimeStep(int milliSeconds)
{
    qint64 microSeconds = milliSeconds * 1000;
    timeLine->setStepSize(microSeconds);
    timePerPx = microSeconds / 50;
    // TODO on rangeChanged
}

void TimeManager::onRangeChanged(qint64 begin, qint64 end)
{
    lowVisRange = begin;
    highVisRange = end;
    timeLine->drawFrom(begin);
}

void TimeManager::setTime(qint64 time)
{
    currentTime = time;
    emit currentTimeChanged(time);
}

qint64 TimeManager::getZoomFactor(bool zoomOut)
{
    qint64 currentStepSize = timeLine->getStepSize();
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

void TimeManager::onNewMax(qint64 timestamp)
{
    if (timestamp > maximum)
        maximum = timestamp;
    int max = timestamp/1000000; // max in seconds
    int min = 0;

    hScrollBar->setMaximum(max - hScrollBar->pageStep());
    hScrollBar->setMinimum(min);

    if (autoScroll) {
        qint64 timeFrame = highVisRange - lowVisRange;
        hScrollBar->setValue(hScrollBar->maximum());
        qint64 lowerRange = timestamp < timeFrame ? 0 : timestamp - timeFrame;
        emit rangeChanged(lowerRange, timestamp);
    }
}

void TimeManager::onZoomIn()
{
    qint64 newStepSize = timeLine->getStepSize() - getZoomFactor(false);
    if (newStepSize <= 0) return;

    timeLine->setStepSize(newStepSize);
    timePerPx = newStepSize / 50;
    emit rangeChanged(lowVisRange, getUpperEnd(lowVisRange));
}

void TimeManager::onZoomOut()
{
    qint64 newStepSize = timeLine->getStepSize() + getZoomFactor(true);
    timeLine->setStepSize(newStepSize);
    timePerPx = newStepSize / 50;
    emit rangeChanged(lowVisRange, getUpperEnd(lowVisRange));
}

void TimeManager::horizontalScroll(int pos)
{
    lowVisRange = pos*1000000;
    highVisRange = getUpperEnd(lowVisRange);
    emit rangeChanged(lowVisRange, highVisRange);
}

void TimeManager::onTimeout()
{
    currentTime += getTimePerPx();
    if (currentTime > getMaximum()) { // stop playing
        playstate = PAUSED;
        timer->stop();
        return;
    }

    emit currentTimeChanged(currentTime);
}

qint64 TimeManager::getUpperEnd(qint64 lowerEnd)
{
    return timeLine->getUpperEnd(lowerEnd);
}

void TimeManager::onPlay()
{
    switch(playstate)
    {
    case PLAYING:
        playstate = PAUSED;
        timer->stop();
        break;
    case PAUSED:
        playstate = PLAYING;
        if (currentTime > getHighVisRange() || currentTime < getLowVisRange()) {
            center(currentTime);
            emit currentTimeChanged(currentTime);
        }
        timer->start();
        break;
    }
}

