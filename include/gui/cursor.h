#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsWidget>
#include <QPen>


class TimeManager;
class PresentationItem;

class Cursor : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit Cursor(TimeManager *timeManager, QGraphicsItem *parent);
    ~Cursor();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

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
