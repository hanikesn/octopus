#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

class MainView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainView(QWidget *parent = 0);

    ~MainView();

signals:
    void changedRange(qint64 begin, qint64 end);
    void verticalScroll();

public slots:

protected:
    void scrollContentsBy(int dx, int dy);
};

#endif // MAINVIEW_H
