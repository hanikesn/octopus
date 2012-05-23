#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QWidget>

class Track;
class QVBoxLayout;

class PresentationArea : public QWidget
{
    Q_OBJECT
public:
    explicit PresentationArea(QWidget *parent = 0);
    ~PresentationArea();

    void addTrack(Track *t);
    
signals:
    
public slots:

private:
    QVBoxLayout *trackLayout;
    
};

#endif // PRESENTATIONAREA_H
