#include "CSVExporter.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include "dataprovider.h"
#include "qtextstream.h"
#include "DBUtil.h"
#include "value.h"
#include "abstractdataseries.h"

QString CSVExporter::getFileType()
{
    return QObject::tr("CSV (*.csv)");
}

void CSVExporter::write(QIODevice &file, const DataProvider &dp, const QStringList &dataSeries, qint64 begin, qint64 end)
{
    QTextStream stream(&file);

    QMap<QString, qint64> offsetMap;

    foreach(QString const& series, dataSeries)
    {
        offsetMap.insert(series, dp.getDataSeries(series)->offset);
    }

    Sqlite::PreparedStatement stmt = dp.getDB().getData(dataSeries, begin, end);

    QString name;
    qint64 timestamp;
    Value val;

    for (auto it = stmt.execute(); it != Sqlite::DB::Done; ++it) {
        Sqlite::Row r = *it;
        r >> name >> timestamp >> val;
        if(offsetMap.contains(name)) {
            timestamp += offsetMap[name];
        }
        stream << name << ";" << timestamp << ";" << val.asString() << "\n";
    }
}
