#include "gui/startscreen.h"

#include <QSpinBox>
#include <QDebug>

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent),
    type(NEW)
{
    ui.setupUi(this);

    ui.newButton->setStyleSheet("border: 0; margin: 0px;");
    ui.loadButton->setStyleSheet("border: 0; margin: 0px;");

    ui.newButton->setFocusPolicy(Qt::ClickFocus);
    ui.loadButton->setFocusPolicy(Qt::ClickFocus);
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

