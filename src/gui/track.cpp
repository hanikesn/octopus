#include "track.h"

#include <cmath>

#include <QDebug>

Track::Track(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    ui.plot->xAxis->setRange(0, 30);

    addData();
}

void Track::setPlotRange(int position, int rangeMin, int rangeMax)
{
    int rangeSize = ui.plot->xAxis->range().size();
    ui.plot->xAxis->setRange(position, position + rangeSize);
    ui.plot->replot();

    // TODO(Steffi): remove
    qDebug() << "pos: " << position << "   rangeMin:" << rangeMin << "   rangeMax: " << rangeMax;
}

void Track::addData()
{
    ///////////////////////////////////////////////////////////////
    /* copied from plot-examples: MainWindow::setupLineStyleDemo */
    QPen pen;

    QStringList lineNames;
    lineNames << "lsNone" << "lsLine" << "lsStepLeft" << "lsStepRight"
                 << "lsStepCenter" << "lsImpulse";

    // add graphs with different line styles:
    for (int i=QCPGraph::lsNone; i<=QCPGraph::lsImpulse; ++i)
    {
      ui.plot->addGraph();
      pen.setColor(QColor(sin(i*1+1.2)*80+80, sin(i*0.3+0)*80+80, sin(i*0.3+1.5)*80+80));
      ui.plot->graph()->setPen(pen);
      ui.plot->graph()->setName(lineNames.at(i-QCPGraph::lsNone));
      ui.plot->graph()->setLineStyle((QCPGraph::LineStyle)i);
      ui.plot->graph()->setScatterStyle(QCPGraph::ssCircle);
      ui.plot->graph()->setScatterSize(5);

      // generate data:
      QVector<double> x(500), y(500);
      for (int j=0; j<500; ++j)
      {
        x[j] = j/10.0 * 5*3.14 + 0.01;
        y[j] = 7*sin(x[j])/x[j] - (i-QCPGraph::lsNone)*5 + (QCPGraph::lsImpulse)*5 + 2;
      }
      ui.plot->graph()->setData(x, y);
      ui.plot->graph()->rescaleValueAxis(true);
    }
    ///////////////////////////////////////////////////////////////
}
