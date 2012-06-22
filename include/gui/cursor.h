#ifndef CURSOR_H
#define CURSOR_H

#include <QWidget>
#include <QPen>


class TimeManager;
class PresentationItem;

class Cursor : public QWidget
{
    Q_OBJECT

public:
    explicit Cursor(TimeManager *timeManager, QWidget *parent);

    qint64 getTime();

public slots:
    void setTime(qint64 time);

    void updateHeight(int height);

    void onUpdate();

protected:
    void paintEvent(QPaintEvent *);

private:    
    QPen pen;    
    QBrush brush;

    qint64 currentTime;

    TimeManager *timeMgr;
};

#endif // CURSOR_H
