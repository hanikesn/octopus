#ifndef TRACKHOLDER_H
#define TRACKHOLDER_H

#include <QWidget>

class TimeManager;
class Selection;
class Cursor;

class TrackHolder : public QWidget
{
public:
    TrackHolder(TimeManager& timeManager, Cursor& cursor, Selection& selection, QWidget* parent);
protected:
    /**
      * If shift-button is pressed, a selection is started. Otherwise nothing happens.
      * @param event The mousePressEvent to be processed.
      */
    void mousePressEvent(QMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' it ends at the current position of
      * the event (x-position). The corresponding timestamps are calculated and the
      * 'selection()'-signal is emitted.
      * If no new selection has been started this shows the cursor at the events position.
      * @param event The mouseReleaseEvent to be processed.
      */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
      * If a selection has been started at 'mousePressEvent()' and the shift-button is pressed
      * the selected area is increased in size to the events position (x-position).
      * Otherwise, nothing happens.
      * @param event The mouseMoveEvent to be processed.
      */
    void mouseMoveEvent(QMouseEvent *event);

    /**
      * As we have no interaction for the double click, this function does nothing.
      * @param event The mouseDoubleClickEvent to be processed.
      */
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    bool createSelection;
    TimeManager& timeManager;
    Selection& selection;
    Cursor& cursor;

    // TODO delete
    int offsetLeft;
};

#endif // TRACKHOLDER_H
