#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include <QScrollArea>
#include <QSize>
#include <QRectF>

#include "serializable.h"

class PresentationItem;
class DataProvider;
class Track;
class QVBoxLayout;
class QGraphicsScene;
class QGraphicsWidget;
class Cursor;
class QScrollBar;
class TimeLine;
class TimeManager;
class Cursor;
class Selection;

class MouseEventHandler
{
public:
    virtual ~MouseEventHandler() {}
    virtual void mousePressEvent(QMouseEvent *event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void mouseMoveEvent(QMouseEvent *event) = 0;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) = 0;
};

class PresentationArea : public QScrollArea, public Serializable
{
    Q_OBJECT
public:
    explicit PresentationArea(const DataProvider &dataProvider,
                              QScrollBar *hScrollBar, QWidget *parent = 0);
    ~PresentationArea();

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

    bool hasUnsavedChanges(){return unsavedChanges;}
    void setUnsavedChanges(bool uc);

    bool isRecording() {return recording;}

signals:
    void verticalScroll(QRectF visibleRectangle);
    void exportRange(qint64 begin, qint64 end);
    void play();
    void zoomIn();
    void zoomOut();
    void saveProject(qint64 start, qint64 end);

    void changedViewHeight(int h);
    void changedViewWidth(int w);
public slots:
    void onAddTrack();
    void onDelete(Track *t);    

    void onPlay();

    void onRecord();

private slots:
    /**
      * Is called when the signal rangeChange(qint64, qint64) is emitted.
      * Updates the tracks to the new range.
      * @param begin Begin of the range.
      * @param end End of the range (usually 30s more than begin)
      */
    void onRangeChanged(qint64 begin, qint64 end);

    void updatePlotMargins();

protected:
    bool eventFilter(QObject *, QEvent *);

private:    
    const DataProvider &dataProvider;
    QList<Track*> tracks;

    bool unsavedChanges;
    bool recording;

    TimeLine *timeLine;
    Cursor *cursor;
    Selection *selection;

    TimeManager *timeManager;

    MouseEventHandler* viewportMouseHandler;

    qint64 recordStart, recordEnd;

    void addTrack(const QList<QString>& fullDataSeriesNames);
    void addTracks(const QList<QString>& fullDataSeriesNames);
    Track* add(const QList<QString>& fullDataSeriesNames);

    int showRecordDialog();
};

#endif // PRESENTATIONAREA_H
