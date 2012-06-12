#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsWidget>

#include <QPen>

class Cursor : public QGraphicsWidget
{

public:
    explicit Cursor(int offset, QGraphicsScene *parent = 0);
    ~Cursor();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

private:    
    QPen pen;    
    QBrush brush;
    int offset;
};

#endif // CURSOR_H
