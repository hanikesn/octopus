#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsWidget>


class QGraphicsItem;

class TimeLine : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit TimeLine(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    void adjustVisibleRange(qint64 begin, qint64 end);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect();

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:

public slots:    

private:

};

#endif // TIMELINE_H
