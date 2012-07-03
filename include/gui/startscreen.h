#ifndef STARTSCREEN_H
#define STARTSCREEN_H

namespace Ui {
    class StartScreen;
}

#include <QDialog>

class StartScreen : public QDialog
{
    Q_OBJECT
public:
    explicit StartScreen(QWidget *parent);
    ~StartScreen();

    enum Type {CANCEL, LOAD, NEW};

    Type showScreen();

private:
    Ui::StartScreen* ui;
    Type type;

private slots:
    void on_newButton_clicked();
    void on_loadButton_clicked();    
};

#endif // STARTSCREEN_H

