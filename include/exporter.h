#ifndef EXPORTER_H
#define EXPORTER_H

#include <qglobal.h>

class QString;
class QIODevice;
class QStringList;
class DataProvider;

class Exporter
{
    Q_DISABLE_COPY(Exporter)
public:
    Exporter() {}
    virtual ~Exporter() {}
    /**
     * @brief getFileType
     * @return a String to identify the Export in the save dialog
     */
    virtual QString getFileType() = 0;

    virtual QString getSuffix() = 0;

    virtual void write(QIODevice& file, DataProvider const& dp, QStringList const& DataSeries, qint64 begin, qint64 end) = 0;
};

#endif // EXPORTER_H
