#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QDateTime>

#include "gui/sourcedialog.h"
#include "dataprovider.h"
#include "gui/presentationarea.h"
#include "gui/mainview.h"
#include "serializer.h"
#include "parser.h"
#include "CVSExporter.h"
#include "gui/startscreen.h"

const QString MainWindow::TITLE = "Octopus 0.1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pa(0),
    dataProvider(0),
    trackScene(0)
{    
    ui.setupUi(this);

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveAsAction = new QAction(tr("Save As ..."), this);
    saveAsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    loadAction = new QAction(tr("Load..."), this);
    loadAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(QKeySequence(QKeySequence::Quit));

    ui.mainView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui.mainView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui.mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(ui.mainView, SIGNAL(resized(QSize)), this, SIGNAL(changedViewSize(QSize)));
    connect(ui.mainView, SIGNAL(verticalScroll()), this, SLOT(onVerticalScroll()));

    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(onSaveAs()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoad()));
    connect(newAction, SIGNAL(triggered()), this, SLOT(onNew()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    exporterFactory.addExporter(std::unique_ptr<Exporter>(new CVSExporter()));

    setUpButtonBars();
    setUpMenu();

    onNew();
    //TODO(domi): Kommentare wegmachen
//    StartScreen *s = new StartScreen(this);
//    if (s->showScreen() == StartScreen::LOAD)
//        onLoad();
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

    connect(&importButton, SIGNAL(clicked()), this, SLOT(onImportAction()));
    connect(&exportButton, SIGNAL(clicked()), this, SLOT(onExportAction()));

    ui.mainToolBar->addWidget(&toolBarWidget);
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
}

void MainWindow::onVerticalScroll()
{    
    emit verticalScroll(ui.mainView->mapToScene(ui.mainView->viewport()->geometry()).boundingRect());
}

void MainWindow::onExportRange(qint64 begin, qint64 end)
{
    QStringList sources = SourceDialog::getSources(*dataProvider, tr("Export"), false, QStringList(), this).front();

    QFileDialog dialog(this, tr("Export"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    dialog.setNameFilters(exporterFactory.names());
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if(fileNames.isEmpty())
        return;

    QFile file(fileNames.first());
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,tr("Error"), tr("Could not safe file."));
        return;
    }

    exporterFactory.getExporter(dialog.selectedNameFilter()).write(file, *dataProvider, sources, begin, end);

    qDebug() << "MainWindow::onExportRange " << begin << ":" << end << fileNames << sources;
}

void MainWindow::setUpMenu()
{
    menu.setTitle(tr("&File"));
    menu.addAction(newAction);
    menu.addAction(loadAction);
    menu.addAction(saveAction);    
    menu.addAction(saveAsAction);
    menu.addAction(quitAction);
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
    if(checkForUnsavedChanges() == QMessageBox::Abort) return;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                                                    projectPath, "Octopus (*.oct)");
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
    DataProvider* olddataProvider = dataProvider;
    QString dbfile = result["dbfile"].toString();
    // load the db might fail
    try {
        dataProvider = new DataProvider(dbfile, this);
    } catch(std::exception& e) {
        qDebug() << "Loading of DB failed.";
        return;
    }

    // at this point loading was successful --> delete old presentationArea and create new one.
    projectPath = fileName;

    if(olddataProvider) {
        olddataProvider->closeDB();
        olddataProvider->deleteLater();
    }

    setUpView();
    setTitle(QFileInfo(projectPath).completeBaseName().remove(".oct"));

    pa->load(&result);
    pa->setUnsavedChanges(false);
}

static void addData(DataProvider& dp)
{
    EI::Description desc1("Dummy", "dum");
    desc1.addDataSeries("Interpolatable.x", EI::DataSeriesInfo(EI::Value::DOUBLE, EI::DataSeriesInfo::INTERPOLATABLE,""));
    desc1.addDataSeries("Interpolatable.y", EI::DataSeriesInfo(EI::Value::DOUBLE, EI::DataSeriesInfo::INTERPOLATABLE,""));
    desc1.addDataSeries("Discrete", EI::DataSeriesInfo(EI::Value::DOUBLE, EI::DataSeriesInfo::STATEFUL,""));
    dp.onNewSender(desc1);

    EI::Description desc2("Dummy-2", "dum");
    desc2.addDataSeries("Interpolatable.x", EI::DataSeriesInfo(EI::Value::DOUBLE, EI::DataSeriesInfo::INTERPOLATABLE,""));
    desc2.addDataSeries("Interpolatable.y", EI::DataSeriesInfo(EI::Value::DOUBLE, EI::DataSeriesInfo::INTERPOLATABLE,""));
    dp.onNewSender(desc2);

     for (int j=0; j<500; ++j)
    {
      double d = j/15.0 * 5*3.14 + 0.01;
      dp.onNewData(d*1000000, "Dummy.Interpolatable.x", Value(7*sin(d)/d));
      dp.onNewData(d*1000000, "Dummy.Interpolatable.y", Value(-7*sin(d)/d));
      dp.onNewData(d*1000000, "Dummy.Discrete", Value("ping"));
    }
}

void MainWindow::onNew()
{
    if(checkForUnsavedChanges() == QMessageBox::Abort) return;

    if(dataProvider) {
        dataProvider->closeDB();
        dataProvider->deleteLater();
    }
    // create a temporary file for the db
    dataProvider = new DataProvider(QDir::tempPath() + "/Octopus-" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"), this);

    setUpView();
    addData(*dataProvider);
    projectPath = "";
    setTitle("");
}

void MainWindow::setTitle(QString pName)
{
    QString windowTitle(TITLE);
    if(!pName.isEmpty())
        windowTitle += " - ";

    windowTitle += pName;
    setWindowTitle(windowTitle);
}

void MainWindow::setUpView()
{
    if (trackScene)
        trackScene->deleteLater();
    if (pa)
        pa->deleteLater();

    trackScene = new TrackScene(this);

    pa = new PresentationArea(trackScene, *dataProvider, ui.hScrollBar, this);
    ui.mainView->setScene(trackScene);

    // set new PA to current viewsize
    pa->onChangedViewSize(ui.mainView->size());

    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(this, SIGNAL(verticalScroll(QRectF)), pa, SIGNAL(verticalScroll(QRectF)));
    connect(this, SIGNAL(changedViewSize(QSize)), pa, SLOT(onChangedViewSize(QSize)));
    connect(&addTrackButton, SIGNAL(clicked()), pa, SLOT(onAddTrack()));

    qRegisterMetaType<EIDescriptionWrapper>("EIDescriptionWrapper");
    qRegisterMetaType<Value>("Value");
    connect(&networkAdapter, SIGNAL(onNewSender(EIDescriptionWrapper)), dataProvider, SLOT(onNewSender(EIDescriptionWrapper)), Qt::QueuedConnection);
    connect(&networkAdapter, SIGNAL(onNewData(qint64,QString,Value)),      dataProvider, SLOT(onNewData(qint64,QString,Value)), Qt::QueuedConnection);

    networkAdapter.discoverSenders();

    connect(&playButton, SIGNAL(clicked()), pa, SLOT(onPlay()));
}

void MainWindow::save(bool saveAs)
{
    QString caption = saveAs ? tr("Save as") : tr("Save");

    if (projectPath.isEmpty() || saveAs) {
        QString fileName = QFileDialog::getSaveFileName(this, caption,
                                                        projectPath, "Octopus (*.oct)");
        if (fileName.isEmpty()) return;

        if (fileName.endsWith(".oct") == false)
            fileName += ".oct";

        if(projectPath.isEmpty()) {
            // The db lies in a temporary file. We need too move it.
            QString dbname = fileName;
            dbname.remove(QRegExp(".oct$"));
            dataProvider->moveDB(dbname + ".db");
        }

        projectPath = fileName;

        if (!saveAs){
            setTitle(QFileInfo(fileName).completeBaseName().remove(".oct"));
        }
    }
    QVariantMap pName;
    pName.insert("dbfile", dataProvider->getDBFileName());
    pa->save(&pName);

    QJson::Serializer serializer;
    serializer.setIndentMode(QJson::IndentFull);
    QByteArray json = serializer.serialize(pName);

    // open/create the file
    QFile file(projectPath);
    file.open(QIODevice::WriteOnly);
    if(file.write(json) == -1)
        qDebug() << "Could not write config file!";

    pa->setUnsavedChanges(false);
}

int MainWindow::checkForUnsavedChanges()
{
    if(!pa || !pa->hasUnsavedChanges())
        return -1;

    QMessageBox msg;
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort);
    msg.setIcon(QMessageBox::Information);
    msg.setButtonText(QMessageBox::Save, tr("Save"));
    msg.setButtonText(QMessageBox::Discard, tr("Discard"));
    msg.setButtonText(QMessageBox::Abort, tr("Cancel"));
    msg.setDefaultButton(QMessageBox::Save);
    msg.setText(tr("There are some unsaved changes in this project. Do you wish to save these?"));
    int result = msg.exec();
    if (result == QMessageBox::Save)
        save(false);
    return result;
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
    //TODO(domi): Kommentare wegmachen:
//    if(checkForUnsavedChanges() != QMessageBox::Abort)
//        QMainWindow::closeEvent(ce);
//    else
//        ce->ignore();
}
