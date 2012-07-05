/*
Saves start- and endtimestamps from the record and emits corresponding signals (to MainWindow and
PresentationArea).
*/
#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>

class TimeManager;

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(TimeManager* timeManager, QObject *parent);

    bool isRecording();
    
signals:

    /**
      * This signal is emitted when the user chooses to save the current recording.
      * @param start The start timestamp of the recording.
      * @param end The end timestamp of the recording.
      */
    void saveProject(qint64 start, qint64 end);

    /**
      * This signal is emitted whenever the state of the recording changes
      * (saved, discarded, paused, started).
      * If 'end' is -1 and recording is true the recording is still active/just started.
      * If both timestamps are -1 the recording ended.
      * @param start Start timestamp of the record
      * @param end End timestamp of the record or -1 if the record hasn't ended so far.
      * @param recording State of the recorder (recording/not recording)
      */
    void record(qint64 start, qint64 end, bool recording);
    
public slots:
    /**
     * @brief toggleRecording
     * @return true if recording is enabled
     */
    bool toggleRecording();

private:

    /**
      * Shows a dialog which offers the user to save/discard/continue the recording.
      * @return The result value of the dialogs execution.
      */
    int showRecordDialog();

    TimeManager* timeManager;

    bool recording;
    qint64 recordStart, recordEnd;
};

#endif // RECORDER_H
