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
     * @see QFileDialog::setNameFilter
     */
    virtual QString getFileType() = 0;

    /**
     * The suffix e.g.: "cvs"
     */
    virtual QString getSuffix() = 0;

    /**
     * @brief write the from dataSeries (if empty all data) in the range from begin to end provided by dp to the file
     */
    virtual void write(QIODevice& file, DataProvider const& dp, QStringList const& dataSeries, qint64 begin, qint64 end) = 0;
};

#endif // EXPORTER_H
