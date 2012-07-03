#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dataprovider.h"
#include "networkadapter.h"
#include "mainwindow.h"
#include "gui/startscreen.h"

#include <QAction>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QWidget>
#include <QMenu>
#include <QAction>


class TrackScene;
class Recorder;
class ExportHandler;
class ViewManager;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(StartScreen::Type type, QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *ce);
    
private slots:        
    void onSave();
    void onSaveAs();
    QString onLoad();
    void onNew();    
    /**
      * This saves the recorded data into a new project (project file and new database is created).
      * @param start Timestamp for the begin of the recorded data.
      * @param end Timestamp for the end of the recorded data.
      */
    void onSaveProject(qint64 start, qint64 end);

    void onRecordEnabled(bool recording);

    void onFollowEnabled(bool follow);

    void onPlayEnabled(bool play);

private:
    QVBoxLayout *trackLayout;

    // Buttons and layout for toolbar:
    QWidget toolBarWidget;
    QVBoxLayout layout;
    QPushButton addTrackButton;
    QPushButton plotSettingsButton;
    QPushButton loadButton;
    QPushButton exportButton;
    QPushButton zoomInButton;
    QPushButton zoomOutButton;
    QPushButton playButton;    
    QIcon playButtonIcon;
    QPushButton recButton;
    QIcon recButtonIcon;
    QPushButton followDataButton;

    // Spacer item to center the play button (in the bottomButtonBar)
    QSpacerItem *spacerLeft;
    QSpacerItem *spacerRight;

    // Menu
    QMenu menu;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *loadAction;
    QAction *newAction;
    QAction *quitAction;

    ViewManager *viewManager;

    Ui::MainWindow* ui;

    QString projectPath;

    const static QString TITLE;

    void setUpButtonBars();

    void setUpMenu();

    void setTitle(QString pName);

    void setUpView();

    /**
      * Saves the current projects configuration (position of cursor, view range...).
      * If begin and end are != -1 a new databse will be generated with the data in the range (begin, end).
      * To save all recorded data, leave begin and end with their default values.
      * @param saveAs If this flag is set to true, the user will be asked for a new projectName
      *               'save as'-semantics.
      *               If this flag is false the user will be asked for a projectName only if this
      *               project hasn't been saved before.
      * @param begin Timestamp for the begin of the recorded data.
      * @param end Timestamp for the end of the recorded data.
      */
    void save(bool saveAs, qint64 begin = -1, qint64 end = -1);

    int checkForUnsavedChanges();

    QString getSaveFileName(bool saveAs);

    bool writeProjectSettings(QVariantMap pName, QString path);

signals:
    void timeStepChanged(int);
    void follow();
    void record();
    void play();
};

#endif // MAINWINDOW_H
