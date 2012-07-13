#include "main/startscreen.h"
#include "ui_startscreen.h"

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartScreen()),
    type(CANCEL)
{
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

