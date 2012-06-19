#include "common.h"
#include "exporterfactory.h"
#include "exporter.h"
#include <QStringList>

void ExporterFactory::addExporter(std::unique_ptr<Exporter> exporter)
{
    factories.push_back(std::move(exporter));
}

QStringList ExporterFactory::names()
{
    QStringList list;
    std::for_each(factories.begin(), factories.end(),
                  [&list](std::unique_ptr<Exporter> const& exp)
    {
        list.append(exp->getFileType());
    });
    return list;
}

Exporter& ExporterFactory::getExporter(const QString &name)
{
    auto i = std::find_if(factories.begin(), factories.end(),
                                 [&name](std::unique_ptr<Exporter> const& exp)
    {
        return exp->getFileType() == name;
    });

    return **i;
}
