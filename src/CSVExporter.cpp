#include "CSVExporter.h"
#include <QString>
#include <QObject>
#include "dataprovider.h"
#include "qtextstream.h"
#include "DBUtil.h"
#include "value.h"

QString CSVExporter::getFileType()
{
    return QObject::tr("CSV (*.csv)");
}

void CSVExporter::write(QIODevice &file, const DataProvider &dp, const QStringList &dataSeries, qint64 begin, qint64 end)
{
    QTextStream stream(&file);

    Sqlite::PreparedStatement stmt = dp.getDB().getData(dataSeries, begin, end);

    QString name;
    qint64 timestamp;
    Value val;

    for (auto it = stmt.execute(); it != Sqlite::DB::Done; ++it) {
        Sqlite::Row r = *it;
        r >> name >> timestamp >> val;
        stream << name << ";" << timestamp << ";" << val.asString() << "\n";
    }
}