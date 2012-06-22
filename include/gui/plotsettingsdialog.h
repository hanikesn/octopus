#ifndef PLOTSETTINGSDIALOG_H
#define PLOTSETTINGSDIALOG_H

#include "abstractdataseries.h"
#include "plotsettings.h"
#include "ui_plotsettingsdialog.h"

#include <QDialog>

namespace Ui {
class PlotSettingsDialog;
}

class PlotSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlotSettingsDialog(const QList<AbstractDataSeries*> dataSeries,
                                bool showScalingOption,
                                bool offsetsEditable,
                                QWidget *parent);
    ~PlotSettingsDialog();
    
    /**
     * @return A plot settings object containing the settings chosen by the user.
     *      An empty plot settings object if the dialog was canceled.
     */
    static PlotSettings getSettings(const QList<AbstractDataSeries*> dataSeries,
//                                    QMap<QString, PlotSettings::ScaleType> currentScaleTypes = QMap<QString, PlotSettings::ScaleType>(),
                                    bool showScalingOption = false,
                                    bool offsetsEditable = true,
                                    bool changeDefaultScaleTypes = true,
                                    QWidget *parent = 0);

private:
    Ui::PlotSettingsDialog *ui;

    QMap<QString, AbstractDataSeries*> dataSeriesMap;

    void setupSourceTable(const QList<AbstractDataSeries*> dataSeries, bool offsetsEditable);
    PlotSettings getResult(bool changeDefaultScaleTypes);

private slots:
    void onSameScaleStateChanged(int state);
};

#endif // PLOTSETTINGSDIALOG_H
