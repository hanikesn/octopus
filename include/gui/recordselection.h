#ifndef RECORDSELECTION_H
#define RECORDSELECTION_H

#include <QPen>
#include <QWidget>

class TimeManager;

class RecordSelection : public QWidget
{
    Q_OBJECT
public:
    explicit RecordSelection(TimeManager *timeManager, QWidget *parent);

public slots:
    /**
      * Sets the start(timestamp) and resizes the selection accordingly.
      * @param begin The new begin of the selection (timestamp)
      */
    void setSelectionBegin(qint64 begin);

    /**
      * Sets the end(timestamp) only when there is an active recording and resizes the selection
      * accordingly.
      * @param end The new end of the selection (timestamp)
      */
    void setSelectionEnd(qint64 end);

    /**
      * Sets a fixed height 'h' for this selection.
      */
    void updateHeight(int h);

    /**
      * Moves and resizes the selection.
      */
    void onUpdate();

    /**
      * Determines in which state the record currently is. In case both timestamps are -1 and
      * recording is false this object will be set invisible.
      * @param start Start timestamp of the record
      * @param end End timestamp of the record or -1 if the record hasn't ended so far.
      * @param recording State of the recorder (recording/not recording)
      */
    void onRecord(qint64 start, qint64 end, bool recording);

protected:
    /**
      * Override of the QWidget function. (Custom drawing)
      */
    void paintEvent(QPaintEvent *);

private:
    // Current begin of the selection
    qint64 begin;
    // Current begin of the selection
    qint64 end;

    // A flag to determine wheter there is an active recording
    bool recording;

    // Border color of the selection
    QPen pen;
    // Background color of the selection
    QBrush brush;

    // Converts x-coordinates to timestamps
    TimeManager* timeManager;
};

#endif //RECORDSELECTION_H
