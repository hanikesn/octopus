#include "gui/cursor.h"

#include <QPainter>
#include <QGraphicsScene>

#include "timemanager.h"
#include "gui/presentationitem.h"

#include <QDebug>

Cursor::Cursor(TimeManager *timeManager, QWidget *parent) :
    QWidget(parent),
    pen(Qt::red),
    brush(Qt::red),
    offsetLeft(0),
    currentTime(0),
    timeMgr(timeManager)

{
    setObjectName("Cursor");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedWidth(1);
    update();
}

void Cursor::paintEvent(QPaintEvent *)
{    
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(QRect(0,0, geometry().width(), geometry().height()));
}

void Cursor::onOffsetChanged(int offset)
{
    offsetLeft = offset;
    update();
}

void Cursor::setTime(qint64 time)
{
    currentTime = time;
    update();
}

void Cursor::update()
{
    int newPos = timeMgr->convertTimeToPos(currentTime);
    if(newPos == -1)
        setVisible(false);
    else
        setVisible(true);

    move(newPos + offsetLeft, 0);
}

qint64 Cursor::getTime()
{
    return currentTime;
}

void Cursor::updateHeight(int height)
{
    setFixedHeight(height);
}
