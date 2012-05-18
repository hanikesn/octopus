#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QWidget>

class ExampleTrack;
class QVBoxLayout;

class PresentationArea : public QWidget
{
    Q_OBJECT
public:
    explicit PresentationArea(QWidget *parent = 0);
    ~PresentationArea();

    void addTrack(ExampleTrack *et);
    
signals:
    
public slots:

private:
    QVBoxLayout *trackLayout;
    
};

#endif // PRESENTATIONAREA_H
