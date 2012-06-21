#ifndef PLOTSETTINGSDIALOG_H
#define PLOTSETTINGSDIALOG_H

#include "ui_plotsettingsdialog.h"

#include <QDialog>

class PlotSettings;

namespace Ui {
class PlotSettingsDialog;
}

class PlotSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlotSettingsDialog(const QStringList &dataSeriesNames,
                                bool showScalingOption,
                                bool offsetsEditable,
                                QWidget *parent);
    ~PlotSettingsDialog();
    
    static PlotSettings getSettings(const QStringList &dataSeriesNames,
                                    bool showScalingOption = false,
                                    bool offsetsEditable = true,
                                    QWidget *parent = 0);

private:
    Ui::PlotSettingsDialog *ui;

    void setupSourceTable(const QStringList &dataSeriesNames, bool offsetsEditable);

private slots:
    void onSameScaleStateChanged(int state);
};

#endif // PLOTSETTINGSDIALOG_H
