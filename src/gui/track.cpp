#include "gui/track.h"

#include "abstractdataseries.h"
#include "doubleseries.h"
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
    dataProvider(dataProvider),
    currentScalingMode(PlotSettings::NOSCALING)
{
    init();
}

Track::Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent) :
    QWidget(parent),
    optPlotMarginLeft(0),
    dataProvider(dataProvider),
    currentScalingMode(PlotSettings::NOSCALING)
{
    init();

    addSource(fullDataSeriesName);
}

Track::Track(const DataProvider &dataProvider, const QStringList &fullDataSeriesNames, QWidget *parent) :
    QWidget(parent),
    optPlotMarginLeft(0),
    dataProvider(dataProvider),
    currentScalingMode(PlotSettings::NOSCALING)
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

PlotSettings Track::currentSettings()
{
    PlotSettings settings;

    settings.scalingMode = currentScalingMode;
    settings.plotScaleType = PlotSettings::toScaleType(ui.plot->yAxis->scaleType());

    foreach (Graph *g, graphs) {
        settings.setOffset(g->dataSeriesName(), dataProvider.getDataSeries(g->dataSeriesName())->offset);
        settings.setScaleType(g->dataSeriesName(), g->getScaleType());
    }

    return settings;
}

void Track::addGraph(const DoubleSeries &s) {
    PlotSettings::ScaleType desiredScaleType;
    switch (currentScalingMode) {
    case PlotSettings::NOSCALING:
        desiredScaleType = PlotSettings::toScaleType(ui.plot->yAxis->scaleType());
        break;
    case PlotSettings::MINMAXSCALING:
        desiredScaleType = s.defaultScaleType;
        break;
    default:
        // unknown scaling mode
        desiredScaleType = PlotSettings::NOT_SCALABLE;
        break;
    }

    graphs.append(new InterpolatingGraph(ui.plot, s, currentScalingMode, desiredScaleType));

    if (currentScalingMode == PlotSettings::NOSCALING) {
        ui.plot->rescaleValueAxes();
    }
    ui.plot->replot();
}

void Track::addGraph(const StringSeries &s) {
    graphs.append(new DiscreteGraph(ui.plot, s));
    ui.plot->replot();
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
        QStringList sourcesToAdd = sources.first();
        QList<Graph*> graphsToRemove;
        foreach (Graph *g, graphs) {
            if (!sources.first().contains(g->dataSeriesName())) {
                // data series is not in the list anymore
                graphsToRemove.append(g);
            } else {
                // data series is still in the list, but not new
                sourcesToAdd.removeAll(g->dataSeriesName());
            }
        }

        foreach (Graph *g, graphsToRemove) {
            graphs.removeAll(g);
            ui.plot->removeGraph(g->getGraph());
            g->deleteLater();
        }

        foreach (QString source, sourcesToAdd) {
            addSource(source);
        }

        // only show the legend if the track is not empty
        ui.plot->legend->setVisible(ui.plot->graphCount() > 0);

        if (currentScalingMode == PlotSettings::NOSCALING) {
            ui.plot->rescaleValueAxes();
        }
        ui.plot->replot();
    }
}

void Track::onPlotSettings()
{
    PlotSettings settings = PlotSettingsDialog::getSettings(getFullDataSeriesNames(), currentSettings(), true, false);

    if (!settings.isEmpty()) {
        if (settings.scalingMode == PlotSettings::MINMAXSCALING) {
            // all graphs will scale their values to use the full height of the plot
            ui.plot->yAxis->setScaleType(QCPAxis::stLinear);
            ui.plot->yAxis->setRange(0, 1);
            // set axis and gridlines invisible as they will have no informative value
            // TODO(Steffi): reaktivieren
//            ui.plot->yAxis->setVisible(false);
            ui.plot->yAxis->setGrid(false);
        } else if (settings.scalingMode == PlotSettings::NOSCALING) {
            if (settings.plotScaleType == PlotSettings::LOGSCALE) {
                ui.plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
            } else {
                ui.plot->yAxis->setScaleType(QCPAxis::stLinear);
            }
            ui.plot->yAxis->setVisible(true);
            ui.plot->yAxis->setGrid(true);
        }

        currentScalingMode = settings.scalingMode;

        foreach (Graph *g, graphs) {
            g->update(settings);
        }
    }
}

void Track::onOptPlotMarginsRecalculated(int left, int right, int top, int bottom)
{
    ui.plot->setMarginRight(50);
    ui.plot->setMarginTop(top);
    ui.plot->setMarginBottom(bottom);

    if (optPlotMarginLeft != left) {
        optPlotMarginLeft = left;
        emit optPlotMarginsChanged();
    }
}

int Track::getMarginLeft()
{
    return ui.plot->pos().x() + ui.plot->marginLeft();
}

int Track::getMarginRight()
{
    return ui.plot->marginRight();
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
    QVariantList graphList;
    foreach (Graph *g, graphs) {
        QVariantMap graph;
        graph.insert("name", g->dataSeriesName());
        graph.insert("scaling", g->getScaleType());

        graphList << graph;
    }    
    qvm->insert("dataSeries", graphList);
}

void Track::load(QVariantMap *qvm)
{
    QVariantMap map = qvm->find("track").value().toMap();
    QVariantList graphList = map.find("dataSeries").value().toList();

    foreach (QVariant entry, graphList) {
        QVariantMap graph = entry.toMap(); // one graph object (contains dataSeries + Scaling)
        QString name(graph.find("name").value().toString()); // name of the series
        addSource(name);
        int scaling = graph.find("scaling").value().toInt();
        foreach (Graph *g, graphs) { // look for the graph which was just added.
            if (g->dataSeriesName() == name) {
                g->setScaleType((PlotSettings::ScaleType) scaling);
                break; // there won't be the same graph in this track again --> we can quit the loop
            }
        }
    }
}

