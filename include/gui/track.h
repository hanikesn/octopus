#ifndef Track_H
#define Track_H

#include "dataprovider.h"
#include "ui_track.h"
#include "visitor.h"
#include "gui/graph.h"
#include "serializable.h"
#include "boost/property_tree/ptree.hpp"

#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

class AbstractDataSeries;

class Track : public QWidget, public DataSeriesVisitor, public Serializable
{
    Q_OBJECT
public:
    Track(const DataProvider &dataProvider, QWidget *parent = 0);
    Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent = 0);

    /**
     * Visitor pattern.
     */
    void addGraph(const DoubleSeries &s);
    void addGraph(const StringSeries &s);

    void setOffset(int pixel);

    void save(boost::property_tree::ptree *pt);
    void load(boost::property_tree::ptree *pt);

signals:
    void del(Track*);

public slots:
    void setPlotRange(qint64 begin, qint64 end);

private:
    Ui::Track ui;
    static const QString ICON_AS_BUTTON;

    const DataProvider &dataProvider;
    QList<Graph*> graphs;

    int offset;

    void setupButtons();
    void setupPlot();

    // TODO(Steffi): Remove (dummy func)
    void addData();
    void addSource(const QString &fullDataSeriesName);

private slots:
    void onDelete();
    void onSources();
    void onPlotSettings();
    void onNewMax(qint64 timestamp);
};

#endif // Track_H
