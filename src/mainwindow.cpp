#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "gui/exampletrack.h"
#include <QDebug>
#include "gui/presentationarea.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    setUpToolBar();

    ui.timeLine->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #FFA000, stop: 1 #FF4500);"
                                "padding-right: 3px;"
                                "padding-left: 10px;");
    ui.timeLine->setText("Timeline");

    pa = new PresentationArea();
    ui.scrollArea->setWidget(pa);
//    ui->scrollArea->setStyleSheet("border-top: 0px solid;");
}

MainWindow::~MainWindow()
{

}

void MainWindow::onAddTrackAction()
{    
    ExampleTrack *et = new ExampleTrack();
    connect(this, SIGNAL(visibleRangeChanged(int,int,int)), et, SLOT(adjustVisibleRange(int,int,int)));
    pa->addTrack(et);
}

void MainWindow::onImportAction()
{
    // TODO:
    qDebug() << "Import";
}

void MainWindow::onExportAction()
{
    // TODO:
    qDebug() << "Export";
}

void MainWindow::onPlayAction()
{
    // TODO:
    qDebug() << "Play";
}

void MainWindow::on_horizontalScrollBar_rangeChanged(int min, int max)
{
//    qDebug() << "Range changed(min, max):  (" << min << ", " << max << ")";
    emit visibleRangeChanged(ui.horizontalScrollBar->sliderPosition(), min, max);
}

void MainWindow::on_horizontalScrollBar_sliderMoved(int position)
{
//    qDebug() << "Slider moved: " << position;
    emit visibleRangeChanged(position, ui.horizontalScrollBar->minimum(), ui.horizontalScrollBar->maximum());
}

void MainWindow::setUpToolBar()
{
    addTrackAction = new QAction(QIcon(":/buttons/toolbar/add.png"), tr("Add Track"), this);
    importAction = new QAction(QIcon(":/buttons/toolbar/import.png"), tr("Import"), this);
    exportAction = new QAction(QIcon(":/buttons/toolbar/export.png"), tr("Export"), this);
    playAction = new QAction(QIcon(":/buttons/toolbar/play.png"), tr("Play"), this);

    connect(addTrackAction, SIGNAL(triggered()), this, SLOT(onAddTrackAction()));
    connect(importAction, SIGNAL(triggered()), this, SLOT(onImportAction()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(onExportAction()));
    connect(playAction, SIGNAL(triggered()), this, SLOT(onPlayAction()));

    ui.mainToolBar->addAction(addTrackAction);
    ui.mainToolBar->addAction(importAction);
    ui.mainToolBar->addAction(exportAction);
    ui.mainToolBar->addAction(playAction);
}
