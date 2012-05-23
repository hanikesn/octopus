#ifndef TRACKSCENE_H
#define TRACKSCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>

class TrackScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit TrackScene(QObject *parent = 0);

signals:

public slots:

protected:
    void drawBackground(QPainter *painter, const QRectF &rect);

};

#endif // TRACKSCENE_H
