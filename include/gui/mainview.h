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
    void verticalScroll();
    void resized(QSize size);

public slots:

protected:
    void scrollContentsBy(int dx, int dy);

    void resizeEvent(QResizeEvent *event);
};

#endif // MAINVIEW_H
