#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QDateTime>
//#include <QKeySequence>

#include "gui/sourcedialog.h"
#include "dataprovider.h"
#include "gui/presentationarea.h"
#include "serializer.h"
#include "parser.h"
#include "CVSExporter.h"
#include "gui/startscreen.h"

const QString MainWindow::TITLE = "Octopus 0.1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pa(0),
    dataProvider(0)
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

    //connect(ui.mainView, SIGNAL(verticalScroll()), this, SLOT(onVerticalScroll()));

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

void MainWindow::onExportAction()
{
    // TODO:
    qDebug() << "Export";
}

void MainWindow::setUpButtonBars()
{    
    addTrackButton.setIcon(QIcon(":/buttons/toolbar/icons/add_16.png"));    
    loadButton.setIcon(QIcon(":/buttons/toolbar/icons/document-open-3_16.png"));
    exportButton.setIcon(QIcon(":/buttons/toolbar/icons/document-export-4_16.png"));
    zoomInButton.setIcon(QIcon(":/buttons/toolbar/icons/zoom-in_16.png"));
    zoomOutButton.setIcon(QIcon(":/buttons/toolbar/icons/zoom-out_16.png"));

    playButtonIcon.addPixmap(QPixmap(":/buttons/toolbar/icons/play_16.png"), QIcon::Normal,
                             QIcon::Off);
    playButtonIcon.addPixmap(QPixmap(":/buttons/toolbar/icons/pause_16.png"), QIcon::Normal,
                             QIcon::On);
    playButton.setCheckable(true);
    playButton.setIcon(playButtonIcon);    
    playButton.setShortcut(QKeySequence(Qt::Key_Space));

    recButtonIcon.addPixmap(QPixmap(":/buttons/toolbar/icons/record_16.png"), QIcon::Normal,
                             QIcon::Off);
    recButtonIcon.addPixmap(QPixmap(":/buttons/toolbar/icons/record_16.png"), QIcon::Normal,
                             QIcon::On);
    recButton.setCheckable(true);
    recButton.setIcon(recButtonIcon);

    // add buttons to the horizontal layout in the toolbar
    layout.addWidget(&addTrackButton);
    layout.addWidget(&loadButton);
    layout.addWidget(&exportButton);
    layout.addWidget(&zoomInButton);
    layout.addWidget(&zoomOutButton);

    // buttonBar at the bottom:
    // set up spacers so they get as much space as possible (button between is then centered)
    spacerLeft = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    spacerRight = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    ui.bottomButtonBar->addSpacerItem(spacerLeft);
    ui.bottomButtonBar->addWidget(&playButton);
    ui.bottomButtonBar->addWidget(&recButton);
    ui.bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&loadButton, SIGNAL(clicked()), this, SLOT(onLoad()));
    connect(&exportButton, SIGNAL(clicked()), this, SLOT(onExportAction()));    

    ui.mainToolBar->addWidget(&toolBarWidget);
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
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
        QMessageBox::critical(this,tr("Error"), tr("Could not save file."));
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
    if (pa)
        pa->deleteLater();

    pa = new PresentationArea(*dataProvider, ui.hScrollBar, this);
    ui.verticalLayout_2->insertWidget(0, pa);

    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(pa, SIGNAL(saveProject(qint64,qint64)), this, SLOT(onSaveProject(qint64,qint64)));
    connect(&zoomInButton, SIGNAL(clicked()), pa, SIGNAL(zoomIn()));
    connect(&zoomOutButton, SIGNAL(clicked()), pa, SIGNAL(zoomOut()));
    connect(&addTrackButton, SIGNAL(clicked()), pa, SLOT(onAddTrack()));

    qRegisterMetaType<EIDescriptionWrapper>("EIDescriptionWrapper");
    qRegisterMetaType<Value>("Value");
    connect(&networkAdapter, SIGNAL(onNewSender(EIDescriptionWrapper)), dataProvider, SLOT(onNewSender(EIDescriptionWrapper)), Qt::QueuedConnection);
    connect(&networkAdapter, SIGNAL(onNewData(qint64,QString,Value)),      dataProvider, SLOT(onNewData(qint64,QString,Value)), Qt::QueuedConnection);

    networkAdapter.discoverSenders();

    connect(&playButton, SIGNAL(clicked()), pa, SLOT(onPlay()));
    connect(&recButton, SIGNAL(clicked()), pa, SLOT(onRecord()));
    connect(&recButton, SIGNAL(clicked()), this, SLOT(onRecord()));
}

void MainWindow::save(bool saveAs, qint64 begin, qint64 end)
{
    QString fileName = getSaveFileName(saveAs);
    if (fileName.isEmpty()) return;  // dialog cancelled

    // The db lies in a temporary file. We might need to move it.
    QString dbname = fileName;
    dbname.remove(QRegExp(".oct$"));

    QVariantMap pName; // Map with the projects settings
    if ((begin == -1) && (end == -1)) { // save all
        projectPath = fileName;
        dataProvider->moveDB(dbname + ".db"); // move tmp-database if we save all
        pName.insert("dbfile", dataProvider->getDBFileName());

        if (writeProjectSettings(pName, projectPath)) // in case save was successfull ...
            pa->setUnsavedChanges(false); // ... clear flag in PresentationArea
    } else { // save range
        QString subProjectPath = fileName;
        // TODO(steffen): dataProvider->movePartDB(dbname  + ".db", begin, end)    oder so ähnlich
        pName.insert("dbfile", dbname);
        writeProjectSettings(pName, subProjectPath);
    }
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

QString MainWindow::getSaveFileName(bool saveAs)
{
    QString caption = saveAs ? tr("Save as") : tr("Save");

    if (projectPath.isEmpty() || saveAs) { // determine new filename
        QString fileName = QFileDialog::getSaveFileName(this, caption,
                                                        projectPath, "Octopus (*.oct)");
        if (fileName.isEmpty()) return fileName;

        if (fileName.endsWith(".oct") == false)
            fileName += ".oct";
        if (!saveAs)
            setTitle(QFileInfo(fileName).completeBaseName().remove(".oct"));
        return fileName;
    } else
        return projectPath;
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
    //TODO(domi): Kommentare wegmachen:
//    if(checkForUnsavedChanges() != QMessageBox::Abort)
//        QMainWindow::closeEvent(ce);
//    else
//        ce->ignore();
}

void MainWindow::onRecord()
{    
    if (pa->isRecording())
        recButton.setChecked(true);
    else
        recButton.setChecked(false);
}

void MainWindow::onSaveProject(qint64 start, qint64 end)
{
    qDebug() << Q_FUNC_INFO << start << "|" << end;

    // initiate save (project file)
    save(true, start, end);

    //TODO(steffen): initiate save (database with range (start, end)

    //TODO(domi): Projekt speichern, in die DB nur Bereich aufnehmen
}

bool MainWindow::writeProjectSettings(QVariantMap pName, QString path)
{
    pa->save(&pName);

    QJson::Serializer serializer;
    serializer.setIndentMode(QJson::IndentFull);
    QByteArray json = serializer.serialize(pName);

    // open/create the file
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    if(file.write(json) == -1) {
        qDebug() << "Could not write config file!";
        return false;
    }
    return true;
}
