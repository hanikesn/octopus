#include "main/mainwindow.h"
#include "main/startscreen.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StartScreen *s = new StartScreen(0);
    StartScreen::Type result = s->showScreen();
    if (result == StartScreen::CANCEL) {
        return 0;
    }

    delete s;

    MainWindow w(result);
    w.show();
    
    return a.exec();
}
