#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QCloseEvent>

#include "ui_mainwindow.h"
#include "serializer.h"
#include "parser.h"
#include "gui/startscreen.h"
#include "timemanager.h"
#include "exporthandler.h"
#include "viewmanager.h"

const QString MainWindow::TITLE = "Octopus 0.1";

MainWindow::MainWindow(StartScreen::Type type, QWidget *parent) :
    QMainWindow(parent),        
    ui(new Ui::MainWindow()),
    viewManager(0)
{
    ui->setupUi(this);

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

    if(type == StartScreen::LOAD) {
        if(onLoad().isEmpty())
            onNew();
    } else {
        onNew();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
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
    followDataButton.setText(tr("Pin to new data"));

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
    ui->bottomButtonBar->addSpacerItem(spacerLeft);
    ui->bottomButtonBar->addWidget(&followDataButton);
    ui->bottomButtonBar->addWidget(&playButton);
    ui->bottomButtonBar->addWidget(&recButton);
    ui->bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&loadButton, SIGNAL(clicked()), this, SLOT(onLoad()));        

    connect(&playButton, SIGNAL(clicked()), this, SIGNAL(play()));
    connect(&recButton, SIGNAL(clicked()), this, SIGNAL(record()));
    connect(&followDataButton, SIGNAL(clicked()), this, SIGNAL(follow()));

    ui->mainToolBar->addWidget(&toolBarWidget);
    addToolBar(Qt::LeftToolBarArea, ui->mainToolBar);
}

void MainWindow::setUpMenu()
{
    menu.setTitle(tr("&File"));
    menu.addAction(newAction);
    menu.addAction(loadAction);
    menu.addAction(saveAction);    
    menu.addAction(saveAsAction);
    menu.addAction(quitAction);
    ui->menuBar->addMenu(&menu);
}

void MainWindow::onSave()
{
    save(false);
}

void MainWindow::onSaveAs()
{
    save(true);
}

QString MainWindow::onLoad()
{    
    if (checkForUnsavedChanges() == QMessageBox::Abort) return "";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                                                    projectPath, "Octopus (*.oct)");

    if(fileName.isEmpty()) return fileName;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);    

    QByteArray json = file.readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(json, &ok).toMap();
    if (!ok){
        qDebug() << "Could not parse config file:" << fileName << " ! Aborting...";
        return "";
    }


    QString dbfile = QFileInfo(file).absolutePath() + "/" + result["dbfile"].toString();

    ViewManager* tmp;
    try {
        tmp = new ViewManager(this, dbfile);
    } catch(std::exception const& e) {
        QMessageBox::critical(this, tr("Octopus"), tr("Could not load DB."));
        return projectPath;
    }

    if(viewManager)
        viewManager->deleteLater();

    viewManager = tmp;

    projectPath = fileName;

    setTitle(QFileInfo(projectPath).completeBaseName().remove(".oct"));

    viewManager->load(&result);

    // no recording in a loaded project.
    recButton.setEnabled(false);

    // we can't follow new data.
    followDataButton.setEnabled(false);

    // enable saving
    saveAction->setEnabled(true);

    setUpView();
    return fileName;
}

void MainWindow::onNew()
{
    if (checkForUnsavedChanges() == QMessageBox::Abort) return;

    projectPath = "";
    setTitle("");

    // enable recording
    recButton.setEnabled(true);

    // enable data following
    followDataButton.setEnabled(true);

    // disable saving
    saveAction->setEnabled(false);

    if(viewManager)
        viewManager->deleteLater();

    viewManager = new ViewManager(this);

    setUpView();
}

void MainWindow::setTitle(QString pName)
{
    QString windowTitle(TITLE);
    if (!pName.isEmpty())
        windowTitle += " - ";

    windowTitle += pName;
    setWindowTitle(windowTitle);
}

void MainWindow::setUpView()
{
    QSignalMapper* mapZoom = new QSignalMapper(this);
    mapZoom->setMapping(&zoomInButton, 100);
    mapZoom->setMapping(&zoomOutButton, -100);
    connect(&zoomOutButton, SIGNAL(clicked()), mapZoom, SLOT(map()));
    connect(&zoomInButton, SIGNAL(clicked()), mapZoom, SLOT(map()));
    connect(mapZoom, SIGNAL(mapped(int)),   viewManager, SIGNAL(zoom(int)));

    connect(this, SIGNAL(follow()),         viewManager, SIGNAL(follow()));
    connect(this, SIGNAL(play()),           viewManager, SIGNAL(play()));
    connect(this, SIGNAL(record()),         viewManager, SLOT(onRecord()));

    connect(viewManager, SIGNAL(saveProject(qint64,qint64)), this, SLOT(onSaveProject(qint64,qint64)));
    connect(viewManager, SIGNAL(recordEnabled(bool)), this, SLOT(onRecordEnabled(bool)));
    connect(viewManager, SIGNAL(followEnabled(bool)), this, SLOT(onFollowEnabled(bool)));
    connect(viewManager, SIGNAL(playEnabled(bool)), this, SLOT(onPlayEnabled(bool)));

    // corresponding connect-statements:
    connect(&addTrackButton, SIGNAL(clicked()), viewManager, SIGNAL(addTrack()));
    connect(&plotSettingsButton, SIGNAL(clicked()), viewManager, SIGNAL(plotSettings()));    
    connect(&exportButton, SIGNAL(clicked()), viewManager, SIGNAL(exportData()));    

    ui->centralWidgetLayout->insertWidget(0, viewManager);
}

void MainWindow::save(bool saveAs, qint64 begin, qint64 end)
{
    QString fileName = getSaveFileName(saveAs);
    if (fileName.isEmpty()) return;  // dialog cancelled

    QString dbname = fileName + ".db";
    dbname.remove(QRegExp(".oct$"));

    QString relative_dbname = QFileInfo(fileName).dir().relativeFilePath(dbname);

    QVariantMap pName; // Map with the projects settings    
    if ((begin == -1) && (end == -1)) { // save all
        projectPath = fileName;
        viewManager->saveDB(dbname, -1, -1);
        pName.insert("dbfile", relative_dbname);

        if (writeProjectSettings(pName, projectPath)) { // in case save was successfull ...
            viewManager->setUnsavedChanges(false);
        }
    } else { // save range
        QString subProjectPath = fileName;        
        viewManager->saveDB(dbname, begin, end);
        pName.insert("dbfile", relative_dbname);
        writeProjectSettings(pName, subProjectPath);
    }
}

int MainWindow::checkForUnsavedChanges()
{
    if (!viewManager || !viewManager->hasUnsavedChanges()) return QMessageBox::Discard;

    QMessageBox msg;
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort);
    msg.setIcon(QMessageBox::Information);
    msg.setButtonText(QMessageBox::Save, tr("Save"));
    msg.setButtonText(QMessageBox::Discard, tr("Discard"));
    msg.setButtonText(QMessageBox::Abort, tr("Cancel"));
    msg.setDefaultButton(QMessageBox::Save);
    msg.setText(tr("This project contains unsaved changes or new data which have been received in the background.\n"
                   "Do you wish to save the current state?"));
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
    if (viewManager && viewManager->isRecording()) { // ask whether recording should be stopped
        // simulate button click
        viewManager->onRecord();

        if (viewManager->isRecording()) { // if there is still a running recording, the user continued!
            ce->ignore();
            return; // dont exit the program
        }
    }

    // usual check for unsaved changes in the project
    if (checkForUnsavedChanges() != QMessageBox::Abort)
        QMainWindow::closeEvent(ce);
    else
        ce->ignore();
}

void MainWindow::onRecordEnabled(bool recording)
{
    recButton.setChecked(recording);
}

void MainWindow::onSaveProject(qint64 start, qint64 end)
{
    // initiate save (project file)
    save(true, start, end);
}

void MainWindow::onFollowEnabled(bool follow)
{
    followDataButton.setChecked(follow);
    playButton.setChecked(follow);

}

void MainWindow::onPlayEnabled(bool play)
{
    if (followDataButton.isChecked())
        followDataButton.setChecked(false);
    playButton.setChecked(play);
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
