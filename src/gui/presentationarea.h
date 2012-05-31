#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QObject>

#include "presentationitem.h"

class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class Cursor;

class PresentationArea : public QObject
{
    Q_OBJECT
public:
    explicit PresentationArea(QGraphicsScene *scene);
    ~PresentationArea();

signals:
    void rangeChanged(qint64 begin, qint64 end);
    void changedWindowSize(QSize size);
    void verticalScroll(QRectF visibleRectangle);
    void exportRange(qint64 begin, qint64 end);

public slots:
    void onAddTrack();
    void onDelete(Track *t);
    void onRangeChanged(qint64 begin, qint64 end);    
    void onChangedWindowSize(QSize size);

private slots:
    void onSelection(qint64 begin, qint64 end);
    void onExportTriggered();

private:    
    PresentationItem *pi;    

    QList<Track*> tracks;    

    QSize currentViewSize;

    qint64 selectionBegin, selectionEnd;

    static const int ACTIONAREAOFFSET;
};

#endif // PRESENTATIONAREA_H
