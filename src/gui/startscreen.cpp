#include "gui/startscreen.h"
#include "ui_startscreen.h"

#include <QSpinBox>

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent),
    type(CANCEL)
{
    ui = new Ui::StartScreen;
    ui->setupUi(this);
}

StartScreen::~StartScreen()
{
    delete ui;
}

StartScreen::Type StartScreen::showScreen()
{
    exec();
    return type;
}

void StartScreen::on_newButton_clicked()
{
    type = NEW;
    close();
}

void StartScreen::on_loadButton_clicked()
{
    type = LOAD;
    close();
}

