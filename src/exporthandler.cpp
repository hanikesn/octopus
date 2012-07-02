#include "exporthandler.h"

#include <sstream>
#include <QFileDialog>
#include <QMessageBox>

#include "CSVExporter.h"
#include "databaseadapter.h"
#include "gui/sourcedialog.h"
#include "dataprovider.h"

ExportHandler::ExportHandler(QObject *parent, DataProvider *dp) :
    QObject(parent),
    begin(-1),
    end(-1),
    dataProvider(dp)
{
    exporterFactory.addExporter(std::unique_ptr<Exporter>(new CSVExporter()));
}

ExportHandler::~ExportHandler()
{

}

void ExportHandler::onSelectionChanged(qint64 begin, qint64 end)
{
    this->begin = begin;
    this->end = end;
}

void ExportHandler::onExport()
{
    onExport(begin, end);
}

void ExportHandler::onExport(qint64 begin, qint64 end)
{
    if (begin == -1 && end == -1) { // export all data
        const DatabaseAdapter& da = dataProvider->getDB();
        da.getMinMaxTimeStamp(begin, end);
    }

    if (begin > end)
        std::swap(begin, end);

    QList<QStringList> res = SourceDialog::getSources(*dataProvider, tr("Export"), false, QStringList());

    if(res.isEmpty())
        return;

    QStringList sources = res.front();

    QFileDialog dialog(0, tr("Export"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    dialog.setNameFilters(exporterFactory.names());
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.isEmpty())
        return;

    Exporter& exporter = exporterFactory.getExporter(dialog.selectedNameFilter());

    if (!fileNames.first().endsWith(exporter.getSuffix()))
        fileNames.first().append(exporter.getSuffix());

    QFile file(fileNames.first());
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0,tr("Error"), tr("Could not save file."));
        return;
    }

    exporter.write(file, *dataProvider, sources, begin, end);
}
