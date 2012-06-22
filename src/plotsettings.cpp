#include "plotsettings.h"

// This list needs to be kept up to date with the respective enum in the header file!
const QStringList PlotSettings::scaleTypeNames = QStringList() << "lin" << "log";

PlotSettings::PlotSettings()
{
}

bool PlotSettings::isEmpty()
{
    return /*offsets.isEmpty() &&*/ scaleTypes.isEmpty();
}

void PlotSettings::setScaleType(const QString &fullDataSeriesName, ScaleType scaleType)
{
    scaleTypes.insert(fullDataSeriesName, scaleType);
}

PlotSettings::ScaleType PlotSettings::scaleType(const QString &fullDataSeriesName) const
{
    return scaleTypes.value(fullDataSeriesName);
}
