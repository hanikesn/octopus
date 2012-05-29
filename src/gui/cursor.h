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

    void setCursorPos(int pos);

private:
    // current x-position of the cursor
    int currentPos;

    QBrush brush;
    QPen pen;

    static const int ACTIONAREAOFFSET;

};

#endif // CURSOR_H
