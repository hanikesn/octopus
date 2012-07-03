#include "gui/plotsettingsdialog.h"

#include "plotsettings.h"

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QSettings>
#include <limits>

static const int SOURCENAMECOL = 0;
static const int OFFSETCOL = 1;
static const int SCALECOL = 2;
static const int COLCOUNT = 3;
static const int OFFSETDIVISOR = 1000;

PlotSettingsDialog::PlotSettingsDialog(const QStringList &dataSeriesNames,
                                       const PlotSettings &preset,
                                       bool showScalingOption,
                                       bool offsetsEditable,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotSettingsDialog)
{
    ui->setupUi(this);

    setupSourceTable(dataSeriesNames, preset, offsetsEditable);
    ui->sameScaleOption->setVisible(showScalingOption);
    ui->sameScaleOption->setChecked(showScalingOption && preset.scalingMode == PlotSettings::NOSCALING);

    switch (preset.plotScaleType) {
    case PlotSettings::LINSCALE:
        ui->linChoice->setChecked(true);
        break;
    case PlotSettings::LOGSCALE:
        ui->logChoice->setChecked(true);
        break;
    default:
        // if it's none of the above, we have no button for it :)
        break;
    }

    onSameScaleStateChanged(ui->sameScaleOption->checkState());

    connect(ui->sameScaleOption, SIGNAL(stateChanged(int)), this, SLOT(onSameScaleStateChanged(int)));
}

PlotSettingsDialog::~PlotSettingsDialog()
{
    delete ui;
}

void PlotSettingsDialog::setupSourceTable(const QStringList &dataSeriesNames, const PlotSettings &preset, bool offsetsEditable)
{
    ui->sourceTable->setColumnCount(COLCOUNT);

    QTableWidgetItem *sourceNameColHeader = new QTableWidgetItem(tr("Data Series Name"));
    sourceNameColHeader->setToolTip(tr("The name of the data series."));
    ui->sourceTable->setHorizontalHeaderItem(SOURCENAMECOL, sourceNameColHeader);

    QTableWidgetItem *offsetColHeader = new QTableWidgetItem(tr("Offset"));
    offsetColHeader->setToolTip(tr("The offset in milliseconds.\n\n"
                                   "This value is added to each timestamp\n"
                                   "of the respective data series.\n"
                                   "You can use this for example to adjust for\n"
                                   "time lags in the transmission of signals."));
    ui->sourceTable->setHorizontalHeaderItem(OFFSETCOL, offsetColHeader);

    QTableWidgetItem *scaleColHeader = new QTableWidgetItem(tr("Scale Type"));
    if (offsetsEditable) {
        scaleColHeader->setToolTip(tr("The <i>default</i> type of scale to be used for the data series."));
    } else {
        scaleColHeader->setToolTip(tr("The type of scale to be used for the data series."));
    }
    ui->sourceTable->setHorizontalHeaderItem(SCALECOL, scaleColHeader);

    QHeaderView *hHeader = ui->sourceTable->horizontalHeader();
    hHeader->setDefaultAlignment(Qt::AlignLeft);
    hHeader->setResizeMode(SOURCENAMECOL, QHeaderView::Stretch);
    hHeader->setResizeMode(OFFSETCOL, QHeaderView::ResizeToContents);
    hHeader->setResizeMode(SCALECOL, QHeaderView::ResizeToContents);
    hHeader->setMinimumSectionSize(120);

    QHeaderView *vHeader = ui->sourceTable->verticalHeader();
    vHeader->setVisible(false);

    foreach (QString sourceName, dataSeriesNames) {
        int row = ui->sourceTable->rowCount();
        ui->sourceTable->setRowCount(row + 1);

        QTableWidgetItem *sourceItem = new QTableWidgetItem(sourceName);
        // the item showing the data series name should not be editable
        sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable);
        ui->sourceTable->setItem(row, SOURCENAMECOL, sourceItem);

        QDoubleSpinBox *offsetSpinner = new QDoubleSpinBox(ui->sourceTable);
        offsetSpinner->setAlignment(Qt::AlignRight);
        // helps to set the min/max before you set the value ;)
        offsetSpinner->setMinimum(0.0);
        offsetSpinner->setMaximum(std::numeric_limits<double>::max());
        offsetSpinner->setDecimals(qRound(qLn(OFFSETDIVISOR)/qLn(10)));
        offsetSpinner->setValue(preset.offset(sourceName)/OFFSETDIVISOR);
        offsetSpinner->setEnabled(offsetsEditable);
        ui->sourceTable->setCellWidget(row, OFFSETCOL, offsetSpinner);

        if (preset.scaleType(sourceName) == PlotSettings::NOT_SCALABLE) {
            QTableWidgetItem *scaleItem = new QTableWidgetItem(tr("n/a"));
            scaleItem->setFlags(scaleItem->flags() & ~Qt::ItemIsEditable);
            ui->sourceTable->setItem(row, SCALECOL, scaleItem);
        } else {
            QComboBox *scaleCombo = new QComboBox();
            scaleCombo->addItems(PlotSettings::scaleTypeNames);
            scaleCombo->setCurrentIndex(preset.scaleType(sourceName));
            ui->sourceTable->setCellWidget(row, SCALECOL, scaleCombo);
        }
    }
}

PlotSettings PlotSettingsDialog::getSettings(const QStringList &dataSeriesNames,
                                             const PlotSettings &preset,
                                             bool showScalingOption,
                                             bool offsetsEditable,
                                             QWidget *parent)
{
    PlotSettingsDialog *d = new PlotSettingsDialog(dataSeriesNames, preset, showScalingOption, offsetsEditable, parent);
    d->exec();

    return d->getResult();
}

PlotSettings PlotSettingsDialog::getResult()
{
    if (result() == QDialog::Rejected) {
        return PlotSettings();
    }

    PlotSettings settings;
    if (ui->sameScaleOption->isChecked()) {
        settings.scalingMode = PlotSettings::NOSCALING;
        if (ui->logChoice->isChecked()) {
            settings.plotScaleType = PlotSettings::LOGSCALE;
        } else {
            settings.plotScaleType = PlotSettings::LINSCALE;
        }
    } else {
        settings.scalingMode = PlotSettings::MINMAXSCALING;
        // keep plotScaleType valid
        settings.plotScaleType = PlotSettings::NOT_SCALABLE;
    }

    for (int row = 0; row < ui->sourceTable->rowCount(); row++) {
        QString sourceName = ui->sourceTable->item(row, SOURCENAMECOL)->text();

        QDoubleSpinBox *offsetSpinner = qobject_cast<QDoubleSpinBox*>(ui->sourceTable->cellWidget(row, OFFSETCOL));
        if (offsetSpinner) {
            settings.setOffset(sourceName, offsetSpinner->value()*OFFSETDIVISOR);
        }

        QComboBox *scaleCombo = qobject_cast<QComboBox*>(ui->sourceTable->cellWidget(row, SCALECOL));
        if (scaleCombo) {
            if (ui->sameScaleOption->checkState() == Qt::Unchecked) {
                settings.setScaleType(sourceName, (PlotSettings::ScaleType) scaleCombo->currentIndex());
            } else {
                if (ui->logChoice->isChecked()) {
                    settings.setScaleType(sourceName, PlotSettings::LOGSCALE);
                } else {
                    settings.setScaleType(sourceName, PlotSettings::LINSCALE);
                }
            }
        } else {
            settings.setScaleType(sourceName, PlotSettings::NOT_SCALABLE);
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
