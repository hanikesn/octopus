#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <sstream>

#include "dataprovider.h"
#include "gui/presentationarea.h"
#include "gui/mainview.h"
#include "serializer.h"
#include "parser.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    projectName(QString::fromUtf8("projectName")),
    projectPath(QString::fromUtf8(""))
{    
    ui.setupUi(this);
    dataProvider = new DataProvider();
    trackScene = new TrackScene(this);
    pa = new PresentationArea(trackScene, *dataProvider, ui.hScrollBar, this);

    saveAction = new QAction(tr("Save"), this);
    saveAsAction = new QAction(tr("Save As ..."), this);
    loadAction = new QAction(tr("Load..."), this);

    ui.mainView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui.mainView->setScene(trackScene);
    ui.mainView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);    
    ui.mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // TODO(domi): nicht vergessen :)
//    connect(ui.mainView, SIGNAL(changedRange(qint64, qint64)), pa, SLOT(onRangeChanged(qint64, qint64)));
    connect(ui.mainView, SIGNAL(verticalScroll()), this, SLOT(onVerticalScroll()));
    connect(this, SIGNAL(verticalScroll(QRectF)), pa, SIGNAL(verticalScroll(QRectF)));
    connect(this, SIGNAL(changedWindowSize(QSize)), pa, SLOT(onChangedWindowSize(QSize)));
    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(onSaveAs()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoad()));

    connect(&networkAdapter, SIGNAL(onNewDataSeries(QString,QString,Data::Properties)), dataProvider, SLOT(onNewDataSeries(QString,QString,Data::Properties)));
    connect(&networkAdapter, SIGNAL(onNewData(qint64,QString,Value)),                   dataProvider, SLOT(onNewData(qint64,QString,Value)));

    setUpButtonBars();
    setUpMenu();
}

MainWindow::~MainWindow()
{
    dataProvider->deleteLater();
    trackScene->deleteLater();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    /* One resizeEvent is fired right after initialisation of window.
       We dont want to receive this --> wait for first "actual" resizeEvent.*/
    if(event->oldSize().isValid())
        emit changedWindowSize(ui.mainView->size());
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
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
}

void MainWindow::onVerticalScroll()
{    
    emit verticalScroll(ui.mainView->mapToScene(ui.mainView->viewport()->geometry()).boundingRect());
}

void MainWindow::onExportRange(qint64 begin, qint64 end)
{
    qDebug() << "MainWindow::onExportRange " << begin << ":" << end;
}

void MainWindow::setUpMenu()
{
    menu.setTitle(tr("File"));
    menu.addAction(saveAction);
    menu.addAction(loadAction);
    menu.addAction(saveAsAction);
    ui.menuBar->addMenu(&menu);
}

void MainWindow::onSave()
{
    save(false);
}

void MainWindow::onSaveAs()
{
    save(true);
}

void MainWindow::onLoad()
{        
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                                                    QDir::currentPath(), "Octopus (*.oct)");
    if(fileName.isEmpty()) return;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);    

    QByteArray json = file.readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();
    if(!ok){
        qDebug() << "Could not parse config file! Aborting...";
        return;
    }

    // at this point loading was successful --> delete old presentationArea and create new one.
    setUpView();
    projectName = result["projectName"].toString();
    setTitle(projectName);

    pa->load(&result);    
}

void MainWindow::setTitle(QString pName)
{
    QString windowTitle("Octopus 0.1 - ");
    windowTitle += pName;
    setWindowTitle(windowTitle);
}

void MainWindow::setUpView()
{
    dataProvider->deleteLater();
    dataProvider = 0;
    trackScene->deleteLater();
    trackScene = 0;
    pa->deleteLater();
    pa = 0;

    dataProvider = new DataProvider();
    trackScene = new TrackScene(this);
    ui.mainView->setScene(trackScene);
    pa = new PresentationArea(trackScene, *dataProvider, ui.hScrollBar, this);
    // set new PA to current viewsize
    pa->onChangedWindowSize(ui.mainView->size());

    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pa, SIGNAL(verticalScroll(QRectF)));
    connect(this, SIGNAL(changedWindowSize(QSize)), pa, SLOT(onChangedWindowSize(QSize)));
    connect(&addTrackButton, SIGNAL(clicked()), pa, SLOT(onAddTrack()));
}

void MainWindow::save(bool saveAs)
{
    if (projectPath.isEmpty() || saveAs) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        QDir::currentPath(), "Octopus (*.oct)");
        if (fileName.isEmpty()) return;

        if (fileName.endsWith(".oct") == false)
            fileName += ".oct";

        projectPath = fileName;
        if (!saveAs){
            projectName = QFileInfo(fileName).completeBaseName().remove(".oct");
            setTitle(projectName);
        }
    }
    QVariantMap pName;
    pName.insert("projectName", projectName);
    pa->save(&pName);

    QJson::Serializer serializer;
    serializer.setIndentMode(QJson::IndentFull);
    QByteArray json = serializer.serialize(pName);

    // open/create the file
    QFile file(projectPath);
    file.open(QIODevice::WriteOnly);
    if(file.write(json) == -1)
        qDebug() << "Could not write config file!";
}
