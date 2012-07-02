#include "gui/timescrollbar.h"

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

    int val = max / timePerTick() - pageStep();
    setMaximum(qMax(0, val));

    blockSignals(true);
    setValue(begin/timePerTick());
    blockSignals(false);
}

void TimeScrollbar::onNewMax(qint64 max)
{
    this->max = max;
    int val = max / timePerTick() - pageStep();
    setMaximum(qMax(0, val));
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

