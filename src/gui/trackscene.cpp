#include "trackscene.h"
#include <QDebug>

TrackScene::TrackScene(QObject *parent) :
    QGraphicsScene(parent)
{
}


void TrackScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    qDebug() << "drawBackground";
}
