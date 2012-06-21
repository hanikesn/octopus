#ifndef Track_H
#define Track_H

#include "ui_track.h"
#include "visitor.h"
#include "serializable.h"

#include <QWidget>

class Graph;
class DataProvider;
class AbstractDataSeries;

class Track : public QWidget, public DataSeriesVisitor, public Serializable
{
    Q_OBJECT
public:
    int optPlotMarginLeft;

    Track(const DataProvider &dataProvider, QWidget *parent = 0);
    Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent = 0);
    Track(const DataProvider &dataProvider, const QStringList &fullDataSeriesNames, QWidget *parent = 0);

    /**
     * Visitor pattern.
     */
    void addGraph(const DoubleSeries &s);
    void addGraph(const StringSeries &s);

    void mousePressEvent(QMouseEvent *);

    int getPlotOffset();
    void setPlotMarginLeft(int margin);

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

signals:
    void optPlotMarginsChanged();
    void del(Track*);

public slots:
    void setPlotRange(qint64 begin, qint64 end);

private:
    Ui::Track ui;
    static const QString ICON_AS_BUTTON;

    const DataProvider &dataProvider;
    QList<Graph*> graphs;

    void init();
    void setupButtons();
    void setupPlot();

    void addSource(const QString &fullDataSeriesName);

    QStringList getFullDataSeriesNames();

private slots:
    void onDelete();
    void onSources();
    void onPlotSettings();
    void onOptPlotMarginsRecalculated(int left, int /*right*/, int /*top*/, int /*bottom*/);
};

#endif // Track_H
