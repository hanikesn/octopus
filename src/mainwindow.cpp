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
    pa = new PresentationArea(&trackScene, dataProvider, ui.hScrollBar);

    saveAction = new QAction(tr("Save"), this);
    loadAction = new QAction(tr("Load..."), this);

    ui.mainView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui.mainView->setScene(&trackScene);
    ui.mainView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);    
    ui.mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // TODO(domi): nicht vergessen :)
//    connect(ui.mainView, SIGNAL(changedRange(qint64, qint64)), pa, SLOT(onRangeChanged(qint64, qint64)));
    connect(ui.mainView, SIGNAL(verticalScroll()), this, SLOT(onVerticalScroll()));
    connect(this, SIGNAL(verticalScroll(QRectF)), pa, SIGNAL(verticalScroll(QRectF)));
    connect(this, SIGNAL(changedWindowSize(QSize)), pa, SLOT(onChangedWindowSize(QSize)));
    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoad()));

    setUpButtonBars();
    setUpMenu();

}

MainWindow::~MainWindow()
{
    delete pa;
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
    ui.menuBar->addMenu(&menu);
}

void MainWindow::onSave()
{
    if(projectPath.isEmpty()){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        QDir::currentPath(), "Octopus (*.oct)");
        if(fileName.isEmpty()) return;

        if(fileName.endsWith(".oct") == false)
            fileName += ".oct";

        projectPath = fileName;
        projectName = QFileInfo(fileName).completeBaseName().remove(".oct");
        setTitle(projectName);
    }
    QVariantMap pName;
    pName.insert("projectName", projectName);
    pa->save(&pName);

    QJson::Serializer serializer;
    serializer.setIndentMode(QJson::IndentMode::IndentFull);
    QByteArray json = serializer.serialize(pName);

    // open/create the file
    QFile file(projectPath);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << json;
    out.flush();
    file.close();
}

void MainWindow::onLoad()
{    
    //TODO(domi): laden
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                                                    QDir::currentPath(), "Octopus (*.oct)");
    if(fileName.isEmpty()) return;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    QByteArray json;
    QByteArray tmp;
    in >> tmp;
    while(!tmp.isEmpty()){
        json.append(tmp);
        in >> tmp;
    }

    file.close();

    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();
    if(!ok){
        qDebug() << "Could not parse config file! Aborting...";
        return;
    }

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
