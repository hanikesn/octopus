#include "exampletrack.h"

#include <QSizePolicy>
#include <QDebug>
#include <QHBoxLayout>

const QString ExampleTrack::NORMAL_STYLE = QString(
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #123456, stop: 1 #ffffff);"
            "border 10px solid #ff0000");



ExampleTrack::ExampleTrack(QWidget *parent) :
    QWidget(parent)
{    
    setMaximumHeight(70);
    QHBoxLayout *layout = new QHBoxLayout(this);

//    customPlot();
    customPlot.setStyleSheet(NORMAL_STYLE);
    customPlot.setMinimumSize(2000, 70);
    customPlot.setMaximumSize(3000, 70);
//    actionArea();
    actionArea.setMaximumSize(60, 70);
    actionArea.setMinimumSize(60, 70);
    actionArea.setStyleSheet("background-color: #bcbcbc;"
                              "border-right: 1px solid #ff0000;");

    layout->addWidget(&actionArea);
    layout->addWidget(&customPlot);    
}

void ExampleTrack::paintEvent(QPaintEvent *qpe)
{
    opt.init(this);    
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ExampleTrack::adjustVisibleRange(int position, int rangeMin, int rangeMax)
{
    qDebug() << "pos: " << position << "   rangeMin:" << rangeMin << "   rangeMax: " << rangeMax;

    // TODO: Range auf QCustomPlot entsprechend setzen.
}
