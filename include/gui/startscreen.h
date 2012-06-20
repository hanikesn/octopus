#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "ui_startscreen.h"

#include <QDialog>

class StartScreen : public QDialog
{
    Q_OBJECT
public:
    explicit StartScreen(QWidget *parent);

    enum Type {LOAD, NEW};

    Type showScreen();

    void closeEvent(QCloseEvent *ce);

public slots:

private:
    Ui::StartScreen ui;
    Type type;

private slots:

    void on_newButton_clicked();
    void on_loadButton_clicked();
};

#endif // STARTSCREEN_H

