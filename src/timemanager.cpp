#include "timemanager.h"

#include "gui/timeline.h"

#include <QDebug>
#include <QScrollBar>
#include <QTimer>

TimeManager::TimeManager(QScrollBar *hScrollBar, QObject* parent):
    QObject(parent),
    lowVisRange(0),
    highVisRange(0),
    timePerPx(40000),
    maximum(0),
    timeoutUpdateIntervall(40000),
    timeoutIntervall(40),
    stepSize(2000000), // 2 seconds
    playstate(PAUSED),
    autoScroll(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this))
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

void TimeManager::addRange(qint64 delta)
{
    if (delta <= 0) return;
    emit rangeChanged(lowVisRange + delta, highVisRange + delta);

    // no signals necessary if we trigger them ourselves
    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/1000000);
    hScrollBar->blockSignals(false);
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
    emit rangeChanged(lowVisRange, highVisRange);
    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/1000000);
    hScrollBar->blockSignals(false);
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
    lowVisRange = pos*1000000;
    highVisRange = lowVisRange + range;
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

void TimeManager::onOffsetChanged(int offset)
{
    offsetLeft = offset;
    // TODO besser berechnen
    onNewWidth(width);
}

void TimeManager::onNewWidth(int w)
{
    width = w;
    highVisRange = lowVisRange + timePerPx * (w - offsetLeft);
    emit rangeChanged(lowVisRange, highVisRange);
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
