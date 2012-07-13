#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H

#include "mainview/tracks/qcustomplot.h"

#include <QMap>
#include <QString>
#include <QStringList>

/*
  A plot settings object can store settings for a plot, such as
  scaling mode and scale type, as well as individual settings for
  specific data series.
*/
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

    /**
     * Strings for the different scale types, e.g. for use in drop down menus.
     */
    static const QStringList scaleTypeNames;

    /**
     * The scaling mode for the plot. By default, this value is set to NOSCALING.
     */
    ScalingMode scalingMode;

    /**
     * The scale type to be applied to the y axis of the plot. By default, this value
     *      is set to LINSCALE.
     */
    ScaleType plotScaleType;

    /**
     * @return True if the plot settings object contains no settings, false otherwise.
     */
    bool isEmpty();

    /**
     * Stores the offset for the given data series in the settings.
     */
    void setOffset(const QString &fullDataSeriesName, int microSecs);

    /**
     * @return The offset for the given data series from the settings.
     *      0 if there are no settings for the given data series.
     */
    int offset(const QString &fullDataSeriesName) const;

    /**
     * Stores the scale type for the given data series in the settings.
     */
    void setScaleType(const QString &fullDataSeriesName, ScaleType scaleType);

    /**
     * @return The scale type for the given data series from the settings.
     *      NOT_SCALABLE if there are no settings for the given data series.
     */
    ScaleType scaleType(const QString &fullDataSeriesName) const;

    /**
     * Converts a QCPAxis::ScaleType to a PlotSettings::ScaleType.
     * @return The PlotSettings::ScaleType.
     */
    static ScaleType toScaleType(QCPAxis::ScaleType scaleType);

private:
    /**
     * A map of data series names and their respective offsets.
     */
    QMap<QString, int> offsets;

    /**
     * A map of data series names and their respective scale types.
     */
    QMap<QString, ScaleType> scaleTypes;
};

#endif // PLOTSETTINGS_H
