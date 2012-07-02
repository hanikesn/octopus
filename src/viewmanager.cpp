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

    createNewView(dbfile);
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

QString ViewManager::getDBName()
{
    return dataProvider->getDBFileName();
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
    emit record(recorder->isRecording(), timeManager->getMaximum());
}

void ViewManager::createNewView(QString dbfile)
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
        dataProvider = new DataProvider(dbfile, this);
        networkAdapter = new NetworkAdapter(this);

    } else { // load-action
        DataProvider* olddataProvider = dataProvider;
        dataProvider = new DataProvider(dbfile, this);

        if(olddataProvider) {
            olddataProvider->closeDB();
            olddataProvider->deleteLater();
        }
    }
    exportHandler = new ExportHandler(this, dataProvider);
    setUpView();
        addData(*dataProvider);
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
    connect(this, SIGNAL(play()), timeManager, SLOT(onPlay()));
    connect(this, SIGNAL(follow(bool)), timeManager, SLOT(onFollow(bool)));
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
