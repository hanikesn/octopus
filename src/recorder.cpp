#include <QMessageBox>

#include "recorder.h"
#include "timemanager.h"

Recorder::Recorder(TimeManager* timeManager, QObject *parent) :
    QObject(parent),
    timeManager(timeManager),
    recording(false),
    recordStart(0),
    recordEnd(0)
{

}

int Recorder::showRecordDialog()
{
    QMessageBox msg;
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Ok);
    msg.setIcon(QMessageBox::Information);
    msg.setButtonText(QMessageBox::Save, tr("Save"));
    msg.setButtonText(QMessageBox::Discard, tr("Discard"));
    msg.setButtonText(QMessageBox::Ok, tr("Continue Recording"));
    msg.setDefaultButton(QMessageBox::Save);
    msg.setText(tr("Do you wish to save the currently recorded data?"));
    int result = msg.exec();
    return result;
}

bool Recorder::isRecording()
{
    return recording;
}

bool Recorder::toggleRecording()
{
    if (recording) {
        recordEnd = timeManager->getMaximum();
        emit record(recordStart, recordEnd, false);
        // end recording, show record dialog
        int result = showRecordDialog();
        if (result == QMessageBox::Save) {
            emit saveProject(recordStart, recordEnd);
            recording = false;
            recordStart = -1;
            recordEnd = -1;
            emit record(recordStart, recordEnd, recording);
        } else if (result == QMessageBox::Discard) { // discard recording
            recording = false;
            recordStart = -1;
            recordEnd = -1;
            emit record(recordStart, recordEnd, recording);
        } else if (result == QMessageBox::Ok) { // go on with the recording
            recording = true;
            recordEnd = -1;
            emit record(recordStart, recordEnd, recording);
        }
    } else { // start recording
        recordStart = timeManager->getMaximum();
        recording = true;
        emit record(recordStart, -1, recording);
    }    
    return recording;
}
