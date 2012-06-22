#include "gui/plotsettingsdialog.h"

#include "plotsettings.h"

#include <QComboBox>
#include <QDebug>
#include <QRadioButton>
#include <QSpinBox>
#include <limits>

static const int SOURCENAMECOL = 0;
static const int OFFSETCOL = 1;
static const int SCALECOL = 2;
static const int COLCOUNT = 3;

PlotSettingsDialog::PlotSettingsDialog(const QList<AbstractDataSeries*> dataSeries,
                                       bool showScalingOption,
                                       bool offsetsEditable,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotSettingsDialog)
{
    ui->setupUi(this);

    setupSourceTable(dataSeries, offsetsEditable);
    ui->sameScaleOption->setVisible(showScalingOption);
    ui->sourceTable->setColumnHidden(SCALECOL, showScalingOption);
    ui->sameScaleOption->setChecked(showScalingOption);
    ui->linChoice->setChecked(showScalingOption);

    connect(ui->sameScaleOption, SIGNAL(stateChanged(int)), this, SLOT(onSameScaleStateChanged(int)));
}

PlotSettingsDialog::~PlotSettingsDialog()
{
    delete ui;
}

void PlotSettingsDialog::setupSourceTable(const QList<AbstractDataSeries*> dataSeries, bool offsetsEditable)
{
    ui->sourceTable->setColumnCount(COLCOUNT);

    ui->sourceTable->setHorizontalHeaderItem(SOURCENAMECOL, new QTableWidgetItem(tr("Data Series Name")));
    ui->sourceTable->setHorizontalHeaderItem(OFFSETCOL, new QTableWidgetItem(tr("Offset")));
    ui->sourceTable->setHorizontalHeaderItem(SCALECOL, new QTableWidgetItem(tr("Scale Type")));

    QHeaderView *hHeader = ui->sourceTable->horizontalHeader();
    hHeader->setDefaultAlignment(Qt::AlignLeft);
    hHeader->setResizeMode(SOURCENAMECOL, QHeaderView::Stretch);
    hHeader->setResizeMode(OFFSETCOL, QHeaderView::ResizeToContents);
    hHeader->setResizeMode(SCALECOL, QHeaderView::ResizeToContents);

    QHeaderView *vHeader = ui->sourceTable->verticalHeader();
    vHeader->setVisible(false);

    foreach (AbstractDataSeries *series, dataSeries) {
        int row = ui->sourceTable->rowCount();
        ui->sourceTable->setRowCount(row + 1);

        QTableWidgetItem *sourceItem = new QTableWidgetItem(series->fullName());
        // the item showing the data series name should not be editable
        sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable);
        ui->sourceTable->setItem(row, SOURCENAMECOL, sourceItem);
        dataSeriesMap.insert(series->fullName(), series);

        QSpinBox *offsetSpinner = new QSpinBox(ui->sourceTable);
        offsetSpinner->setValue(series->offset);
        offsetSpinner->setMaximum(std::numeric_limits<int>::max());
        offsetSpinner->setEnabled(offsetsEditable);
        ui->sourceTable->setCellWidget(row, OFFSETCOL, offsetSpinner);

        QComboBox *scaleCombo = new QComboBox();
        scaleCombo->addItems(PlotSettings::scaleTypeNames);
        scaleCombo->setCurrentIndex(series->defaultScaleType);
        ui->sourceTable->setCellWidget(row, SCALECOL, scaleCombo);
    }
}

PlotSettings PlotSettingsDialog::getSettings(const QList<AbstractDataSeries*> dataSeries,
//                                             QMap<QString, PlotSettings::ScaleType> currentScaleTypes,
                                             bool showScalingOption,
                                             bool offsetsEditable,
                                             bool changeDefaultScaleTypes,
                                             QWidget *parent)
{
    PlotSettingsDialog *d = new PlotSettingsDialog(dataSeries, showScalingOption, offsetsEditable, parent);
    d->exec();

    return d->getResult(changeDefaultScaleTypes);
}

PlotSettings PlotSettingsDialog::getResult(bool changeDefaultScaleTypes)
{
    if (result() == QDialog::Rejected) {
        return PlotSettings();
    }

    PlotSettings settings;
    for (int row = 0; row < ui->sourceTable->rowCount(); row++) {
        QString sourceName = ui->sourceTable->item(row, SOURCENAMECOL)->text();
        AbstractDataSeries *series = dataSeriesMap.value(sourceName);

        if (series) {
            QSpinBox *offsetSpinner = qobject_cast<QSpinBox*>(ui->sourceTable->cellWidget(row, OFFSETCOL));
            series->offset = offsetSpinner->value();

            QComboBox *scaleCombo = qobject_cast<QComboBox*>(ui->sourceTable->cellWidget(row, SCALECOL));
            if (changeDefaultScaleTypes) {
                series->defaultScaleType = (PlotSettings::ScaleType) scaleCombo->currentIndex();
            }

            if (ui->sameScaleOption->checkState() == Qt::Unchecked) {
                settings.setScaleType(sourceName, (PlotSettings::ScaleType) scaleCombo->currentIndex());
            } else {
                if (ui->logChoice->isChecked()) {
                    settings.setScaleType(sourceName, PlotSettings::LOGSCALE);
                } else {
                    settings.setScaleType(sourceName, PlotSettings::LINSCALE);
                }
            }
        }
    }

    return settings;
}

void PlotSettingsDialog::onSameScaleStateChanged(int state)
{
    foreach (QAbstractButton *b, ui->scaleChoicesGroup->buttons()) {
        b->setVisible(state == Qt::Checked);
    }
    // show if unchecked (state == 0), hide otherwise
    ui->sourceTable->setColumnHidden(SCALECOL, state);
}
