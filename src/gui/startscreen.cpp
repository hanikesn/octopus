#include "gui/startscreen.h"

#include <QDebug>

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    setStyleSheet("background-color:#ffffff;");
    ui.loadButton->setFlat(true);
    ui.newButton->setFlat(true);
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
