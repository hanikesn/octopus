#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <mainwindow.h>
#include"ui_mainwindow.h"



class PresentationArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void onAddTrackAction();
    void onImportAction();
    void onExportAction();
    void onPlayAction();

    void on_horizontalScrollBar_rangeChanged(int min, int max);

    void on_horizontalScrollBar_sliderMoved(int position);

private:
    QVBoxLayout *trackLayout;

    // Buttons and layout for toolbar:
    QWidget toolBarWidget;
    QHBoxLayout layout;
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

    void setUpButtonBars();

signals:
    void visibleRangeChanged(int, int, int);

};

#endif // MAINWINDOW_H
