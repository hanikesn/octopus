#include "gui/selection.h"

#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>

#include "timemanager.h"

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
    // Determine left and right coordinates
    int left = timeManager->clipPos(timeManager->convertTimeToPos(begin));
    int right = timeManager->clipPos(timeManager->convertTimeToPos(end));

    if(left > right)
        std::swap(left, right);

    setFixedWidth(right-left);
    move(left, 0);
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

void Selection::onZoomIn()
{
    emit zoomIn(begin, end);
}

void Selection::updateHeight(int h)
{
    setFixedHeight(h);
}

void Selection::show()
{
    setVisible(true);
    onUpdate();
    emit selectionChanged(begin, end);
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
    emit selectionChanged(begin, end);
}

void Selection::setSelectionEnd(qint64 time)
{
    end = time;
    onUpdate();
    emit selectionChanged(begin, end);
}

void Selection::contextMenuEvent(QContextMenuEvent * event)
{ 
    menu->popup(event->globalPos());
}
