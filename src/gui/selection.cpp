#include "gui/selection.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>

Selection::Selection(QGraphicsScene *parent) :
    QGraphicsItem(0, parent),
    height(0),
    width(0),
    pen(Qt::lightGray),
    brush(Qt::lightGray)
{
    menu = new QMenu(0);
    exportAction = new QAction(tr("Export Range"), this);
    menu->addAction(exportAction);
    connect(exportAction, SIGNAL(triggered()), this, SIGNAL(exportTriggered()));
}

Selection::~Selection()
{
    delete menu;
}

QRectF Selection::boundingRect() const
{
    return  QRectF(0, 0, width, height);
}

void Selection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QRectF frame(0, 0, width, height);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setOpacity(0.5);
    painter->drawRoundedRect(frame, 5, 5);
}

void Selection::setHeight(int h)
{
    height = h;
}

void Selection::setWidth(int w)
{
    width = w;
}

void Selection::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu->popup(event->screenPos());
}
