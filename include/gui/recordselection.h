#ifndef RECORDSELECTION_H
#define RECORDSELECTION_H

#include <QPen>
#include <QWidget>

class TimeManager;

class RecordSelection : public QWidget
{
    Q_OBJECT
public:
    explicit RecordSelection(TimeManager *timeManager, QWidget *parent);

public slots:
    void setSelectionBegin(qint64 begin);
    void setSelectionEnd(qint64 end);

    void updateHeight(int h);

    void onUpdate();

    void onRecord(qint64 start, qint64 end, bool recording);

protected:
    void paintEvent(QPaintEvent *);

private:
    qint64 begin;
    qint64 end;

    bool recording;

    QPen pen;
    QBrush brush;

    TimeManager* timeManager;
};

#endif //RECORDSELECTION_H
