#include "timemanager.h"

#include "gui/timeline.h"

#include <QDebug>
#include <QScrollBar>
#include <QTimer>

namespace bc = boost::chrono;

TimeManager::TimeManager(QScrollBar *hScrollBar, Clock::time_point startTime, QObject* parent):
    QObject(parent),
    lowVisRange(0),
    highVisRange(30000000),
    timePerPx(40000),
    maximum(0),
    currentTime(0),
    live(startTime != Clock::time_point()),
    playing(false),
    following(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this)),
    absoluteStartTime(startTime)
{
    timer->setSingleShot(false);
    timer->setInterval(1000/60);
    connect(timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));

    if(live) {
        timer->start();
    }
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
    if(begin >= end)
        return;

    lowVisRange = begin;

    timePerPx = (end - begin) / (width - marginLeft - marginRight);

    highVisRange = end;

    emit rangeChanged(lowVisRange, highVisRange);
}

void TimeManager::ensureCursorVisibility()
{
    // We need to adjust the range slightly so that the cursor stays visible
    if((currentTime > highVisRange - getTimePerPx())) {
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
    highVisRange = lowVisRange + range;
    setRange(lowVisRange, highVisRange);
    unsavedChanges = true;
}

void TimeManager::setTime(qint64 time)
{
    if (following) return;
    currentTime = qMin(time, getMaximum());
    unsavedChanges = true;

    if(playing) {
        startTime = Clock::now() - bc::microseconds(currentTime);
    } else {
        emit currentTimeChanged(currentTime);
    }
}

void TimeManager::onNewMax(qint64 timestamp)
{
    if (timestamp < maximum)
        return;

    maximum = timestamp;

    emit newMax(timestamp);
}

int TimeManager::getStepSize()
{
    // TODO ROUNDING
    return getTimePerPx() * 50;
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
    Clock::time_point now = Clock::now();

    if(live) {
        onNewMax(bc::duration_cast<bc::microseconds>(now - absoluteStartTime).count());
    }

    if(!playing)
        return;

    if(following)
        currentTime = maximum;
    else {
        currentTime = bc::duration_cast<bc::microseconds>(now - startTime).count();
    }

    // stop playing if end is reached and we are not expecting new data
    if ((currentTime > getMaximum()) && !following) {
        currentTime = getMaximum();
        playing = false;
        timer->stop();
        return;
    }

    ensureCursorVisibility();

    emit currentTimeChanged(currentTime);
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
    if(!live)
        return;

    if (following) {
        playing = true;
        startTime = absoluteStartTime;
    } else {
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
        if(!live)
            timer->stop();
        following = false;
    } else {
        playing = true;        
        if (currentTime > getHighVisRange() || currentTime < getLowVisRange()) {
            center(currentTime);
        }
        timer->start();
        startTime = Clock::now() - bc::microseconds(currentTime);
    }
}
