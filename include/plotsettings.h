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
        LOGSCALE,    // logarithmic scale
        NOT_SCALABLE
    };

    static const QStringList scaleTypeNames;

    /**
     * @return True if the plot settings object contains no settings, false otherwise.
     */
    bool isEmpty();

    void setOffset(const QString &fullDataSeriesName, int microSecs);
    int offset(const QString &fullDataSeriesName) const;

    void setScaleType(const QString &fullDataSeriesName, ScaleType scaleType);
    ScaleType scaleType(const QString &fullDataSeriesName) const;

    ScaleType plotScaleType;
    ScalingMode scalingMode;

private:
    QMap<QString, int> offsets;
    QMap<QString, ScaleType> scaleTypes;
};

#endif // PLOTSETTINGS_H
