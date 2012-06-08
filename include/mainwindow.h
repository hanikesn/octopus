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
    
private slots:
    void onImportAction();
    void onExportAction();
    void onPlayAction();    
    void onSave();
    void onLoad();
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
    QAction *loadAction;

    DataProvider dataProvider;
    NetworkAdapter networkAdapter;

    Ui::MainWindow ui;

    TrackScene trackScene;

    HorizontalScrollBar *hScrollBar;

    QString projectName, projectPath;

    void setUpButtonBars();

    void setUpMenu();

    void setTitle(QString pName);

signals:
    void verticalScroll(QRectF visibleRectangle);
    void changedWindowSize(QSize size);

};

#endif // MAINWINDOW_H
