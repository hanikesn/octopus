#include "common/plotsettings.h"

// This list needs to be kept up to date with the respective enum in the header file!
const QStringList PlotSettings::scaleTypeNames = QStringList() << "lin" << "log";

PlotSettings::PlotSettings() :
    scalingMode(NOSCALING),
    plotScaleType(LINSCALE)
{
}

bool PlotSettings::isEmpty()
{
    return offsets.isEmpty() && scaleTypes.isEmpty();
}

void PlotSettings::setOffset(const QString &fullDataSeriesName, int microSecs)
{
    offsets.insert(fullDataSeriesName, microSecs);
}

int PlotSettings::offset(const QString &fullDataSeriesName) const
{
    if (offsets.contains(fullDataSeriesName)) {
        return offsets.value(fullDataSeriesName);
    } else {
        return 0;
    }
}

void PlotSettings::setScaleType(const QString &fullDataSeriesName, ScaleType scaleType)
{
    scaleTypes.insert(fullDataSeriesName, scaleType);
}

PlotSettings::ScaleType PlotSettings::scaleType(const QString &fullDataSeriesName) const
{
    if (scaleTypes.contains(fullDataSeriesName)) {
        return scaleTypes.value(fullDataSeriesName);
    } else {
        return NOT_SCALABLE;
    }
}

PlotSettings::ScaleType PlotSettings::toScaleType(QCPAxis::ScaleType scaleType)
{
    switch (scaleType) {
    case QCPAxis::stLinear:
        return LINSCALE;
        break;
    case QCPAxis::stLogarithmic:
        return LOGSCALE;
        break;
    default:
        return NOT_SCALABLE;
        break;
    }
}
