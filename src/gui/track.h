#ifndef Track_H
#define Track_H

#include "ui_track.h"

#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

class Track : public QWidget
{
    Q_OBJECT
public:
    explicit Track(QWidget *parent = 0);

signals:
    void del(Track*);

public slots:    
    void setPlotRange(qint64 begin, qint64 end);

private:
    void addData();

    Ui::Track ui;
    static const QString ICON_AS_BUTTON;

private slots:
    void onDelete();
    void onSources();
    void onPlotSettings();
};

#endif // Track_H
