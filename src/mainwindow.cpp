#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QDateTime>
#include <QSignalMapper>
#include <QCloseEvent>

#include "dataprovider.h"
#include "gui/presentationarea.h"
#include "serializer.h"
#include "parser.h"
#include "gui/startscreen.h"
#include "timemanager.h"
#include "recorder.h"
#include "exporthandler.h"
#include "viewmanager.h"

const QString MainWindow::TITLE = "Octopus 0.1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),        
    selectionBegin(-1),
    selectionEnd(-1)
{    
    ui.setupUi(this);

    viewManager = new ViewManager(this, ui.hScrollBar);
    connect(viewManager, SIGNAL(newMax(qint64)), ui.hScrollBar, SLOT(onNewMax(qint64)));
    connect(viewManager, SIGNAL(rangeChanged(qint64,qint64)), ui.hScrollBar, SLOT(onRangeChanged(qint64,qint64)));
    connect(ui.hScrollBar, SIGNAL(rangeChanged(qint64,qint64)), viewManager, SIGNAL(setRange(qint64,qint64)));


    QSignalMapper* mapZoom = new QSignalMapper(this);
    mapZoom->setMapping(&zoomInButton, 100);
    mapZoom->setMapping(&zoomOutButton, -100);
    connect(&zoomOutButton, SIGNAL(clicked()), mapZoom, SLOT(map()));
    connect(&zoomInButton, SIGNAL(clicked()), mapZoom, SLOT(map()));
    connect(mapZoom, SIGNAL(mapped(int)),   viewManager, SIGNAL(zoom(int)));
    connect(this, SIGNAL(follow(bool)),     viewManager, SIGNAL(follow(bool)));


    connect(viewManager, SIGNAL(saveProject(qint64,qint64)), this, SLOT(onSaveProject(qint64,qint64)));


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

    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(onSaveAs()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoad()));
    connect(newAction, SIGNAL(triggered()), this, SLOT(onNew()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setUpButtonBars();
    setUpMenu();

    //TODO(domi): Kommentare wegmachen
//    StartScreen *s = new StartScreen(this);
//    if (s->showScreen() == StartScreen::LOAD)
//            onLoad();
//    else
//        onNew();
    viewManager->createNewView();
    onNew();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setUpButtonBars()
{    
    addTrackButton.setIcon(QIcon(":/buttons/toolbar/icons/add_16.png"));
    plotSettingsButton.setIcon(QIcon(":/buttons/toolbar/settings"));
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

    followDataButton.setCheckable(true);
    followDataButton.setText(tr("ADf"));

    // corresponding connect-statements:
    connect(&addTrackButton, SIGNAL(clicked()), viewManager, SIGNAL(addTrack()));
    connect(&plotSettingsButton, SIGNAL(clicked()), viewManager, SIGNAL(plotSettings()));
    connect(&playButton, SIGNAL(clicked()), viewManager, SIGNAL(play()));
    connect(&recButton, SIGNAL(clicked()), viewManager, SLOT(onRecord()));
    connect(&exportButton, SIGNAL(clicked()), viewManager, SIGNAL(exportData()));

    // add buttons to the vertical layout in the toolbar
    layout.addWidget(&addTrackButton);
    layout.addWidget(&plotSettingsButton);
    layout.addWidget(&loadButton);
    layout.addWidget(&exportButton);
    layout.addWidget(&zoomInButton);
    layout.addWidget(&zoomOutButton);

    // buttonBar at the bottom:
    // set up spacers so they get as much space as possible (buttons between are then centered)
    spacerLeft = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    spacerRight = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    ui.bottomButtonBar->addSpacerItem(spacerLeft);
    ui.bottomButtonBar->addWidget(&followDataButton);
    ui.bottomButtonBar->addWidget(&playButton);
    ui.bottomButtonBar->addWidget(&recButton);
    ui.bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&loadButton, SIGNAL(clicked()), this, SLOT(onLoad()));
    connect(&recButton, SIGNAL(clicked()), this, SLOT(onRecord()));
    connect(&followDataButton, SIGNAL(clicked()), this, SLOT(onFollowData()));
    connect(&playButton, SIGNAL(clicked()), this, SLOT(onPlay()));

    ui.mainToolBar->addWidget(&toolBarWidget);
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
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
    if (checkForUnsavedChanges() == QMessageBox::Abort) return;
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

    QString dbfile = result["dbfile"].toString();

    if (viewManager->createNewView(dbfile) == 0) return;

    // at this point loading was successful --> delete old presentationArea and create new one.
    projectPath = fileName;

    setTitle(QFileInfo(projectPath).completeBaseName().remove(".oct"));

    viewManager->load(&result);

    // no recording in a loaded project.
    recButton.setEnabled(false);
    ui.verticalLayout_2->insertWidget(0, viewManager->getPresentationArea());
}

void MainWindow::onNew()
{
    if (checkForUnsavedChanges() == QMessageBox::Abort) return;

    ui.verticalLayout_2->insertWidget(0, viewManager->createNewView());

    projectPath = "";
    setTitle("");

    // enable recording
    recButton.setEnabled(true);
}

void MainWindow::setTitle(QString pName)
{
    QString windowTitle(TITLE);
    if(!pName.isEmpty())
        windowTitle += " - ";

    windowTitle += pName;
    setWindowTitle(windowTitle);
}

void MainWindow::save(bool saveAs, qint64 begin, qint64 end)
{
    QString fileName = getSaveFileName(saveAs);
    if (fileName.isEmpty()) return;  // dialog cancelled

    // The db lies in a temporary file. We might need to move it.
    QString dbname = fileName + ".db";
    dbname.remove(QRegExp(".oct$"));

    QVariantMap pName; // Map with the projects settings    
    if ((begin == -1) && (end == -1)) { // save all
        projectPath = fileName;
        viewManager->saveDB(dbname, -1, -1);
        pName.insert("dbfile", dbname);

        if (writeProjectSettings(pName, projectPath)) { // in case save was successfull ...
            viewManager->setUnsavedChanges(false);
        }
    } else { // save range
        QString subProjectPath = fileName;        
        viewManager->saveDB(dbname, begin, end);
        pName.insert("dbfile", dbname);
        writeProjectSettings(pName, subProjectPath);
    }
}

int MainWindow::checkForUnsavedChanges()
{        
    if (!viewManager->hasUnsavedChanges()) return -1;

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
//    if (recorder->isRecording()) { // ask whether recording should be stopped
//        // simulate button click
//        recorder->toggleRecording();

//        if (recorder->isRecording()) { // if there is still a running recording, the user continued!
//            ce->ignore();
//            return; // dont exit the program
//        }
//        onRecord(); // in case the recording was stopped, set corresponding state of rec button.
//    }

//    // usual check for unsaved changes in the project
//    if (checkForUnsavedChanges() != QMessageBox::Abort)
//        QMainWindow::closeEvent(ce);
//    else
//        ce->ignore();
}

void MainWindow::onRecord()
{        
    recButton.setChecked(viewManager->isRecording());
}

void MainWindow::onSaveProject(qint64 start, qint64 end)
{
    // initiate save (project file)
    save(true, start, end);
}

void MainWindow::onFollowData()
{
    playButton.setChecked(followDataButton.isChecked());
    emit follow(followDataButton.isChecked());
}

void MainWindow::onPlay()
{
    if (followDataButton.isChecked())
        followDataButton.setChecked(false);
}

bool MainWindow::writeProjectSettings(QVariantMap pName, QString path)
{
    viewManager->save(&pName);

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
