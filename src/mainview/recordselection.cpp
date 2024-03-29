#include "mainview/recordselection.h"

#include "time/timemanager.h"

#include <QPainter>

#include <QDebug>

RecordSelection::RecordSelection(TimeManager* timeManager, QWidget *parent):
    QWidget(parent),
    begin(0),
    end(0),
    recording(false),
    pen(Qt::red),
    brush(Qt::red),
    timeManager(timeManager)
{
    setObjectName("RecordSelection");

    setVisible(false);
}

void RecordSelection::onUpdate()
{
    int left = timeManager->clipPos(timeManager->convertTimeToPos(begin));
    int right = timeManager->clipPos(timeManager->convertTimeToPos(end));

    if(left > right)
        std::swap(left, right);

    setFixedWidth(right-left);
    move(left, 0);
}

void RecordSelection::onRecord(qint64 start, qint64 end, bool recording)
{
    if (recording && (end == -1)) { // start record
        setVisible(true);
        setSelectionBegin(start);
    } else if (!recording && (start != -1) && (end != -1)) { // recording paused
        setSelectionEnd(end);
    } else if (!recording && (start == -1) && (end == -1)) { // recording ended
        setVisible(false);
    }
    this->recording = recording;
}

void RecordSelection::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setOpacity(0.5);
    painter.drawRoundedRect(QRect(0,0, geometry().width(), geometry().height()), 5, 5);
}

void RecordSelection::updateHeight(int h)
{
    setFixedHeight(h);
}

void RecordSelection::setSelectionBegin(qint64 time)
{
    begin = time;
    end = time;
    onUpdate();
}

void RecordSelection::setSelectionEnd(qint64 time)
{    
    if (!recording) return; // When there is no recording, the recorded range can't change!
    end = time;
    onUpdate();
}
