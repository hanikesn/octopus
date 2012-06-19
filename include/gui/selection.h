#ifndef SELECTION_H
#define SELECTION_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPen>

class QMenu;
class QAction;
class PresentationItem;

class Selection : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Selection(PresentationItem *parent = 0);
    ~Selection();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void setHeight(int h);
    void setWidth(int w);

    int getHeight() { return height; }
    int getWidth() { return width; }

    void hide();

public slots:
    void onUpdate(QSize size);

signals:
    void exportTriggered();

private:

    int height, width;
    QPen pen;
    QBrush brush;
    PresentationItem *presentationItem;

    QMenu *menu;
    QAction *exportAction;

};

#endif // SELECTION_H
