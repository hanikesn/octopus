#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include <qglobal.h>
#include <memory>
#include <vector>

class Exporter;
class QStringList;

class ExporterFactory
{
    Q_DISABLE_COPY(ExporterFactory)
public:

    void addExporter(std::unique_ptr<Exporter> exporter);

    QStringList names();

    Exporter& getExporter(QString const& name);
private:

    std::vector<std::unique_ptr<Exporter> > factories;
};

#endif // EXPORTERFACTORY_H
