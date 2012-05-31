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
    explicit Track(DataProvider *dataProvider, QWidget *parent = 0);

signals:
    void del(Track*);

public slots:
    void setPlotRange(qint64 begin, qint64 end);
    void onNewData(const AbstractDataSeries *series, qint64 timeStamp);

private:
    Ui::Track ui;
    static const QString ICON_AS_BUTTON;

    DataProvider *dataProvider;

    void setupButtons();
    void setupPlot();

    void addData();

private slots:
    void onDelete();
    void onSources();
    void onPlotSettings();
};

#endif // Track_H
