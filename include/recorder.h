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
    void saveProject(qint64 start, qint64 end);

    void record(qint64 start, qint64 end, bool recording);
    
public slots:
    /**
     * @brief toggleRecording
     * @return true if recording is enabled
     */
    bool toggleRecording();

private:
    int showRecordDialog();

    TimeManager* timeManager;

    bool recording;
    qint64 recordStart, recordEnd;
};

#endif // RECORDER_H
