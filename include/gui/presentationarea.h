#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QObject>

#include "dataprovider.h"
#include "presentationitem.h"
#include "serializable.h"

#include "boost/property_tree/ptree.hpp"

class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class Cursor;
class QScrollBar;

class PresentationArea : public QObject, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationArea(QGraphicsScene *scene, const DataProvider &dataProvider,
                              QScrollBar *hScrollBar);
    ~PresentationArea();

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

signals:
    void changedWindowSize(QSize size);
    void verticalScroll(QRectF visibleRectangle);
    void exportRange(qint64 begin, qint64 end);    

public slots:
    void onAddTrack();
    void onDelete(Track *t);

    /**
      * Resizes tracks to new view length.
      * Propagates event (PresentationItem resizes timeLine and cursor)
      * @param size Size of the new view (it's the size of the mainView not the window size)
      */
    void onChangedWindowSize(QSize size);    

private slots:
    /**
      * Transforms the position of the selection to timestamps and saves the it.
      * @param begin Begin of the selection
      * @param end End of the selection
      */
    void onSelection(qint64 begin, qint64 end);

    /**
      * Propagates export signal with the range of the current selection (if available)
      */
    void onExportTriggered();

    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted.
      * Updates the tracks to the new range.
      * @param begin Begin of the range.
      * @param end End of the range (usually 30s more than begin)
      */
    void onRangeChanged(qint64 begin, qint64 end);

private:    
    PresentationItem *pi;

    const DataProvider &dataProvider;
    QList<Track*> tracks;

    QSize currentViewSize;
    qint64 selectionBegin, selectionEnd;

    void addTracks(const QList<QString>& fullDataSeriesNames);
    void addTrack(const QList<QString>& fullDataSeriesNames);

    void add(Track *t);
};

#endif // PRESENTATIONAREA_H
