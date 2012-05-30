#ifndef SELECTION_H
#define SELECTION_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPen>

class Selection : public QGraphicsItem
{

public:
    explicit Selection(QGraphicsScene *parent = 0);
    ~Selection();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHeight(int h);
    void setWidth(int w);

    int getHeight() { return height; }
    int getWidth() { return width; }

private:

    QBrush brush;
    QPen pen;
    int height, width;

};

#endif // SELECTION_H
