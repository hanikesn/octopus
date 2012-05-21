#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QAction>
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

    // Actions for toolbar
    QAction *addTrackAction;
    QAction *importAction;
    QAction *exportAction;
    QAction *playAction;

    // Area for the tracks
    PresentationArea *pa;

    Ui::MainWindow ui;

    void setUpToolBar();

signals:
    void visibleRangeChanged(int, int, int);

};

#endif // MAINWINDOW_H
