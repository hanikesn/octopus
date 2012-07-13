#ifndef PLOTSETTINGSDIALOG_H
#define PLOTSETTINGSDIALOG_H

#include "common/plotsettings.h"
#include "datamodel/abstractdataseries.h"

#include <QDialog>

namespace Ui {
class PlotSettingsDialog;
}

/*
  The plot settings dialog shows the user the current settings for the plot and allows him to change them.
  Which settings can be edited depends on the parameters passed to the dialog by the caller.
*/
class PlotSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlotSettingsDialog(const QStringList &dataSeriesNames,
                                const PlotSettings &preset,
                                bool showScalingOption,
                                bool offsetsEditable,
                                QWidget *parent);
    ~PlotSettingsDialog();
    
    /**
     * @return A plot settings object containing the settings chosen by the user.
     *      An empty plot settings object if the dialog was canceled.
     */
    static PlotSettings getSettings(const QStringList &dataSeriesNames,
                                    const PlotSettings &preset,
                                    bool showScalingOption = false,
                                    bool offsetsEditable = true,
                                    QWidget *parent = 0);

private:
    Ui::PlotSettingsDialog *ui;

    void setupSourceTable(const QStringList &dataSeriesNames, const PlotSettings &preset, bool offsetsEditable);

    /**
     * @return A plot settings object containing the settings chosen by the user.
     *      An empty plot settings object if the dialog was canceled.
     */
    PlotSettings getResult();

private slots:
    /**
     * Adjusts the dialog's appearance according to the state of the same scale option.
     */
    void onSameScaleStateChanged(int state);
};

#endif // PLOTSETTINGSDIALOG_H
