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

    void load(QVariantMap *qvm);
    void save(QVariantMap *qvm);

    void saveDB(QString dbname, qint64 begin, qint64 end);

    QString getDBName();

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


    void recordEnabled(bool recording);
    void followEnabled(bool f);
    void playEnabled(bool f);

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

    void createNewView(QString dbfile);

};

#endif // VIEWMANAGER_H
