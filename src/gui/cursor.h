#ifndef CURSOR_H
#define CURSOR_H

#include <QGraphicsWidget>

#include <QPen>

class Cursor : public QGraphicsWidget
{

public:
    explicit Cursor(QGraphicsScene *parent = 0);
    ~Cursor();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect();

    void setMinHeight(int mh);

    int getMinHeight(){return minHeight;}

private:
    int minHeight;
    QBrush brush;
    QPen pen;


};

#endif // CURSOR_H
