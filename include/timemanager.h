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
    TimeManager(QScrollBar *hScrollBar, QObject* parent);

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

private:
    /**
      * Returns the amount of time the range is changed during playing
      * (every 'timeoutIntervall' msecs)
      */
    qint64 getTimePerPx() {return timePerPx;}

signals:
    void rangeChanged(qint64 begin, qint64 end);

    void currentTimeChanged(qint64 time);

public slots:
    void onNewMax(qint64 timestamp);

    /**
     * @brief onZoom
     * @param factor positive means zoom in
     */
    void onZoom(int factor);

    void setTime(qint64);

    void onPlay();

    void onMarginsChanged(int marginLeft, int marginRight);

    void onNewWidth(int width);

private slots:
    void onTimeout();

    void horizontalScroll(int pos);

private:
    // low and high limit of the visible range
    qint64 lowVisRange, highVisRange;

    // amount of time which one pixel represents
    qint64 timePerPx;

    // highest timestamp ever seen
    qint64 maximum;

    qint64 currentTime;

    bool playing;

    bool autoScroll;

    QScrollBar *hScrollBar;

    QTimer* timer;

    TimeLine *timeLine;

    int marginLeft, marginRight;
    int width;

    qint64 getZoomFactor(bool zoomOut);
    void updateScrollBar(bool scroll);
    void setRange(qint64 begin, qint64 end);
};

#endif // TIMEMANAGER_H
