#ifndef PRESENTATIONAREA_H
#define PRESENTATIONAREA_H

#include "serializable.h"

#include <QScrollArea>

class QMouseEvent;
class PresentationItem;
class DataProvider;
class Track;
class Cursor;
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
                              TimeManager *timeManager, QWidget *parent = 0);
    ~PresentationArea();

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

    bool hasUnsavedChanges(){return unsavedChanges;}
    void setUnsavedChanges(bool uc);

signals:
    void exportRange(qint64 begin, qint64 end);

    void changedViewHeight(int h);
    void changedViewWidth(int w);

private slots:
    void onAddTrack();
    void onDelete(Track *t);

    void onPlotSettings();
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

    TimeLine *timeLine;
    Cursor *cursor;
    Selection *selection;

    TimeManager *timeManager;

    bool unsavedChanges;

    MouseEventHandler* viewportMouseHandler;

    void addTrack(const QList<QString>& fullDataSeriesNames);
    void addTracks(const QList<QString>& fullDataSeriesNames);
    Track* add(const QList<QString>& fullDataSeriesNames);
    int showRecordDialog();
};

#endif // PRESENTATIONAREA_H
