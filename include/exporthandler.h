/*
Receives signals to export (either to export all data or just a specified range).
Shows a dialog to get the desired file to write to.
*/
#ifndef EXPORTHANDLER_H
#define EXPORTHANDLER_H

#include <QObject>

#include "exporterfactory.h"

class ExportHandler : public QObject
{
    Q_OBJECT
public:
    ExportHandler(QObject* parent, DataProvider *dp);
    ~ExportHandler();

private:
    qint64 begin, end;

    ExporterFactory exporterFactory;

    DataProvider *dataProvider;

private slots:
    /**
      * Keeps the the member variables 'begin'/'end' up to date with the current selection.
      * If both begin and end are -1 all of the data will be exported instead of only a range.
      * @param begin Begin of the current selection
      * @param end End of the current selection
      */
    void onSelectionChanged(qint64 begin, qint64 end);

    /**
      * Calls 'onExport(qint64, qint64) with 'begin' and 'end'.
      */
    void onExport();

    /**
      * Exports data with begin < timeStamp < end.
      * The user will be asked which of the data series he would like to be exported.
      * @param begin Begin of the range to be exported.
      * @param end End of the range to be exported.
      */
    void onExport(qint64 begin, qint64 end);

};

#endif
