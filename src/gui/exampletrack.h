#ifndef EXAMPLETRACK_H
#define EXAMPLETRACK_H

#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>


class ExampleTrack : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleTrack(QWidget *parent = 0);

    /**
      * Necessary for using stylesheets.
      */
    void paintEvent(QPaintEvent *qpe);

signals:
    
public slots:

    void adjustVisibleRange(int position, int rangeMin, int rangeMax);

private:
    static const QString NORMAL_STYLE;

    QPainter p;
    QStyleOption opt;

    QLabel actionArea;
    QLabel customPlot;

};

#endif // EXAMPLETRACK_H
