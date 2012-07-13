#ifndef SELECTION_H
#define SELECTION_H

#include <QPen>
#include <QWidget>

class QMenu;
class TimeManager;

/*
Marks the range the user has selected (via mouse) to export or zoom into.
*/
class Selection : public QWidget
{
    Q_OBJECT
public:
    explicit Selection(TimeManager *timeManager, QWidget *parent);

    /**
      * Hides the selection (setVisible(false)) and emits a signal, that the selection is no longer
      * valid.
      */
    void hide();

    /**
      * Shows the selection (setVisible(true)) and emits a signal with the current range.
      */
    void show();

public slots:
    /**
      * Sets the start(timestamp) and resizes the selection accordingly.
      * Emits a signal with the current range.
      * @param begin The new begin of the selection (timestamp)
      */
    void setSelectionBegin(qint64 begin);

    /**
      * Sets the end(timestamp) and resizes the selection accordingly.
      * Emits a signal with the current range.
      * @param end The new end of the selection (timestamp)
      */
    void setSelectionEnd(qint64 end);

    /**
      * Sets a fixed height 'h' for this selection.
      */
    void updateHeight(int h);

    /**
      * Moves and resizes the selection.
      */
    void onUpdate();

signals:
    /**
      * This signal is emitted when the user preses the export-item in the context menu.
      * @param begin Current begin of the selection
      * @param end Current end of the selection
      */
    void onExport(qint64 begin, qint64 end);

    /**
      * This signal is emitted if the range of the selection changes.
      * @param begin Current begin of the selection
      * @param end Current end of the selection
      */
    void selectionChanged(qint64 begin, qint64 end);

    /**
      * This signal is emitted when the user preses the 'zoom in' item in the context menu.
      * @param start Current start of the selection
      * @param end Current end of the selection
      */
    void zoomIn(qint64 start, qint64 end);

protected slots:
    /**
      * Emits the 'onExport(qint64, qint64)' signal.
      */
    void exportTriggered();

protected:
    /**
      * Override of the QWidget function.
      */
    void contextMenuEvent(QContextMenuEvent * event);

    /**
      * Override of the QWidget function. (Custom drawing)
      */
    void paintEvent(QPaintEvent *);

private:
    // Current begin of the selection
    qint64 begin;
    // Current begin of the selection
    qint64 end;

    qint64 left;
    qint64 right;


    // Border color of the selection
    QPen pen;
    // Background color of the selection
    QBrush brush;

    QMenu *menu;

    // Converts x-coordinates to timestamps
    TimeManager* timeManager;

private slots:
    /**
      * Emits the 'zoomIn(qint64, qint64)' signal.
      */
    void onZoomIn();
};

#endif // SELECTION_H
