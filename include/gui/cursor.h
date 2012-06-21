#ifndef CURSOR_H
#define CURSOR_H

#include <QWidget>
#include <QWidget>
#include <QPen>


class TimeManager;
class PresentationItem;

class Cursor : public QWidget
{
    Q_OBJECT

public:
    explicit Cursor(TimeManager *timeManager, QWidget *parent);

    void paintEvent(QPaintEvent *);

    qint64 getTime();

public slots:
    void setTime(qint64 time);

    void onOffsetChanged(int offset);

    void updateCoverHeight(int height);
    void updateMaxHeight(int height);

    void update();

private:    
    QPen pen;    
    QBrush brush;
    int offsetLeft;

    qint64 currentTime;

    TimeManager *timeMgr;

    int coverHeight;
    int maxHeight;
};

#endif // CURSOR_H
