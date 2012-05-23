#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>

class Track;
class QVBoxLayout;

class PresentationArea : public QGraphicsItem
{
    // QGraphicsItem does not inherit from QObject --> no Q_OBJECT Macro possible!

public:
    explicit PresentationArea(QGraphicsScene *parent = 0);
    ~PresentationArea();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void addTrack(Track *t);

    void setHeight(qreal height);
    void setWidth(qreal width);

private:
    qreal height;
    qreal width;

    QGraphicsScene *parent;

    
};

#endif // PRESENTATIONAREA_H
