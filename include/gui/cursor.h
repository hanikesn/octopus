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
    explicit Cursor(int offsetLeft, TimeManager *timeManager,  PresentationItem *presentationItem,
                    QGraphicsItem *parent = 0);
    ~Cursor();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

    qint64 getTime();

public slots:
    void setTime(qint64 time);

    void onOffsetChanged(int offset);

    void onUpdateSize(QSize size);

    void update();

private:    
    void changePos(int pos);

    QPen pen;    
    QBrush brush;
    int offsetLeft;

    qint64 currentTime;

    TimeManager *timeMgr;
    PresentationItem *presentationItem;
};

#endif // CURSOR_H
