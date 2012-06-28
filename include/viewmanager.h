#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>

#include "serializable.h"

class DataProvider;
class NetworkAdapter;
class ExportHandler;
class PresentationArea;
class TimeManager;
class Recorder;
class TimeScrollbar;
class MainWindow;

class ViewManager : public QObject, public Serializable
{
    Q_OBJECT
public:
    explicit ViewManager(MainWindow *parent, TimeScrollbar *sb);
    ~ViewManager();

    void load(QVariantMap *qvm);
    void save(QVariantMap *qvm);

    void saveDB(QString dbname, qint64 begin, qint64 end);

    bool hasUnsavedChanges();
    void setUnsavedChanges(bool uc);

    bool isRecording();

    PresentationArea* getPresentationArea() {return presentationArea;}

    /**
      * Creates a new view (dataProvider, exportHandler, presentationArea, timeManager, recorder)
      * and if necessary a networkAdapter (not necessary if a database is loaded).
      * If dbfile is not empty, the dataProvider tries to load the specified file.
      * @param dbfile - The dbfile to be loaded.
      */
    PresentationArea* createNewView(QString dbfile = "");

signals:
    void newMax(qint64 max);
    void rangeChanged(qint64 begin, qint64 end);
    void saveProject(qint64,qint64);
    void setRange(qint64 begin, qint64 end);

    // activated by button in mainwindow
    void addTrack();
    // activated by button in mainwindow
    void exportData();
    // activated by button in mainwindow
    void play();
    // activated by button in mainwindow
    void follow(bool follow);
    // activated by button in mainwindow
    void plotSettings();
    // activated by button in mainwindow and mouse wheel
    void zoom(int z);

public slots:
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

    void setUpView();

    void makeConnects();

};

#endif // VIEWMANAGER_H
