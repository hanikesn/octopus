/*
  A track consists of a number of buttons and a plot, containing one graph per data series to be shown in this track.
*/

#ifndef Track_H
#define Track_H

#include "plotsettings.h"
#include "visitor.h"
#include "serializable.h"

#include <QWidget>

class Graph;
class DataProvider;
class AbstractDataSeries;

namespace Ui {
    class Track;
}

class Track : public QWidget, public DataSeriesVisitor, public Serializable
{
    Q_OBJECT
public:
    Track(const DataProvider &dataProvider, QWidget *parent = 0);
    Track(const DataProvider &dataProvider, const QString &fullDataSeriesName, QWidget *parent = 0);
    Track(const DataProvider &dataProvider, const QStringList &fullDataSeriesNames, QWidget *parent = 0);
    ~Track();

    /**
     * Visitor pattern.
     */
    void addGraph(const DoubleSeries &s);
    void addGraph(const StringSeries &s);

    /**
     * @return The distance between the left border of the track and the left vertical axis of the plot.
     */
    int getMarginLeft();

    /**
     * @return The distance between the right border of the track and the right vertical axis of the plot.
     */
    int getMarginRight();

    /**
     * Sets the distance between the left border of the plot and its left vertical axis.
     */
    void setPlotMarginLeft(int margin);

    void save(QVariantMap *qvm);
    void load(QVariantMap *qvm);

    int getOptPlotMarginLeft() const {return optPlotMarginLeft;}

signals:
    /**
     * Emitted when the optimal plot margins for this track have changed.
     */
    void optPlotMarginsChanged();

    /**
     * Emitted when this track is up for deletion.
     */
    void del(Track*);

public slots:
    /**
     * Sets the range for the plots x axis.
     */
    void setPlotRange(qint64 begin, qint64 end);

private:
    Ui::Track* ui;

    /**
     * The style sheet for the track's buttons.
     */
    static const QString ICON_AS_BUTTON;

    /**
     * The optimal left plot margin for the track.
     */
    int optPlotMarginLeft;

    const DataProvider &dataProvider;
    QList<Graph*> graphs;

    PlotSettings::ScalingMode currentScalingMode;

    /**
     * Convenience function for reuse in every constructor.
     */
    void init();

    /**
     * Sets up the track's buttons.
     */
    void setupButtons();

    /**
     * Sets the tool tip colors to the given colors.
     */
    QPalette setToolTipColors(QPalette palette, const QColor baseColor, const QColor textColor);

    /**
     * Sets up the track's plot.
     */
    void setupPlot();

    /**
     * Adds the given data series to the plot.
     */
    void addSource(const QString &fullDataSeriesName);

    /**
     * @return A list of the names of all data series shown in this track's plot.
     */
    QStringList getFullDataSeriesNames();

    /**
     * @return The current settings for this track's plot, including
     *      the settings for each individual data series.
     */
    PlotSettings currentSettings();

    /**
     * Updates the plot according to the given settings.
     */
    void update(PlotSettings settings);

private slots:
    /**
     * Initiates the deletion of this track by emitting the del signal.
     */
    void onDelete();

    /**
     * Shows the source dialog and updates the contents of the plot
     *      according to the user's choice of data series to be shown.
     */
    void onSources();

    /**
     * Shows the plot settings dialog and updates the appearance of the
     *      plot according to the user's choices.
     */
    void onPlotSettings();

    /**
     * Called upon reception of the plot's optMarginsRecalculated signal.
     *      Sets the top and bottom margins as suggested by the plot and
     *      emits the optPlotMarginsChanged signal, as the placement of
     *      the plot's left vertical axis needs to be the same as in the
     *      other tracks.
     */
    void onOptPlotMarginsRecalculated(int left, int /*right*/, int /*top*/, int /*bottom*/);
};

#endif // Track_H
