#ifndef EXAMPLETRACK_H
#define EXAMPLETRACK_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include "ui_track.h"

// TODO(Steffi): Rename
class ExampleTrack : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleTrack(QWidget *parent = 0);

signals:
    
public slots:
    // TODO(Steffi): in qint64 ändern; position rausschmeißen
    void setPlotRange(int position, int rangeMin, int rangeMax);

private:
    void addData();

    Ui::Track ui;
};

#endif // EXAMPLETRACK_H
