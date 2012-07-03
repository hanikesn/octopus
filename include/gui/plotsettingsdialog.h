#ifndef PLOTSETTINGSDIALOG_H
#define PLOTSETTINGSDIALOG_H

#include "abstractdataseries.h"
#include "plotsettings.h"

#include <QDialog>

namespace Ui {
class PlotSettingsDialog;
}

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
    PlotSettings getResult();

private slots:
    void onSameScaleStateChanged(int state);
};

#endif // PLOTSETTINGSDIALOG_H
