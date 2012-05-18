#include "exampletrack.h"
#include <QLabel>
#include <QSizePolicy>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>

const QString ExampleTrack::NORMAL_STYLE = QString(
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #123456, stop: 1 #ffffff);"
            "border 10px solid #ff0000");


ExampleTrack::ExampleTrack(QWidget *parent) :
    QWidget(parent)
{
    //    this->setStyleSheet(NORMAL_STYLE);
    setMaximumHeight(70);
    QHBoxLayout *layout = new QHBoxLayout(this);

    customPlot = new QLabel();
    customPlot->setStyleSheet(NORMAL_STYLE);
    customPlot->setMinimumSize(2000, 70);
    customPlot->setMaximumSize(3000, 70);
    actionArea = new QLabel();
    actionArea->setMaximumSize(60, 70);
    actionArea->setMinimumSize(60, 70);
    actionArea->setStyleSheet("background-color: #bcbcbc;"
                              "border-right: 1px solid #ff0000;");


    layout->addWidget(actionArea);
    layout->addWidget(customPlot);

}

void ExampleTrack::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //    qDebug() << "paintEvent";

}


void ExampleTrack::resizeEvent(QResizeEvent *re)
{
    //    qDebug() << "Resisze Event";

}

void ExampleTrack::adjustVisibleRange(int position, int rangeMin, int rangeMax)
{
    qDebug() << "pos: " << position << "   rangeMin:" << rangeMin << "   rangeMax: " << rangeMax;

    // TODO: Range auf QCustomPlot entsprechend setzen.
}
