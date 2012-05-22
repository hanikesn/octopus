#ifndef TIMELINE_H
#define TIMELINE_H

#include <QLabel>
#include <QWidget>


class TimeLine : public QLabel
{
    Q_OBJECT
public:
    explicit TimeLine(QWidget *parent = 0);

signals:

public slots:
    void adjustVisibleRange(int position, int rangeMin, int rangeMax);

private:

};

#endif // TIMELINE_H
