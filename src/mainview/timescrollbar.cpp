#include "mainview/timescrollbar.h"

TimeScrollbar::TimeScrollbar(QWidget *parent) :
    QScrollBar(parent),
    begin(0),
    end(100),
    max(0)
{
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onScroll(int)));
}

qint64 TimeScrollbar::timePerTick()
{
    return (end - begin) / width();
}

void TimeScrollbar::onRangeChanged(qint64 begin, qint64 end)
{
    this->begin = begin;
    this->end = end;

    blockSignals(true);
    setMaximum(maximumValue());
    setValue(begin/timePerTick());
    blockSignals(false);
}

void TimeScrollbar::onNewMax(qint64 max)
{    
    this->max = max;
    setMaximum(maximumValue());
}

void TimeScrollbar::onScroll(int val)
{
    qint64 range = end - begin;
    begin = qMax(0LL, val * timePerTick());
    end = begin + range;
    emit rangeChanged(begin, end);
}

void TimeScrollbar::resizeEvent(QResizeEvent *)
{
    setPageStep(width());
}

int TimeScrollbar::maximumValue()
{
    qint64 tmp = qMax(end, max);
    int val = tmp / timePerTick() - pageStep();
    return qMax(0, val);
}

