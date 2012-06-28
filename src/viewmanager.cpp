#include "viewmanager.h"

#include <QFileDialog>
#include <QDateTime>


#include "dataprovider.h"
#include "networkadapter.h"
#include "exporthandler.h"
#include "gui/presentationarea.h"
#include "timemanager.h"
#include "recorder.h"
#include "mainwindow.h"
#include "gui/timescrollbar.h"

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
      dp.onNewData(d*1000000, "Dummy.Interpolatable.x", Value(14*sin(d)/d + 3));
      dp.onNewData(d*1000000, "Dummy.Interpolatable.y", Value(-7*sin(d)/d));
      dp.onNewData(d*1000000, "Dummy.Discrete", Value("ping"));
    }
}

ViewManager::ViewManager(MainWindow *parent, TimeScrollbar *sb):
    QObject(parent),
    parent(parent),
    dataProvider(0),
    networkAdapter(0),
    exportHandler(0),
    presentationArea(0),
    timeManager(0),
    recorder(0),
    scrollBar(sb)
{

}

ViewManager::~ViewManager()
{
    if (networkAdapter) networkAdapter->deleteLater();
    if (exportHandler) exportHandler->deleteLater();
}

void ViewManager::load(QVariantMap *qvm)
{
    dataProvider->load(qvm);
    presentationArea->load(qvm);
    presentationArea->setUnsavedChanges(false);
    timeManager->setUnsavedChanges(false);
}

void ViewManager::save(QVariantMap *qvm)
{
    presentationArea->save(qvm);
    dataProvider->save(qvm);
}

void ViewManager::saveDB(QString dbname, qint64 begin, qint64 end)
{
    if ((begin == -1) && (end == -1)) { // save all
        dataProvider->moveDB(dbname);
    } else {
        dataProvider->copyDB(dbname, begin, end);
    }
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
}

PresentationArea *ViewManager::createNewView(QString dbfile)
{
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
        dataProvider = new DataProvider(QDir::tempPath() + "/Octopus-" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"), this);
        networkAdapter = new NetworkAdapter();

    } else { // load-action
        DataProvider* olddataProvider = dataProvider;
        try {
            dataProvider = new DataProvider(dbfile, this);
        } catch(std::exception& e) {
            qDebug() << "Loading of DB failed.";
            return 0;
        }
        if(olddataProvider) {
            olddataProvider->closeDB();
            olddataProvider->deleteLater();
        }
    }
    exportHandler = new ExportHandler(dataProvider);
    setUpView();
    //    addData(*dataProvider);
    return presentationArea;
}

void ViewManager::setUpView()
{
    if (recorder)
        recorder->deleteLater();
    if (presentationArea)
        presentationArea->deleteLater();
    if (timeManager)
        timeManager->deleteLater();

    if (networkAdapter)
        timeManager = new TimeManager(scrollBar, networkAdapter->getStartTime(), this);
    else
        timeManager = new TimeManager(scrollBar, TimeManager::Clock::time_point(), this);

    scrollBar->onRangeChanged(timeManager->getLowVisRange(), timeManager->getHighVisRange());

    presentationArea = new PresentationArea(*dataProvider, timeManager, parent);
    recorder = new Recorder(timeManager, this);

    makeConnects();
}

void ViewManager::makeConnects()
{
    // export
    connect(this, SIGNAL(exportData()), exportHandler, SLOT(onExport()));

    // timeManager
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), this, SIGNAL(rangeChanged(qint64,qint64)));
    connect(timeManager, SIGNAL(newMax(qint64)), this, SIGNAL(newMax(qint64)));
    connect(timeManager, SIGNAL(rangeChanged(qint64,qint64)), this, SIGNAL(rangeChanged(qint64,qint64)));
    connect(this, SIGNAL(play()), timeManager, SLOT(onPlay()));
    connect(this, SIGNAL(setRange(qint64,qint64)), timeManager, SLOT(setRange(qint64,qint64)));
    connect(this, SIGNAL(follow(bool)), timeManager, SLOT(onFollow(bool)));
    connect(this, SIGNAL(zoom(int)), timeManager, SLOT(zoom(int)));

    // presentationArea
    connect(this, SIGNAL(plotSettings()), presentationArea, SLOT(onPlotSettings()));
    connect(this, SIGNAL(addTrack()), presentationArea, SLOT(onAddTrack()));
    connect(presentationArea, SIGNAL(exportRange(qint64,qint64)), exportHandler, SLOT(onExport(qint64,qint64)));
    connect(presentationArea, SIGNAL(selectionChanged(qint64,qint64)), exportHandler, SLOT(onSelectionChanged(qint64,qint64)));

    // recorder
    connect(recorder, SIGNAL(saveProject(qint64,qint64)), this, SIGNAL(saveProject(qint64,qint64)));

    // networkAdapter if available
    if (networkAdapter) {
        qRegisterMetaType<EIDescriptionWrapper>("EIDescriptionWrapper");
        qRegisterMetaType<Value>("Value");
        connect(networkAdapter, SIGNAL(onNewSender(EIDescriptionWrapper)),  dataProvider, SLOT(onNewSender(EIDescriptionWrapper)), Qt::QueuedConnection);
        connect(networkAdapter, SIGNAL(onNewData(qint64,QString,Value)),    dataProvider, SLOT(onNewData(qint64,QString,Value)), Qt::QueuedConnection);
        networkAdapter->discoverSenders();
    }
}









