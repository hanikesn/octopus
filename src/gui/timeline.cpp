#include "timeline.h"

#include <QDebug>

TimeLine::TimeLine(QWidget *parent):
    QLabel(parent)
{
    this->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #FFA000, stop: 1 #FF4500);"
                        "border: 1px solid #666666;"
                        "margin: 0;"
                        "padding: 0;");
}

void TimeLine::adjustVisibleRange(qint64 begin, qint64 end)
{

}

