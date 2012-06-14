#include "gui/track.h"

#include "abstractdataseries.h"
#include "gui/discretegraph.h"
#include "gui/interpolatinggraph.h"
#include "gui/sourcedialog.h"

#include "common.h"

#include <cmath>

#include <QDebug>

const QString Track::ICON_AS_BUTTON = QString(
            "border: 0;"
            "margin: 0px;");

Track::Track(const DataProvider &dataProvider, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider),
    offset(52),
    lowRange(-1),
    highRange(-1)
{
    init();

    // TODO(Steffi): Remove (dummy func)
//    addData();
    addSource("Dummy.Interpolatable");
    addSource("Dummy.Discrete");
}

Track::Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider)
{
    init();

    addSource(fullDataSeriesName);
}

Track::Track(const DataProvider &dataProvider, const QStringList &fullDataSeriesNames, QWidget *parent) :
    QWidget(parent),
    dataProvider(dataProvider)
{
    init();

    foreach (QString name, fullDataSeriesNames) {
        addSource(name);
    }
}

void Track::init()
{
    ui.setupUi(this);

    setupButtons();
    setupPlot();
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

    ui.plot->legend->setVisible(true);
    ui.plot->legend->setPositionStyle(QCPLegend::psTopLeft);
}

void Track::setPlotRange(qint64 begin, qint64 end)
{
    if (lowRange != begin || highRange != end){
        lowRange = begin;
        highRange = end;
		{
			Measurement("setRange");
            ui.plot->xAxis->setRange(begin, end);
        }
        ui.plot->setNotAntialiasedElements(QCP::aeAll);
		{
			Measurement("plot");
			ui.plot->replot();
		}
    }
}

void Track::addData()
{
    QPen pen;

    // add interpolating graph
    ui.plot->addGraph();
    ui.plot->graph()->setName("Interpolating");
    ui.plot->graph()->setLineStyle(QCPGraph::lsLine);
    ui.plot->graph()->setScatterStyle(QCP::ssDisc);
    ui.plot->graph()->setScatterSize(4);
    pen.setColor(QColor(sin(1+1.2)*80+80, sin(1*0.3+0)*80+80, sin(1*0.3+1.5)*80+80));
    ui.plot->graph()->setPen(pen);

    // generate data:
    QVector<double> x(500), y(500);
    for (int j=0; j<500; ++j)
    {
      double d = j/15.0 * 5*3.14 + 0.01;
      x[j] = d*1000000;
      y[j] = 7*sin(d)/d + 2;
    }
    ui.plot->graph()->setData(x, y);

    // rescale yAxis
    ui.plot->graph()->rescaleValueAxis(true);

    // add discrete graph
    ui.plot->addGraph();
    ui.plot->graph()->setName("Discrete");
    ui.plot->graph()->setLineStyle(QCPGraph::lsImpulse);
    ui.plot->graph()->setScatterStyle(QCP::ssTriangleInverted);
    ui.plot->graph()->setScatterSize(4);
    pen.setColor(QColor(sin(2+1.2)*80+80, sin(2*0.3+0)*80+80, sin(2*0.3+1.5)*80+80));
    ui.plot->graph()->setPen(pen);

    // generate data:
    for (int j=0; j<500; ++j)
    {
      x[j] = (j/10.0 * 5*3.14 + 0.01)*1000000;
      y[j] = ui.plot->yAxis->range().size() * 0.4;
    }
    ui.plot->graph()->setData(x, y);

    ui.plot->legend->setVisible(true);
}

void Track::addSource(const QString &fullDataSeriesName)
{
    AbstractDataSeries *series = dataProvider.getDataSeries(fullDataSeriesName);

    if (series) {
        // visit the data series to determine its type and add the according graph
        series->accept(this);
    }
}

QStringList Track::getFullDataSeriesNames()
{
    QStringList list;
    foreach (Graph *g, graphs) {
        list.append(g->dataSeriesName());
    }
    return list;
}

void Track::addGraph(const DoubleSeries &s) {
    graphs.append(new InterpolatingGraph(ui.plot, s));
}

void Track::addGraph(const StringSeries &s) {
    graphs.append(new DiscreteGraph(ui.plot, s));
}

void Track::onDelete()
{
    emit del(this);
}

void Track::onSources()
{
    QList<QStringList> sources = SourceDialog::getSources(dataProvider,
                                                          "Select Data Series to be Shown in This Track",
                                                          false,
                                                          getFullDataSeriesNames());
    if (!sources.isEmpty()) {
        while (!graphs.isEmpty()) {
            graphs.takeFirst()->deleteLater();
        }
        foreach (QString source, sources.first()) {
            addSource(source);
        }
        ui.plot->replot();
    }
}

void Track::onPlotSettings()
{
    qDebug() << "Pretending to show plot settings dialog.";
    // TODO(Steffi)
}

void Track::setOffset(int pixel)
{
    offset = pixel;
}

void Track::save(QVariantMap *qvm)
{    
    QVariantList track;
    foreach(Graph *g, graphs){
        track << g->dataSeriesName();
    }
    qvm->insert("dataSeries", track);
}

void Track::load(QVariantMap *qvm)
{
    QVariantMap map = qvm->find("track").value().toMap();
    QVariantList seriesList = map.find("dataSeries").value().toList();
    foreach(QVariant series, seriesList){
        addSource(series.toString());
    }
}

