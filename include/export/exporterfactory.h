#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include <qglobal.h>
#include <memory>
#include <vector>

#include "export/exporter.h"

class QStringList;

/**
 * A factory for Exporter classes
 */
class ExporterFactory
{
    Q_DISABLE_COPY(ExporterFactory)
public:
    ExporterFactory() {}

    void addExporter(std::unique_ptr<Exporter> exporter);

    /**
     * You can use the StringList as value for QFileDialog::setNameFilters
     */
    QStringList names();

    Exporter& getExporter(QString const& name);
private:

    std::vector<std::unique_ptr<Exporter> > factories;
};

#endif // EXPORTERFACTORY_H
