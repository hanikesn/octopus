#ifndef SELECTION_H
#define SELECTION_H

#include <QPen>
#include <QWidget>

class QMenu;
class PresentationItem;
class TimeManager;

class Selection : public QWidget
{
    Q_OBJECT
public:
    explicit Selection(TimeManager *timeManager, QWidget *parent);

    void hide();
    void show();

public slots:
    void setSelectionBegin(qint64 begin);
    void setSelectionEnd(qint64 end);

    void updateHeight(int h);

    void onUpdate();

signals:
    void onExport(qint64 begin, qint64 end);

    void selectionChanged(qint64 begin, qint64 end);

protected slots:
    void exportTriggered();

protected:
    void contextMenuEvent(QContextMenuEvent * event);
    void paintEvent(QPaintEvent *);

private:

    qint64 begin;
    qint64 end;

    QPen pen;
    QBrush brush;

    QMenu *menu;

    TimeManager* timeManager;
};

#endif // SELECTION_H
