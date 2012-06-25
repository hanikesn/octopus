#include "gui/track.h"

#include "abstractdataseries.h"
#include "plotsettings.h"
#include "gui/discretegraph.h"
#include "gui/interpolatinggraph.h"
#include "gui/plotsettingsdialog.h"
#include "gui/sourcedialog.h"

#include "dataprovider.h"
#include "common.h"
#include "measure.h"

#include <cmath>

#include <QDebug>

const QString Track::ICON_AS_BUTTON = QString(
            "border: 0;"
            "margin: 0px;");

Track::Track(const DataProvider &dataProvider, QWidget *parent) :
    QWidget(parent),
    optPlotMarginLeft(0),
    dataProvider(dataProvider)
{
    init();
}

Track::Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent) :
    QWidget(parent),
    optPlotMarginLeft(0),
    dataProvider(dataProvider)
{
    init();

    addSource(fullDataSeriesName);
}

Track::Track(const DataProvider &dataProvider, const QStringList &fullDataSeriesNames, QWidget *parent) :
    QWidget(parent),
    optPlotMarginLeft(0),
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

    ui.plot->setAttribute(Qt::WA_TransparentForMouseEvents);

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
    ui.plot->setAutoMargin(false);
    connect(ui.plot, SIGNAL(optMarginsRecalculated(int,int,int,int)), this, SLOT(onOptPlotMarginsRecalculated(int,int,int,int)));

    ui.plot->legend->setVisible(true);
    ui.plot->legend->setPositionStyle(QCPLegend::psTopLeft);
}

void Track::setPlotRange(qint64 begin, qint64 end)
{
    if (ui.plot->xAxis->range().lower != begin
            || ui.plot->xAxis->range().lower != end) {
        ui.plot->xAxis->setRange(begin, end);
		{
            MEASURE("plot");
            ui.plot->replot();
		}
    }
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
            Graph* graph = graphs.takeFirst();
            ui.plot->removeGraph(graph->getGraph());
            graph->deleteLater();
        }

        foreach (QString source, sources.first()) {
            addSource(source);
        }
        // only show the legend if the track is not empty
        ui.plot->legend->setVisible(!sources.first().isEmpty());

        ui.plot->rescaleValueAxes();
        ui.plot->replot();
    }
}

void Track::onPlotSettings()
{
    // TODO(Steffi)

    QList<AbstractDataSeries*> dataSeries;
    foreach (QString name, getFullDataSeriesNames()) {
        AbstractDataSeries *series = dataProvider.getDataSeries(name);
        if (series) {
            dataSeries.append(series);
        }
    }

    PlotSettings settings = PlotSettingsDialog::getSettings(dataSeries, true, false, false);

    if (!settings.isEmpty()) {
        //    if (settings.scalingMode == PlotSettings::MINMAXSCALING) {
        //        ui.plot->yAxis->setScaleType(QCPAxis::stLinear);
        //        ui.plot->yAxis->setRange(0, 1);
        //    } else if (settings.scalingMode == PlotSettings::NOSCALING) {
        //        if (settings.scaleType == PlotSettings::LOGSCALE) {
        //            ui.plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        //        } else {
        //            ui.plot->yAxis->setScaleType(QCPAxis::stLinear);
        //        }
        //    }

        foreach (Graph *g, graphs) {
            g->update(settings);
        }
    }
}

void Track::onOptPlotMarginsRecalculated(int left, int right, int top, int bottom)
{
    ui.plot->setMarginRight(0);
    ui.plot->setMarginTop(top);
    ui.plot->setMarginBottom(bottom);

    if (optPlotMarginLeft != left) {
        optPlotMarginLeft = left;
        emit optPlotMarginsChanged();
    }
}

int Track::getPlotOffset()
{
    return ui.plot->pos().x();
}

void Track::setPlotMarginLeft(int margin)
{
    if (margin != ui.plot->marginLeft()) {
        ui.plot->setMarginLeft(margin);
    }
    ui.plot->replot();
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

