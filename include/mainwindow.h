#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dataprovider.h"
#include "networkadapter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gui/presentationarea.h"
#include "gui/trackscene.h"

#include <QAction>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QWidget>
#include <QMenu>
#include <QAction>


class PresentationArea;
class HorizontalScrollBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent(QResizeEvent *event);

protected:
    void closeEvent(QCloseEvent *ce);
    
private slots:
    void onImportAction();
    void onExportAction();
    void onSave();
    void onSaveAs();
    void onLoad();
    void onNew();
    void onVerticalScroll();
    void onExportRange(qint64 begin, qint64 end);

private:
    QVBoxLayout *trackLayout;

    // Buttons and layout for toolbar:
    QWidget toolBarWidget;
    QVBoxLayout layout;
    QPushButton addTrackButton;
    QPushButton importButton;
    QPushButton exportButton;
    QPushButton playButton;    

    // Spacer item to center the play button (in the bottomButtonBar)
    QSpacerItem *spacerLeft;
    QSpacerItem *spacerRight;

    // Area for the tracks
    PresentationArea *pa;

    // Menu
    QMenu menu;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *loadAction;
    QAction *newAction;
    QAction *quitAction;

    DataProvider *dataProvider;

    NetworkAdapter networkAdapter;

    Ui::MainWindow ui;

    TrackScene *trackScene;

    HorizontalScrollBar *hScrollBar;

    QString projectName, projectPath;

    const static QString TITLE;

    void setUpButtonBars();

    void setUpMenu();

    void setTitle(QString pName);

    /**
      * Deletes and sets new objects for: pa, dataProvider trackScene.
      * Reconnects necessary signals.
      */
    void setUpView();

    /**
      * Saves the current projects configuration (position of cursor, view range...).
      * @param saveAs If this flag is set to true, the user will be asked for a new projectName
      *               'save as'-semantics.
      *               If this flag is false the user will be asked for a projectName only if this
      *               project hasn't been saved before.
      */
    void save(bool saveAs);

    int checkForUnsavedChanges();

signals:
    void verticalScroll(QRectF visibleRectangle);
    void changedWindowSize(QSize size);

};

#endif // MAINWINDOW_H
