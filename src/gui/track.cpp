#include "track.h"

#include "abstractdataseries.h"

#include <cmath>

#include <QDebug>

const QString Track::ICON_AS_BUTTON = QString(
            "border: 0;"
            "margin: 0px;");

Track::Track(DataProvider *dataProvider, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider)
{
    ui.setupUi(this);

    setupButtons();
    setupPlot();

    addData();
}

void Track::setupButtons()
{
    ui.delButton->setStyleSheet(ICON_AS_BUTTON);
    connect(ui.delButton, SIGNAL(clicked()), this, SLOT(onDelete()));

    // TODO(Steffi): srcButton-Label korrigieren
    ui.srcButton->setStyleSheet(ICON_AS_BUTTON);
    connect(ui.srcButton, SIGNAL(clicked()), this, SLOT(onSources()));

    ui.setButton->setStyleSheet(ICON_AS_BUTTON);
    connect(ui.setButton, SIGNAL(clicked()), this, SLOT(onPlotSettings()));
}

void Track::setupPlot()
{
    ui.plot->xAxis->setRange(0, 30);
}

void Track::setPlotRange(qint64 begin, qint64 end)
{
    int rangeSize = ui.plot->xAxis->range().size();
    ui.plot->xAxis->setRange(begin, begin + rangeSize);
    ui.plot->replot();
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

void Track::onNewData(const AbstractDataSeries *series, qint64 timeStamp)
{
//    series->getData(timeStamp);

    // TODO(Steffi)
}

void Track::onDelete()
{
    qDebug() << "Pretending to delete track.";
    // TODO(Steffi)

    emit del(this);
}

void Track::onSources()
{
    qDebug() << "Pretending to show source dialog.";

    // TODO(Steffi):

    QList<QString> dataSeriesNames = dataProvider->getDataSeriesList();
    // parse list entries
    // show dialog

    QList<QString> selected;
    foreach (QString s, selected) {
        // get selected series from dataProvider
        // get data from data series
        // show data
    }
}

void Track::onPlotSettings()
{
    qDebug() << "Pretending to show plot settings dialog.";
    // TODO(Steffi)
}
