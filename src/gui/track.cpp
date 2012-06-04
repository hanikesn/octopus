#include "gui/track.h"

#include "abstractdataseries.h"
#include "gui/discretegraph.h"
#include "gui/interpolatinggraph.h"

#include <cmath>

#include <QDebug>

const QString Track::ICON_AS_BUTTON = QString(
            "border: 0;"
            "margin: 0px;");

Track::Track(const DataProvider *dataProvider, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider)
{
    ui.setupUi(this);

    setupButtons();
    setupPlot();

    // TODO(Steffi): Remove (dummy func)
    addData();

//    connect(dataProvider, SIGNAL(newMax(qint64)), this, SLOT(onNewMax(qint64)));
}

Track::Track(const DataProvider *dataProvider, const QString &fullDataSeriesName, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider)
{
    ui.setupUi(this);

    setupButtons();
    setupPlot();

    addSource(fullDataSeriesName);
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
    setPlotRange(0, 30);
}

void Track::setPlotRange(qint64 begin, qint64 end)
{
    ui.plot->xAxis->setRange(begin, end);
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

void Track::addSource(const QString &fullDataSeriesName)
{
    AbstractDataSeries *series = dataProvider->getDataSeries(fullDataSeriesName);

    if (series) {
        // visit the data series to determine its type and add the according graph
        series->accept(this);

        // TODO(Steffi)
    }
}

void Track::addGraph(const DoubleSeries &s) {
    qDebug() << "Visiting a double series! :)";

    // TODO(Steffi)

    graphs.append(InterpolatingGraph(s));

    // setup correct graph
    // connect newData signal
}

void Track::addGraph(const StringSeries &s) {
    qDebug() << "Visiting a string series! :)";

    // TODO(Steffi)

    graphs.append(DiscreteGraph(s));

    // setup correct graph
    // connect newData signal
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

    // save dialog result in string list
    QList<QString> selected;
    foreach (QString s, selected) {
        addSource(s);
    }
}

void Track::onPlotSettings()
{
    qDebug() << "Pretending to show plot settings dialog.";
    // TODO(Steffi)
}

void Track::onNewMax(qint64 timestamp)
{
    // TODO(Steffi) qcp anpassen
}
