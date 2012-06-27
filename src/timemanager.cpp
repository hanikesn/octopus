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
    playing(false),
    autoScroll(false),
    following(false),
    hScrollBar(hScrollBar),
    timer(new QTimer(this))
{
    timer->setSingleShot(false);
    timer->setInterval(40);
    connect(timer, SIGNAL(timeout()),                  this, SLOT(onTimeout()));

    connect(hScrollBar, SIGNAL(valueChanged(int)),      this, SLOT(horizontalScroll(int)));
    connect(this, SIGNAL(currentTimeChanged(qint64)),   this, SLOT(onCurrentTimeChanged(qint64)));

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
    updateScrollBar(false);
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

    // set value of scrollbar to lowVisRange
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
    unsavedChanges = true;
}

void TimeManager::setTime(qint64 time)
{
    if (following) return;
    currentTime = time;
    emit currentTimeChanged(time);
    unsavedChanges = true;    
}

void TimeManager::updateScrollBar(bool scroll)
{
    int max = getMaximum() / getTimePerPx();
    hScrollBar->setMaximum(max);

    hScrollBar->blockSignals(true);
    hScrollBar->setValue(lowVisRange/getTimePerPx());
    hScrollBar->blockSignals(false);

    if (scroll) {
        ensureCursorVisibility();
    }
}

void TimeManager::onNewMax(qint64 timestamp)
{
    if (timestamp < maximum)
        return;

    maximum = timestamp;
    updateScrollBar(autoScroll);
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

void TimeManager::horizontalScroll(int pos)
{
    qint64 range = highVisRange - lowVisRange;
    lowVisRange = qMax(0LL, pos * getTimePerPx());
    highVisRange = lowVisRange + range;
    setRange(lowVisRange, highVisRange);
    unsavedChanges = true;

    if (currentTime >= lowVisRange && currentTime <= highVisRange)
        autoScroll = true;
    else
        autoScroll = false;
}

void TimeManager::onCurrentTimeChanged(qint64 newTime)
{
    Q_UNUSED(newTime)
    if (currentTime >= lowVisRange && currentTime <= highVisRange)
        autoScroll = true;
    else
        autoScroll = false;
}

void TimeManager::onTimeout()
{
    Clock::duration diff = Clock::now() - startTime;
    currentTime += diff.count() / 1000;
    if ((currentTime > getMaximum()) && !following) { // stop playing if end is reached and we are not expecting new data
        playing = false;
        timer->stop();
        return;
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
    hScrollBar->setSingleStep(60);
    hScrollBar->setPageStep(w);
    highVisRange = lowVisRange + timePerPx * (w - marginLeft - marginRight);
    setRange(lowVisRange, highVisRange);
}

void TimeManager::onFollow(bool following)
{
    if (following) {
        autoScroll = true;
        setTime(maximum);
        if (maximum > getHighVisRange() || maximum < getLowVisRange())
            center(maximum);
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
            emit currentTimeChanged(currentTime);
        }
        timer->start();
        startTime = Clock::now();
    }
}
