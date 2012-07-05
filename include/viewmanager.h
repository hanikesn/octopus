/*
Contains all objects which are necessary to display data on the screen (PresentationArea and data
model)
*/
#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>

#include "serializable.h"

class DataProvider;
class NetworkAdapter;
class ExportHandler;
class PresentationArea;
class TimeManager;
class Recorder;
class TimeScrollbar;
class MainWindow;

class ViewManager : public QWidget, public Serializable
{
    Q_OBJECT
public:
    /**
      * Creates a new view (dataProvider, exportHandler, presentationArea, timeManager, recorder)
      * and if necessary a networkAdapter (not necessary if a database is loaded).
      * If dbfile is not empty, the dataProvider tries to load the specified file.
      * @param dbfile - The dbfile to be loaded.
      */
    explicit ViewManager(QWidget *parent, QString dbfile = QString());

    /**
      * Propagates the load-call to presentationArea and dataProvider.
      * @param qvm The map in which the information is stored.
      */
    void load(QVariantMap *qvm);

    /**
      * Propagates the save-call to presentationArea and dataProvider.
      * @param qvm The map in which the information needs to be stored.
      */
    void save(QVariantMap *qvm, qint64 begin, qint64 end);

    /**
      * Makes a copy of the current database with all data which have begin <= timestamp <= end.
      * @param dbname Filename of the new database
      * @param begin First timestamp to save
      * @param end Last timestamp to save
      */
    void saveDB(QString dbname, qint64 begin, qint64 end);

    bool hasUnsavedChanges();
    void setUnsavedChanges(bool uc);

    bool isRecording();

signals:
    void saveProject(qint64,qint64);

    // activated by button in mainwindow
    void addTrack();
    // activated by button in mainwindow
    void exportData();
    // activated by button in mainwindow
    void play();
    // activated by button in mainwindow
    void follow();
    // activated by button in mainwindow
    void plotSettings();
    // activated by button in mainwindow and mouse wheel
    void zoom(int z);

    /**
      * This signal is emitted when 'onRecord()' is called and contains the state of the recorder.
      * @param recording True if recorder is currently recording, false otherwise.
      */
    void recordEnabled(bool recording);

    /**
      * This signals propagates timeManagers signal 'followEnabled(bool)'
      * @param f True if following new data, false otherwise
      */
    void followEnabled(bool f);

    /**
      * This signals propagates timeManagers signal 'playEnabled(bool)'
      * @param p True if playing is enabled, false otherwise
      */
    void playEnabled(bool p);

public slots:    
    /**
      * This slot is called when the user presses the record-button in the gui.
      * Updates the recorder.
      */
    void onRecord();

private:
    MainWindow *parent;
    DataProvider *dataProvider;
    NetworkAdapter *networkAdapter;
    ExportHandler *exportHandler;
    PresentationArea *presentationArea;
    TimeManager *timeManager;
    Recorder *recorder;

    TimeScrollbar *scrollBar;

    /**
      * Creates the view which includes: ScrollBar, PresentationArea, TimeManager and the Recorder
      * and initiates the necessary connects.
      */
    void setUpView();

    /**
      * Makes connects between: ViewManager, ExportHandler, TimeManager, Scrollbar,
      * PresentationArea, Recorder, NetworkAdapter and DataProvider.
      */
    void makeConnects();

    /**
      * Creates the data model for the view and the view itself.
      * @param dbfile Path to the db. In case it is empty a new temporary db will be created. If
      *               dbfile is not empty the database is going to be loaded and no new data (from
      *               the network) will be accepted.
      */
    void createViewAndModel(QString dbfile);
};

#endif // VIEWMANAGER_H
