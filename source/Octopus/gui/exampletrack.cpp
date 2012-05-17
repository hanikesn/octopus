#include "exampletrack.h"
#include <QLabel>
#include <QSizePolicy>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

const QString ExampleTrack::NORMAL_STYLE = QString(
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #000000, stop: 1 #ffffff);");


ExampleTrack::ExampleTrack(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet(NORMAL_STYLE);
    setMinimumSize(2000, 70);
    setMaximumSize(3000, 70);

    actionArea = new QLabel(this);
    actionArea->setMaximumSize(60, 70);
    actionArea->setMinimumSize(60, 70);
    actionArea->setStyleSheet("background-color: #bcbcbc;"
                              "border: 0px solid;"
                              "border-right: 1px solid #5d5d5d;"
                              "margin-bottom: 0px;");




}

void ExampleTrack::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void ExampleTrack::resizeEvent(QResizeEvent *re)
{
//    qDebug() << "Resisze Event";

}
