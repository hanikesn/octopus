#ifndef CURSOR_H
#define CURSOR_H

#include <QPen>
#include <QWidget>

class TimeManager;
class PresentationItem;

/*
A vertical line that moves and shows the current position in the tracks.
*/
class Cursor : public QWidget
{
    Q_OBJECT

public:
    explicit Cursor(QColor color, TimeManager *timeManager, QWidget *parent);

    /**
      * Current time of the cursor.
      * @return The current time of the cursor.
      */
    qint64 getTime();

public slots:
    /**
      * Sets the position of the cursor (as a timestamp) and repositions it.
      * @param time New time of the cursor
      */
    void setTime(qint64 time);

    /**
      * Sets a fixed height for this cursor.
      * @param height The new height for this cursor.
      */
    void updateHeight(int height);

    /**
      * This function needs to be called whenever the cursors position needs to be changed (for
      * example if the current visible range was modified).
      * Recalculates the cursors position according to 'currentTime' and moves it.
      */
    void onUpdate();

protected:
    /**
      * Override of the QWidget function.
      */
    void paintEvent(QPaintEvent *);

private:    
    // Border color
    QPen pen;    
    // Background color
    QBrush brush;

    // The current time which the cursors x-coordinate represents
    qint64 currentTime;

    // Converts x-coordinates to timestamps
    TimeManager *timeMgr;
};

#endif // CURSOR_H
