#include "gui/mainview.h"
#include <QScrollBar>

#include <QDebug>

MainView::MainView(QWidget *parent):
    QGraphicsView(parent)
{
}

MainView::~MainView()
{

}

void MainView::scrollContentsBy(int dx, int dy)
{
    // scroll only vertically
    if(dx == 0){
        QGraphicsView::scrollContentsBy(dx, dy);
        emit verticalScroll();
    }
}

void MainView::resizeEvent(QResizeEvent *event)
{
    emit resized(size());
}
