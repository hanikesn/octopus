#include "gui/plotsettingsdialog.h"

#include "plotsettings.h"

#include <QComboBox>
#include <QRadioButton>
#include <QSpinBox>
#include <limits>

static const int SOURCENAMECOL = 0;
static const int OFFSETCOL = 1;
static const int SCALECOL = 2;
static const int COLCOUNT = 3;

PlotSettingsDialog::PlotSettingsDialog(const QStringList &dataSeriesNames,
                                       bool showScalingOption,
                                       bool offsetsEditable,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotSettingsDialog)
{
    ui->setupUi(this);

    setupSourceTable(dataSeriesNames, offsetsEditable);
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

void PlotSettingsDialog::setupSourceTable(const QStringList &dataSeriesNames, bool offsetsEditable)
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

    foreach (QString name, dataSeriesNames) {
        int row = ui->sourceTable->rowCount();
        ui->sourceTable->setRowCount(row + 1);

        QTableWidgetItem *sourceItem = new QTableWidgetItem(name);
        // the item showing the data series name should not be editable
        sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable);
        ui->sourceTable->setItem(row, SOURCENAMECOL, sourceItem);

        QSpinBox *offsetSpinner = new QSpinBox(ui->sourceTable);
        offsetSpinner->setValue(0);
        offsetSpinner->setMaximum(std::numeric_limits<int>::max());
        offsetSpinner->setEnabled(offsetsEditable);
        ui->sourceTable->setCellWidget(row, OFFSETCOL, offsetSpinner);

        QComboBox *scaleCombo = new QComboBox();
        scaleCombo->addItems(QStringList() << "LOG" << "LIN");
        ui->sourceTable->setCellWidget(row, SCALECOL, scaleCombo);
    }
}

PlotSettings PlotSettingsDialog::getSettings(const QStringList &dataSeriesNames,
                                             bool showScalingOption,
                                             bool offsetsEditable,
                                             QWidget *parent)
{
    PlotSettingsDialog *d = new PlotSettingsDialog(dataSeriesNames, showScalingOption, offsetsEditable, parent);
    d->exec();

    // TODO(Steffi)
    return PlotSettings();
}

void PlotSettingsDialog::onSameScaleStateChanged(int state)
{
    foreach (QAbstractButton *b, ui->scaleChoicesGroup->buttons()) {
        b->setVisible(state == Qt::Checked);
    }
    // show if unchecked (state == 0), hide otherwise
    ui->sourceTable->setColumnHidden(SCALECOL, state);
}
