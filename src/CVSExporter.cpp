#include "CVSExporter.h"
#include <QString>
#include <QObject>

QString CVSExporter::getFileType()
{
    return QObject::tr("CVC (*.cvs)");
}

void CVSExporter::write(QIODevice &file, const DataProvider &dp, const QStringList &DataSeries, qint64 begin, qint64 end)
{

}
