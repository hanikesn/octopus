#include "mainwindow.h"

#include "gui/presentationarea.h"
#include "gui/mainview.h"
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>

#include <QDebug>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    setUpButtonBars();

    pa = new PresentationArea(&trackScene);
    pa->setPos(0, 0);
    ui.mainView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui.mainView->setScene(&trackScene);

    connect(ui.mainView, SIGNAL(changedRange(qint64, qint64)), pa, SIGNAL(onRangeChanged(qint64, qint64)));
    ui.timeLine->setText("TimeLine");
    connect(ui.mainView, SIGNAL(changedRange(qint64, qint64)), ui.timeLine, SLOT(adjustVisibleRange(qint64, qint64)));
}

MainWindow::~MainWindow()
{

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
    spacerLeft = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    spacerRight = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    ui.bottomButtonBar->addSpacerItem(spacerLeft);
    ui.bottomButtonBar->addWidget(&playButton);
    ui.bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&addTrackButton, SIGNAL(clicked()), pa, SLOT(onAddTrack()));
    connect(&importButton, SIGNAL(clicked()), this, SLOT(onImportAction()));
    connect(&exportButton, SIGNAL(clicked()), this, SLOT(onExportAction()));
    connect(&playButton, SIGNAL(clicked()), this, SLOT(onPlayAction()));

    ui.mainToolBar->addWidget(&toolBarWidget);
}



