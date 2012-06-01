#ifndef Track_H
#define Track_H

#include "dataprovider.h"
#include "ui_track.h"

#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

class AbstractDataSeries;

class Track : public QWidget
{
    Q_OBJECT
public:
    Track(const DataProvider *dataProvider, QWidget *parent = 0);
    Track(const DataProvider *dataProvider, const QString &fullDataSeriesName, QWidget *parent = 0);

signals:
    void del(Track*);

public slots:
    void setPlotRange(qint64 begin, qint64 end);
    void onNewData(const AbstractDataSeries *series, qint64 timeStamp);

private:
    Ui::Track ui;
    static const QString ICON_AS_BUTTON;

    const DataProvider *dataProvider;
    QList<AbstractDataSeries*> dataSeries;

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
