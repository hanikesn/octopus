#include "mainview/selection.h"

#include "time/timemanager.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QPainter>

Selection::Selection(TimeManager* timeManager, QWidget *parent):
    QWidget(parent),
    begin(0),
    end(0),
    pen(Qt::lightGray),
    brush(Qt::lightGray),
    timeManager(timeManager)
{
    setObjectName("Selection");

    // Set up and connect menu items
    menu = new QMenu(this);
    QAction* exportAction = new QAction(tr("Export Range"), this);
    QAction* zoomInAction = new QAction(tr("Zoom In"), this);
    menu->addAction(exportAction);
    menu->addAction(zoomInAction);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportTriggered()));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(onZoomIn()));

    setVisible(false);
}

void Selection::onUpdate()
{
    left = begin;
    right = end;
    if (left > right)
        std::swap(left, right);
    // Determine left and right coordinates
    int l = timeManager->clipPos(timeManager->convertTimeToPos(left));
    int r = timeManager->clipPos(timeManager->convertTimeToPos(right));

    setFixedWidth(r-l+1);
    move(l, 0);
}

void Selection::exportTriggered()
{
    emit onExport(left, right);
}

void Selection::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setOpacity(0.5);
    painter.drawRoundedRect(QRect(0,0, geometry().width(), geometry().height()), 5, 5);
}

void Selection::onZoomIn()
{
    emit zoomIn(left, right);
}

void Selection::updateHeight(int h)
{
    setFixedHeight(h);
}

void Selection::show()
{
    setVisible(true);
    onUpdate();

    emit selectionChanged(left, right);
}

void Selection::hide()
{
    if (!isVisible()) return;

    setVisible(false);

    onUpdate();

    emit selectionChanged(-1, -1);
}

void Selection::setSelectionBegin(qint64 time)
{
    begin = time;
    end = time;

    onUpdate();

    emit selectionChanged(left, right);
}

void Selection::setSelectionEnd(qint64 time)
{
    end = time;

    onUpdate();

    emit selectionChanged(left, right);
}

void Selection::contextMenuEvent(QContextMenuEvent * event)
{ 
    menu->popup(event->globalPos());
}
