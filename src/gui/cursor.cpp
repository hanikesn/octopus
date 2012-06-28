#include "gui/cursor.h"

#include <QPainter>
#include "timemanager.h"


Cursor::Cursor(QColor color, TimeManager *timeManager, QWidget *parent) :
    QWidget(parent),
    pen(color),
    brush(color),
    currentTime(0),
    timeMgr(timeManager)

{
    setObjectName("Cursor");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedWidth(1);
    onUpdate();
}

void Cursor::paintEvent(QPaintEvent *)
{    
    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(QRect(0,0, geometry().width(), geometry().height()));
}

void Cursor::setTime(qint64 time)
{
    currentTime = time;
    onUpdate();
}

void Cursor::onUpdate()
{
    int newPos = timeMgr->convertTimeToPos(currentTime);
    setVisible(timeMgr->isValidPos(newPos));

    move(newPos, 0);
}

qint64 Cursor::getTime()
{
    return currentTime;
}

void Cursor::updateHeight(int height)
{
    setFixedHeight(height);
}
