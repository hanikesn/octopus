#include "mainwindow.h"
#include <QWidget>
#include "gui/exampletrack.h"
#include "gui/presentationarea.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    setUpButtonBars();
    ui.scrollArea->setWidget(&pa);
    ui.timeLine->setText("TimeLine");
}

MainWindow::~MainWindow()
{

}

void MainWindow::onAddTrackAction()
{    
    ExampleTrack *et = new ExampleTrack();
    connect(this, SIGNAL(visibleRangeChanged(int,int,int)), et, SLOT(adjustVisibleRange(int,int,int)));
    connect(this, SIGNAL(visibleRangeChanged(int,int,int)), ui.timeLine, SLOT(adjustVisibleRange(int,int,int)));
    pa.addTrack(et);
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

void MainWindow::setUpButtonBars()
{
    addTrackButton.setIcon(QIcon(":/buttons/toolbar/icons/add.png"));
    importButton.setIcon(QIcon(":/buttons/toolbar/icons/import.png"));
    exportButton.setIcon(QIcon(":/buttons/toolbar/icons/export.png"));
    playButton.setIcon(QIcon(":/buttons/toolbar/icons/play.png"));

    // add buttons to the horizontal layout in the toolbar
    layout.addWidget(&addTrackButton);
    layout.addWidget(&importButton);
    layout.addWidget(&exportButton);

    // buttonBar at the bottom:
    // set up spacers so they get as much space as possible (button between is then centered)
    spacerLeft = new QSpacerItem (100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    spacerRight = new QSpacerItem (100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    ui.bottomButtonBar->addSpacerItem(spacerLeft);
    ui.bottomButtonBar->addWidget(&playButton);
    ui.bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&addTrackButton, SIGNAL(clicked()), this, SLOT(onAddTrackAction()));
    connect(&importButton, SIGNAL(clicked()), this, SLOT(onImportAction()));
    connect(&exportButton, SIGNAL(clicked()), this, SLOT(onExportAction()));
    connect(&playButton, SIGNAL(clicked()), this, SLOT(onPlayAction()));

    ui.mainToolBar->addWidget(&toolBarWidget);
}
