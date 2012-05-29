#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QObject>

#include "presentationitem.h"

class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class TimeLine;
class Cursor;

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
    void onVerticalScroll(QRectF visibleRectangle);
    void onChangedWindowSize(QSize size);

private slots:
    void onCursorPosChanged(int pos);

private:    

    PresentationItem *pi;

    TimeLine *timeLine;

    QList<Track*> tracks;    

    QSize currentViewSize;

    Cursor *cursor;

    static const int ACTIONAREAOFFSET;
};

#endif // PRESENTATIONAREA_H
