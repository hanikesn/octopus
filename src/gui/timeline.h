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
    void adjustVisibleRange(qint64 begin, qint64 end);

private:

};

#endif // TIMELINE_H
