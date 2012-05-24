#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QObject>

#include "presentationitem.h"

class Track;
class QVBoxLayout;
class QGraphicsScene;

class PresentationArea : public QObject
{
    Q_OBJECT
public:
    explicit PresentationArea(QGraphicsScene *scene);
    ~PresentationArea();

signals:
    
public slots:
    void onAddTrack();
    void onDelete(Track *t);
    void onRangeChanged(qint64 begin, qint64 end);

private:    

    PresentationItem *pi;

    QList<Track*> tracks;    
};

#endif // PRESENTATIONAREA_H
