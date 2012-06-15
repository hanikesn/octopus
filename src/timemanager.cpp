#include "timemanager.h"


TimeManager::TimeManager(QScrollBar *hScrollBar, TimeLine *timeLine):
    lowVisRange(0),
    highVisRange(0),
    hScrollBar(hScrollBar),
    timeLine(timeLine),
    timePerPx(40000),
    timeoutUpdateIntervall(40000),
    timeoutIntervall(40),
    autoScroll(false)
{
    hScrollBar->setSingleStep(1);
    hScrollBar->setPageStep(30);
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(0);

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
    return lowVisRange + (pos * timePerPx);
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

    hScrollBar->setValue(lowVisRange/1000000);
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
    hScrollBar->setValue(lowVisRange/1000000);
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

void TimeManager::onRangeChanged(qint64 begin, qint64 end)
{
    lowVisRange = begin;
    highVisRange = end;
    timeLine->drawFrom(begin);
}

void TimeManager::onNewMax(qint64 timestamp)
{
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

void TimeManager::horizontalScroll(int pos)
{
    lowVisRange = pos*1000000;
    highVisRange = getUpperEnd(lowVisRange);
    emit rangeChanged(lowVisRange, highVisRange);

    emit horizontalScroll();
}
