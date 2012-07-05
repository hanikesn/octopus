/*
MainWindow of Octopus. Contains the buttons, toolbar, and the ViewManager.
Handles save/load/close actions.
*/
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
    /**
      * Reimplemented from QWidget.
      * Handles window close events.
      */
    void closeEvent(QCloseEvent *ce);
    
private slots:        
    /**
      * Calls save(bool, qint64, qint64).
      */
    void onSave();

    /**
      * Calls save(bool, qint64, qint64).
      */
    void onSaveAs();

    /**
      * Handles load-action (checking for unsaved changes, parsing config file, propagating
      * load-call to necessary classes, creating view)
      * @return FileName of the loaded config file.
      */
    QString onLoad();

    /**
      * Checks for unsaved changes and creates new view.
      */
    void onNew();    

    /**
      * This saves the recorded data into a new project (project file and new database is created).
      * @param start Timestamp for the begin of the recorded data.
      * @param end Timestamp for the end of the recorded data.
      */
    void onSaveProject(qint64 start, qint64 end);

    /**
      * Set the state of the record button.
      */
    void onRecordEnabled(bool recording);

    /**
      * Set the state of the follow button.
      */
    void onFollowEnabled(bool follow);

    /**
      * Set the state of the play button.
      */
    void onPlayEnabled(bool play);

private:
    Ui::MainWindow* ui;

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

    QString projectPath;

    const static QString TITLE;

    /**
      * Sets icons/texts for the buttons and makes necessary connects.
      */
    void setUpButtonBars();

    /**
      * Adds actions to menu
      */
    void setUpMenu();

    void setTitle(QString pName);

    /**
      * Connects signals between ViewManager and MainWindow and adds the viewManager to the ui.
      */
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
      * @return The filename of the file to save in.
      */
    QString save(bool saveAs, qint64 begin = -1, qint64 end = -1);


    /**
      * Checks whether viewManager has unsaved changes and shows a dialog according to it.
      * In case there are unsaved changes the user has the chance to save/discard/abort.
      * @return The dialogs result value.
      */
    int checkForUnsavedChanges();

    /**
      * Checks whether there is a running record.
      * In case there is running record the user has the chance to save/discard/abort.
      * @return The dialogs result value.
      */
    int checkForActiveRecord();

    /**
      * Shows a dialog where the user can choose the file to save in. If the user has previously
      * saved this project and 'saveAs' is false, this function does not show any dialog but returns
      * the path to the current config file.
      * In case 'saveAs' is true the user will be prompted to specify a new file.
      * @return Path to the config file.
      */
    QString getSaveFileName(bool saveAs);

    /**
      * Propagates the save-call to the viewManager and writes into the config file specified with
      * 'path'.
      * @param pName The QVariantMap in which the necessary data needs to be stored (propagated)
      * @param path Path to a file in which will be written.
      * @return true If write was successful, false otherwise.
      */
    bool writeProjectSettings(QVariantMap config, QString path);

signals:
    /**
      * This signal is emitted when the user presses the "pin to new data" button.
      */
    void follow();

    /**
      * This signal is emitted when the user presses the record button.
      */
    void record();

    /**
      * This signal is emitted when the user presses the play button.
      */
    void play();
};

#endif // MAINWINDOW_H
