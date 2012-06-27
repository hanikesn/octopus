#include "timemanager.h"

#include "gui/timeline.h"

#include <QDebug>
#include <QScrollBar>
#include <QTimer>

TimeManager::TimeManager(QScrollBar *hScrollBar, Clock::time_point startTime, QObject* parent):
    QObject(parent),
    lowVisRange(0),
    highVisRange(30000000),
    timePerPx(40000),
    maximum(0),
    playing(false),
    autoScroll(false),
    following(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this)),
    startTime(startTime)
{
    timer->setSingleShot(false);
    timer->setInterval(1000/60);
    connect(timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));
}

qint64 TimeManager::convertPosToTime(int pos)
{
    return lowVisRange + ((pos - marginLeft) * timePerPx);
}

int TimeManager::convertTimeToPos(qint64 time)
{
    return marginLeft + (time -lowVisRange) / timePerPx;
}

void TimeManager::setRange(qint64 begin, qint64 end)
{
    lowVisRange = begin;
    highVisRange = end;

    if (currentTime >= lowVisRange && currentTime <= highVisRange)
        autoScroll = true;
    else
        autoScroll = false;

    emit rangeChanged(begin, end);
}

void TimeManager::ensureCursorVisibility()
{
    // We need to adjust the range slightly so that the cursor stays visible
    if((currentTime > highVisRange - getTimePerPx()) && autoScroll) {
        qint64 range = highVisRange - lowVisRange;
        highVisRange = currentTime + getTimePerPx();
        lowVisRange = highVisRange - range;
        setRange(lowVisRange, highVisRange);
    }
}

void TimeManager::load(QVariantMap *qvm)
{
    QVariantMap visibleArea = qvm->find("visibleArea").value().toMap();
    lowVisRange = visibleArea["low"].toLongLong();
    highVisRange = visibleArea["high"].toLongLong();

    emit rangeChanged(lowVisRange, highVisRange);

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
    unsavedChanges = true;
}

void TimeManager::setTime(qint64 time)
{
    if (following) return;
    currentTime = qMin(time, getMaximum());
    emit currentTimeChanged(time);
    unsavedChanges = true;    
}

void TimeManager::onNewMax(qint64 timestamp)
{
    if (timestamp < maximum)
        return;

    emit newMax(timestamp);

    maximum = timestamp;
}

void TimeManager::onRangeChanged(qint64 begin, qint64 end)
{
    lowVisRange = begin;
    highVisRange = end;
    setRange(begin, end);
}

int TimeManager::getStepSize()
{
    // TODO ROUNDING
    return timePerPx * 50;
}

void TimeManager::onZoom(int factor)
{
    timePerPx -= factor * 50;
    timePerPx = qMax(1000LL, timePerPx);

    // initiate redraw according to new range
    onNewWidth(width);
}

void TimeManager::movePx(int px)
{
    hScrollBar->setValue(hScrollBar->value() + px);
}

void TimeManager::onTimeout()
{
    Clock::duration diff = Clock::now() - startTime;
    currentTime += diff.count() / 1000;
    if ((currentTime > getMaximum()) && !following) { // stop playing if end is reached and we are not expecting new data
        playing = false;
        timer->stop();
        return;
    } else {
        onNewMax(currentTime);
    }
    ensureCursorVisibility();

    emit currentTimeChanged(currentTime);
    startTime = Clock::now();
}

void TimeManager::onMarginsChanged(int left, int right)
{
    marginLeft = left;
    marginRight = right;
    onNewWidth(width);
}

void TimeManager::onNewWidth(int w)
{
    width = w;
    setRange(lowVisRange, lowVisRange + timePerPx * (w - marginLeft - marginRight));
}

void TimeManager::onFollow(bool following)
{
    if (following) {
        autoScroll = true;
        setTime(maximum);
        ensureCursorVisibility();
        timer->start();
        startTime = Clock::now();
        playing = true;

    } else {
        autoScroll = false;
        timer->stop();
        playing = false;
    }
    this->following = following;
}

void TimeManager::forwardEventToScrollbar(QEvent *ev)
{
    hScrollBar->event(ev);
}

void TimeManager::onPlay()
{    
    if(playing) {
        playing = false;
        timer->stop();
        following = false;
    } else {
        playing = true;        
        if (currentTime > getHighVisRange() || currentTime < getLowVisRange()) {
            center(currentTime);
        }
        timer->start();
        startTime = Clock::now();
    }
}
