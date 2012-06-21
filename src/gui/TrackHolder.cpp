#include "gui/TrackHolder.h"

#include <QMouseEvent>
#include <QVBoxLayout>

#include "timemanager.h"
#include "gui/selection.h"
#include "gui/cursor.h"

TrackHolder::TrackHolder(TimeManager& timeManager, Cursor &cursor, Selection& selection, QWidget *parent)
    : QWidget(parent),
      timeManager(timeManager),
      selection(selection),
      cursor(cursor),
      offsetLeft(0)
{
    setObjectName("TrackHolder");
    setLayout(new QVBoxLayout());
    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

void TrackHolder::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) &&
            (event->modifiers() == Qt::ShiftModifier) &&
            (event->pos().x() >= offsetLeft) &&
            (timeManager.getPlaystate() != TimeManager::PLAYING)) {
        createSelection = true;
        selection.show();
        selection.setSelectionBegin(timeManager.convertPosToTime(event->pos().x()));
    }
}

void TrackHolder::mouseReleaseEvent(QMouseEvent *event)
{
    if(createSelection){
        createSelection = false;
        int selectionEnd = event->pos().x() < offsetLeft ? offsetLeft : event->pos().x();

        selection.setSelectionEnd(timeManager.convertPosToTime(selectionEnd));
        cursor.setVisible(false);
    } else if (event->pos().x() >= offsetLeft) {
        qint64 currentTime = timeManager.convertPosToTime(event->pos().x()- offsetLeft);
        timeManager.setTime(currentTime);
        selection.hide();
    }
}

void TrackHolder::mouseMoveEvent(QMouseEvent *event)
{
    if((event->modifiers() != Qt::ShiftModifier)){
        createSelection = false;
    } else if(event->pos().x() >= offsetLeft) {
        selection.setSelectionEnd(timeManager.convertPosToTime(event->pos().x()));
    }
}

void TrackHolder::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // Nothing to do...
}


