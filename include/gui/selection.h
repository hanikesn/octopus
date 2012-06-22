#ifndef SELECTION_H
#define SELECTION_H

#include <QPen>
#include <QWidget>

class QMenu;
class QAction;
class PresentationItem;
class TimeManager;

class Selection : public QWidget
{
    Q_OBJECT
public:
    explicit Selection(TimeManager *timeManager, QWidget *parent);
    ~Selection();

    void paintEvent(QPaintEvent *);

    // TODO REF
    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void hide();
    void show();

public slots:
    void setSelectionBegin(qint64 begin);
    void setSelectionEnd(qint64 end);

    void updateHeight(int h);

    void update();

signals:
    void onExport(qint64 begin, qint64 end);

    void selectionChanged(qint64 begin, qint64 end);

private slots:

    void exportTriggered();

private:

    qint64 begin;
    qint64 end;

    QPen pen;
    QBrush brush;

    QMenu *menu;
    QAction *exportAction;

    TimeManager* timeManager;
};

#endif // SELECTION_H
