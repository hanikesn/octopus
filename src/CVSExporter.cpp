#include "CVSExporter.h"
#include <QString>
#include <QObject>
#include "dataprovider.h"
#include "qtextstream.h"
#include "DBUtil.h"

QString CVSExporter::getFileType()
{
    return QObject::tr("CVC (*.cvs)");
}

void CVSExporter::write(QIODevice &file, const DataProvider &dp, const QStringList &dataSeries, qint64 begin, qint64 end)
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
