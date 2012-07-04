#include "viewmanager.h"

#include "dataprovider.h"
#include "exporthandler.h"
#include "mainwindow.h"
#include "networkadapter.h"
#include "timemanager.h"
#include "recorder.h"
#include "gui/presentationarea.h"
#include "gui/timescrollbar.h"

#include <QDateTime>
#include <QFileDialog>

ViewManager::ViewManager(QWidget *parent, QString dbfile):
    QWidget(parent),
    dataProvider(0),
    networkAdapter(0),
    exportHandler(0),
    presentationArea(0),
    timeManager(0),
    recorder(0),
    scrollBar(0)
{
    setLayout(new QVBoxLayout());
    layout()->setMargin(0);
    layout()->setSpacing(0);

    createViewAndModel(dbfile);
}

void ViewManager::load(QVariantMap *qvm)
{
    // propagate load-call
    dataProvider->load(qvm);
    presentationArea->load(qvm);

    // no changes directly after loading:
    presentationArea->setUnsavedChanges(false);
    timeManager->setUnsavedChanges(false);
}

void ViewManager::save(QVariantMap *qvm)
{
    // propagate save-call
    presentationArea->save(qvm);
    dataProvider->save(qvm);
}

void ViewManager::saveDB(QString dbname, qint64 begin, qint64 end)
{
    dataProvider->copyDB(dbname, begin, end);
}

bool ViewManager::hasUnsavedChanges()
{
    return presentationArea->hasUnsavedChanges() || timeManager->hasUnsavedChanges();
}

void ViewManager::setUnsavedChanges(bool uc)
{
    presentationArea->setUnsavedChanges(uc);
    timeManager->setUnsavedChanges(uc);
}

bool ViewManager::isRecording()
{
    return recorder->isRecording();
}

void ViewManager::onRecord()
{
    recorder->toggleRecording();
    emit recordEnabled(recorder->isRecording());
}

void ViewManager::createViewAndModel(QString dbfile)
{
    // delete previous objects:
    if (networkAdapter) {
        networkAdapter->deleteLater();
        networkAdapter = 0;
    }
    if (exportHandler)
        exportHandler->deleteLater();

    if (dbfile.isEmpty()) { // new-action
        if (dataProvider) {
            dataProvider->closeDB();
            dataProvider->deleteLater();
        }
        // create a temporary file for the db
        dataProvider = new DataProvider(dbfile, this);
        // create networkAdapter because we expect new data
        networkAdapter = new NetworkAdapter(this);

    } else { // load-action, we need no networkAdapter because we don't expect any new data
        DataProvider* olddataProvider = dataProvider;
        dataProvider = new DataProvider(dbfile, this);

        if(olddataProvider) {
            olddataProvider->closeDB();
            olddataProvider->deleteLater();
        }
    }    
    // We need to export stuff in any case:
    exportHandler = new ExportHandler(this, dataProvider);

    // create view-relevant objects:
    setUpView();
}

void ViewManager::setUpView()
{
    scrollBar = new TimeScrollbar(this);
    scrollBar->setOrientation(Qt::Horizontal);

    if (networkAdapter)
        timeManager = new TimeManager(networkAdapter->getStartTime(), this);
    else
        timeManager = new TimeManager(TimeManager::Clock::time_point(), this);

    scrollBar->onRangeChanged(timeManager->getLowVisRange(), timeManager->getHighVisRange());

    presentationArea = new PresentationArea(*dataProvider, timeManager, scrollBar, this);
    recorder = new Recorder(timeManager, this);

    this->layout()->addWidget(presentationArea);
    this->layout()->addWidget(scrollBar);

    makeConnects();
}

void ViewManager::makeConnects()
{
    // export
    connect(this, SIGNAL(exportData()), exportHandler, SLOT(onExport()));

    // timeManager
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), scrollBar, SLOT(onRangeChanged(qint64,qint64)));
    connect(timeManager, SIGNAL(newMax(qint64)), scrollBar, SLOT(onNewMax(qint64)));
    connect(timeManager, SIGNAL(followEnabled(bool)), this, SIGNAL(followEnabled(bool)));
    connect(timeManager, SIGNAL(playEnabled(bool)), this, SIGNAL(playEnabled(bool)));
    connect(this, SIGNAL(play()), timeManager, SLOT(onPlay()));
    connect(this, SIGNAL(follow()), timeManager, SLOT(onFollow()));
    connect(this, SIGNAL(zoom(int)), timeManager, SLOT(zoom(int)));
    connect(scrollBar, SIGNAL(rangeChanged(qint64,qint64)), timeManager, SLOT(setRange(qint64,qint64)));

    // presentationArea
    connect(this, SIGNAL(plotSettings()), presentationArea, SLOT(onPlotSettings()));
    connect(this, SIGNAL(addTrack()), presentationArea, SLOT(onAddTrack()));    
    connect(presentationArea, SIGNAL(exportRange(qint64,qint64)), exportHandler, SLOT(onExport(qint64,qint64)));
    connect(presentationArea, SIGNAL(selectionChanged(qint64,qint64)), exportHandler, SLOT(onSelectionChanged(qint64,qint64)));

    // recorder
    connect(recorder, SIGNAL(saveProject(qint64,qint64)), this, SIGNAL(saveProject(qint64,qint64)));
    connect(recorder, SIGNAL(record(qint64,qint64,bool)), presentationArea, SIGNAL(record(qint64,qint64,bool)));

    // networkAdapter if available
    if (networkAdapter) {
        qRegisterMetaType<EIDescriptionWrapper>("EIDescriptionWrapper");
        qRegisterMetaType<Value>("Value");
        connect(networkAdapter, SIGNAL(onNewSender(EIDescriptionWrapper)),  dataProvider, SLOT(onNewSender(EIDescriptionWrapper)), Qt::QueuedConnection);
        connect(networkAdapter, SIGNAL(onNewData(qint64,QString,Value)),    dataProvider, SLOT(onNewData(qint64,QString,Value)), Qt::QueuedConnection);
        networkAdapter->discoverSenders();
    }
}
