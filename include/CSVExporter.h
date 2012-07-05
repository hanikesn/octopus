#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include "exporter.h"

/**
 * @brief Exports the data in standard CSV format
 */
class CSVExporter : public Exporter
{
public:
    virtual QString getFileType();

    virtual QString getSuffix();

    virtual void write(QIODevice& file, DataProvider const& dp, QStringList const& DataSeries, qint64 begin, qint64 end);
};

#endif // CSVEXPORTER_H
