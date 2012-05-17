#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "gui/exampletrack.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget *widget = new QWidget();
    trackLayout = new QVBoxLayout();
    trackLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    widget->setLayout(trackLayout);
    ui->trackArea->setWidget(widget);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addTrackButton_clicked()
{
    ExampleTrack *et = new ExampleTrack();
    trackLayout->addWidget(et);

    et->scroll(200, 0);

}

