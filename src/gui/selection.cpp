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
    visible(true),
    height(100),
    width(100),
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
    qint64 begin_ = begin;
    qint64 end_ = end;

    if(begin_ > end_)
        std::swap(begin_, end_);

    int left = timeManager->convertTimeToPos(begin_);
    int right = timeManager->convertTimeToPos(end_);

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
    QRectF frame(0, 0, width, height);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setOpacity(0.5);
    painter.drawRoundedRect(frame, 5, 5);
}

void Selection::setHeight(int h)
{
    height = h;
    update();
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

    visible = false;

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
