#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mainwindow.h>
#include "ui_mainwindow.h"
#include "gui/presentationarea.h"
#include "gui/trackscene.h"

#include <QAction>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QSpacerItem>
#include <QWidget>


class PresentationArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
    
private slots:
    void onImportAction();
    void onExportAction();
    void onPlayAction();    
    void onVerticalScroll();

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

    Ui::MainWindow ui;

    TrackScene trackScene;

    void setUpButtonBars();

signals:    
    void changedRange(qint64 begin, qint64 end);
    void verticalScroll(QRectF visibleRectangle);

};

#endif // MAINWINDOW_H
