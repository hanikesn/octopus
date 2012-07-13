#ifndef TIMESCROLLBAR_H
#define TIMESCROLLBAR_H

#include <QScrollBar>

class TimeManager;

class TimeScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit TimeScrollBar(QWidget *parent = 0);

signals:
    void rangeChanged(qint64 begin, qint64 end);

public slots:
    /**
     * Needed so that the current position can be updated
     */
    void onRangeChanged(qint64 begin, qint64 end);

    /**
     * Needed so that the maximum can be updated
     */
    void onNewMax(qint64 max);

private slots:
    void onScroll(int);

private:
    qint64 timePerTick();
    void resizeEvent(QResizeEvent *);

    qint64 begin;
    qint64 end;
    qint64 max;

    int maximumValue();
};

#endif // TIMESCROLLBAR_H
