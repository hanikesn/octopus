#ifndef CVSEXPORTER_H
#define CVSEXPORTER_H

#include "exporter.h"

class CVSExporter : public Exporter
{
public:
    virtual QString getFileType();

    virtual void write(QIODevice& file, DataProvider const& dp, QStringList const& DataSeries, qint64 begin, qint64 end);
};

#endif // CVSEXPORTER_H
