#ifndef SELECTION_H
#define SELECTION_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPen>

class QMenu;
class QAction;

class Selection : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Selection(QGraphicsScene *parent = 0);
    ~Selection();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void setHeight(int h);
    void setWidth(int w);

    int getHeight() { return height; }
    int getWidth() { return width; }

signals:
    void exportTriggered();

private:

    QBrush brush;
    QPen pen;
    int height, width;

    QMenu *menu;
    QAction *exportAction;

};

#endif // SELECTION_H
