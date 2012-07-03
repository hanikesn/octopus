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
    TimeManager(Clock::time_point startTime, QObject* parent);

    qint64 getLowVisRange() {return lowVisRange;}
    qint64 getHighVisRange() {return highVisRange;}
    qint64 getMaximum() {return maximum;}

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
      * Loads the necessary information(low-/highVisRange and cursor time) from the QVariantMap
      * @param qvm The map in which the information needs to be stored.
      */
    void load(QVariantMap *qvm);

    /**
      * Stores the necessary information(low-/highVisRange and cursor time) to the QVariantMap
      * @param qvm The map in which the information is stored.
      */
    void save(QVariantMap *qvm);

    /**
      * Centers the visible range around 'timestamp' and propagates the changed range.
      * @param timestamp The new center of the visible range.
      */
    void center(qint64 timestamp);

    bool isPlaying() {return playing;}

    int getMarginLeft() {return marginLeft;}
    int getMarginRight() {return marginRight;}

    bool isValidPos(int pos) {return pos > marginLeft && pos < width - marginRight;}
    int clipPos(int pos) {return qMin(qMax(pos, marginLeft), width - marginRight);}

    /**
      * Determines how much time should be between 2 large ticks in the timeline (in microseconds)
      * @return Time between to large ticks.
      */
    int getStepSize();

    bool hasUnsavedChanges() { return unsavedChanges; }
    void setUnsavedChanges(bool uc) { unsavedChanges = uc; }

private:
    /**
      * Returns the amount of time the range is changed during playing
      * (every 'timeoutIntervall' msecs)
      */
    qint64 getTimePerPx() {return timePerPx;}

signals:
    /**
      * This signal is emitted whenever the visible range changes.
      * @param begin Begin of the visible range
      * @param end End of the visible range
      */
    void rangeChanged(qint64 begin, qint64 end);

    /**
      * This signal is emitted when the point of time which is represented by the cursor changes.
      * @param time New time of the cursor
      */
    void currentTimeChanged(qint64 time);

    /**
      * This signal is emitted when new data is received and the datas timestamp is greater than
      * any previously received timestamp.
      * @param time New maximum timestamp
      */
    void newMax(qint64 time);

    /**
      * This signal is enabled whenever the follow-function (cursor is pinned to new data) is
      * de-/activated.
      * @param True in case following is enabled, false otherwise
      */
    void followEnabled(bool);

    /**
      * This signal is enabled whenever the play-function (cursor moves over recorded data) is
      * de-/activated.
      * @param True in case play is enabled, false otherwise
      */
    void playEnabled(bool);

public slots:
    /**
      * This slot is called when new data is received.
      * @param timestamp The timestamp of the new data.
      */
    void onNewMax(qint64 timestamp);

    /**
     * @brief onZoom
     * @param factor positive means zoom in
     * @param pos to zoom into, -1 means center
     */
    void zoom(int factor, qint64 pos = -1);

    /**
      * Sets the currentTime unless the follow-function is active. In case 'play' is active the
      * cursor will be moved to the corresponding position and the playing will be continued
      * from this position.
      */
    void setTime(qint64);

    /**
      * This slot is called when the user presses the play-button in the gui.
      * This function handles the states of 'playing' and 'following' and emits the
      * followEnabled/playEnabled signals.
      */
    void onPlay();

    void onMarginsChanged(int marginLeft, int marginRight);

    void onNewWidth(int width);

    /**
      * This slot is called when the user presses the follow-button("pin to new data") in the gui.
      * This function handles the states of 'playing' and 'following' according to the new state
      * the followEnabled(bool) signal is emitted.
      */
    void onFollow();

    void setRange(qint64 start, qint64 end);

private slots:
    /**
      * This slot is called when the timer emits a timeout. It sets the state of 'playing ' and
      * 'currentTime'.
      */
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

    bool playing; // True in case the play-back is active, false otherwise
    bool following; // Flag to determine whether "Pin to new data" is active or not

    QTimer* timer;

    int marginLeft, marginRight;
    int width;

    // stores whether changes in the visual range have happened.
    bool unsavedChanges;

    Clock::time_point absoluteStartTime;
    /// The point of time when the playing started
    Clock::time_point startTime;

    /**
      * In case the cursor is not visible this function adjusts the visible range slightly so that
      * the cursor is visible again.
      */
    void ensureCursorVisibility();
};

#endif // TIMEMANAGER_H
