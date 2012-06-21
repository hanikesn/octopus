#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <QObject>

#include "serializable.h"

class TimeLine;
class QScrollBar;
class QTimer;

class TimeManager : public QObject, public Serializable
{
    Q_OBJECT
public:
    TimeManager(QScrollBar *hScrollBar, TimeLine *timeLine);
    ~TimeManager();

    qint64 getLowVisRange() {return lowVisRange;}
    qint64 getHighVisRange() {return highVisRange;}
    qint64 getMaximum() {return maximum;}

    /**
      * Returns the amount of time the range is changed during playing
      * (every 'timeoutIntervall' msecs)
      */
    qint64 getTimePerPx() {return timePerPx;}

    qint64 getUpperEnd(qint64 lowerEnd);

    /**
      * Converts a position on the view into the corresponting point in time.
      * @param pos The x-position in the view to be converted.
      */
    qint64 convertPosToTime(int pos);

    /**
      * Converts a point in time to the according x-position on the screen.
      * @param time The point in time to be converted
      */
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

    /**
      * Calculates the amount of time between two points in the view.
      * @param pos1 First position
      * @param pos2 Second position
      * @return The result will always be positive (e.g. if pos1 > pos2 the values will be swapped)
      */
    qint64 difference(int pos1, int pos2);

    void center(qint64 timestamp);

    void changeTimeStep(int milliSeconds);

    enum Playstate {PLAYING, PAUSED};

    Playstate getPlaystate() {return playstate;}

signals:
    void rangeChanged(qint64 begin, qint64 end);

    void currentTimeChanged(qint64 time);

public slots:
    void onNewMax(qint64 timestamp);
    void onZoomIn();
    void onZoomOut();

    void setTime(qint64);


    void onPlay();
private slots:
    void onTimeout();

    void horizontalScroll(int pos);

    void onRangeChanged(qint64 begin, qint64 end);

private:
    // low and high limit of the visible range
    qint64 lowVisRange, highVisRange;

    // amount of time which one pixel represents
    qint64 timePerPx;

    // highest timestamp ever seen
    qint64 maximum;

    // this is the amount of time the range is changed during playing
    // (every timeoutIntervall msecs)
    qint64 timeoutUpdateIntervall;
    int timeoutIntervall;

    qint64 currentTime;

    Playstate playstate;

    bool autoScroll;

    QScrollBar *hScrollBar;

    QTimer* timer;

    TimeLine *timeLine;


    qint64 getZoomFactor(bool zoomOut);

};

#endif // TIMEMANAGER_H
