#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <QObject>
#include <QScrollBar>

#include "gui/timeline.h"
#include "serializable.h"

class TimeManager : public QObject, public Serializable
{
    Q_OBJECT
public:
    TimeManager(QScrollBar *hScrollBar, TimeLine *timeLine);
    ~TimeManager();

    qint64 getLowVisRange() {return lowVisRange;}
    qint64 getHighVisRange() {return highVisRange;}
//    qint64 getTimePerPx() {return timePerPx;}

    qint64 getUpperEnd(qint64 lowerEnd) {return timeLine->getUpperEnd(lowerEnd);}

    qint64 convertPosToTime(int pos);
    int convertTimeToPos(qint64 time);


    /**
      * Adds 'delta' to current high- and lowVisRange.
      * This function will do nothing if delta is <= 0.
      * @param delta The amount of time to be added.
      */
    void addRange(qint64 delta);

    /**
      * Recalculates the range from lowVisRange and emit a rangeChanged() signal.
      */
    void updateRange();

    void load(QVariantMap *qvm);
    void save(QVariantMap *qvm);

    qint64 difference(int pos1, int pos2);


signals:
    void rangeChanged(qint64 begin, qint64 end);

    void horizontalScroll();

public slots:
    void onRangeChanged(qint64 begin, qint64 end);

    void onNewMax(qint64 timestamp);


private slots:
    void horizontalScroll(int pos);

private:
    // low and high limit of the visible range
    qint64 lowVisRange, highVisRange;

    // amount of time which one pixel represents
    qint64 timePerPx;

    bool autoScroll;

    QScrollBar *hScrollBar;

    TimeLine *timeLine;

};

#endif // TIMEMANAGER_H
