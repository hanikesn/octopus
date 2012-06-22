#include "gui/selection.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>

#include "timemanager.h"

#include "gui/presentationitem.h"

#include <QDebug>

Selection::Selection(TimeManager* timeManager, QWidget *parent):
    QWidget(parent),
    begin(0),
    end(0),
    pen(Qt::lightGray),
    brush(Qt::lightGray),
    timeManager(timeManager)
{
    setObjectName("Selection");

    menu = new QMenu();
    exportAction = new QAction(tr("Export Range"), this);
    menu->addAction(exportAction);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportTriggered()));

    setObjectName("Selection");

    hide();
}

void Selection::update()
{
    int left = timeManager->convertTimeToPos(begin);
    int right = timeManager->convertTimeToPos(end);

    if(left > right)
        std::swap(left, right);

    setFixedWidth(right-left);
    move(left, 0);
}


Selection::~Selection()
{
    menu->deleteLater();
}

void Selection::exportTriggered()
{
    emit onExport(begin, end);
}

void Selection::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setOpacity(0.5);
    painter.drawRoundedRect(QRect(0,0, geometry().width(), geometry().height()), 5, 5);
}

void Selection::updateHeight(int h)
{
    setFixedHeight(h);
}

void Selection::show()
{
    setVisible(true);
    update();
    emit selectionChanged(begin, end);
}

void Selection::hide()
{
    if (!isVisible()) return;

    setVisible(false);

    update();

    emit selectionChanged(-1, -1);
}

void Selection::setSelectionBegin(qint64 time)
{
    begin = time;
    end = time;
    update();
    emit selectionChanged(begin, end);
}

void Selection::setSelectionEnd(qint64 time)
{
    end = time;
    update();
    emit selectionChanged(begin, end);
}

/*void Selection::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{ 
    menu->popup(event->screenPos());
}*/
