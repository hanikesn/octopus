#include "mainview/timescrollbar.h"

TimeScrollBar::TimeScrollBar(QWidget *parent) :
    QScrollBar(parent),
    begin(0),
    end(100),
    max(0)
{
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onScroll(int)));
}

qint64 TimeScrollBar::timePerTick()
{
    return (end - begin) / width();
}

void TimeScrollBar::onRangeChanged(qint64 begin, qint64 end)
{
    this->begin = begin;
    this->end = end;

    blockSignals(true);
    setMaximum(maximumValue());
    setValue(begin/timePerTick());
    blockSignals(false);
}

void TimeScrollBar::onNewMax(qint64 max)
{    
    this->max = max;
    setMaximum(maximumValue());
}

void TimeScrollBar::onScroll(int val)
{
    qint64 range = end - begin;
    begin = qMax(0LL, val * timePerTick());
    end = begin + range;
    emit rangeChanged(begin, end);
}

void TimeScrollBar::resizeEvent(QResizeEvent *)
{
    setPageStep(width());
}

int TimeScrollBar::maximumValue()
{
    qint64 tmp = qMax(end, max);
    int val = tmp / timePerTick() - pageStep();
    return qMax(0, val);
}

