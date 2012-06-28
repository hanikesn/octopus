#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <QObject>
#include <boost/chrono.hpp>

#include "serializable.h"

class TimeLine;
class QScrollBar;
class QTimer;

class TimeManager : public QObject, public Serializable
{
    Q_OBJECT
public:
    typedef boost::chrono::high_resolution_clock Clock;

    /**
     * @brief TimeManager
     *
     * Set startTime to zero to indicate that we don't do a live recording
     */
    TimeManager(QScrollBar *hScrollBar, Clock::time_point startTime, QObject* parent);

    qint64 getLowVisRange() {return lowVisRange;}
    qint64 getHighVisRange() {return highVisRange;}
    qint64 getMaximum() {return maximum;}

    void movePx(int px);

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

    void load(QVariantMap *qvm);
    void save(QVariantMap *qvm);

    void center(qint64 timestamp);

    bool isPlaying() {return playing;}

    int getMarginLeft() {return marginLeft;}
    int getMarginRight() {return marginRight;}

    bool isValidPos(int pos) {return pos > marginLeft && pos < width - marginRight;}
    int clipPos(int pos) {return qMin(qMax(pos, marginLeft), width - marginRight);}

    // determines how much time should be between 2 big ticks in the timeline (in microseconds)
    int getStepSize();

    // TODO ugly, scrollbar sollte nicht im mainwindow sein, sondern der presentation area bekannt sein,
    // damit man sie hier die events nicht vorwarden muss.
    void forwardEventToScrollbar(QEvent *ev);

    bool hasUnsavedChanges() { return unsavedChanges; }
    void setUnsavedChanges(bool uc) { unsavedChanges = uc; }

private:
    /**
      * Returns the amount of time the range is changed during playing
      * (every 'timeoutIntervall' msecs)
      */
    qint64 getTimePerPx() {return timePerPx;}

signals:
    void rangeChanged(qint64 begin, qint64 end);

    void currentTimeChanged(qint64 time);

    void newMax(qint64 time);

public slots:
    void onNewMax(qint64 timestamp);

    /**
     * @brief onZoom
     * @param factor positive means zoom in
     * @param pos to zoom into, -1 means center
     */
    void zoom(int factor, qint64 pos = -1);

    void setTime(qint64);

    void onPlay();

    void onMarginsChanged(int marginLeft, int marginRight);

    void onNewWidth(int width);

    void onFollow(bool following);

    void setRange(qint64 start, qint64 end);

private slots:
    void onTimeout();

private:
    // low and high limit of the visible range
    qint64 lowVisRange, highVisRange;

    // amount of time which one pixel represents
    qint64 timePerPx;

    // highest timestamp ever seen
    qint64 maximum;

    qint64 currentTime;

    bool live;

    bool playing;
    bool following;

    QScrollBar *hScrollBar;

    QTimer* timer;

    int marginLeft, marginRight;
    int width;

    // stores whether changes in the visual range have happened.
    bool unsavedChanges;

    Clock::time_point absoluteStartTime;
    /// The point of time when the playing started
    Clock::time_point startTime;

    qint64 getZoomFactor(bool zoomOut);
    void updateScrollBar();
    void ensureCursorVisibility();
};

#endif // TIMEMANAGER_H
