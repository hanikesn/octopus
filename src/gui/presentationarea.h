#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>

class Track;
class QVBoxLayout;

class PresentationArea : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit PresentationArea(QGraphicsScene *parent = 0);
    ~PresentationArea();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHeight(qreal height);
    void setWidth(qreal width);

signals:
    
public slots:
    void onAddTrack();
    void onDelete(Track *t);
    void onRangeChanged(qint64 begin, qint64 end);

private:
    qreal height;
    qreal width;

    QGraphicsScene *parent;

    QList<Track*> tracks;    
};

#endif // PRESENTATIONAREA_H
