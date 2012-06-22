#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H

#include <QMap>
#include <QString>
#include <QStringList>

class PlotSettings
{
public:
    /**
     * Constructs an empty plot settings object.
     */
    PlotSettings();

    enum ScalingMode {
        NOSCALING,      // don't scale the data series' values
        MINMAXSCALING   // scale the data series' values to a range between 0 (min value) and 1 (max value)
    };

    // When changing this enum, also update the respective string list in the source file!
    enum ScaleType {
        LINSCALE,   // linear scale
        LOGSCALE    // logarithmic scale
    };

    static const QStringList scaleTypeNames;

    /**
     * @return True if the plot settings object contains no settings, false otherwise.
     */
    bool isEmpty();

    void setScaleType(const QString &fullDataSeriesName, ScaleType scaleType);
    ScaleType scaleType(const QString &fullDataSeriesName) const;

private:
    QMap<QString, ScaleType> scaleTypes;
};

#endif // PLOTSETTINGS_H
