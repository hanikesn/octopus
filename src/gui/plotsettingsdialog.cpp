#include "gui/plotsettingsdialog.h"

#include "plotsettings.h"

#include <QComboBox>
#include <QRadioButton>
#include <QSpinBox>
#include <limits>

PlotSettingsDialog::PlotSettingsDialog(const QStringList &dataSeriesNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotSettingsDialog)
{
    ui->setupUi(this);

    ui->sameScaleOption->setCheckState(Qt::Unchecked);
    foreach (QAbstractButton *b, ui->scaleChoicesGroup->buttons()) {
        b->setVisible(false);
    }

    setupSourceTable(dataSeriesNames);

    connect(ui->sameScaleOption, SIGNAL(stateChanged(int)), this, SLOT(onSameScaleStateChanged(int)));
}

PlotSettingsDialog::~PlotSettingsDialog()
{
    delete ui;
}

void PlotSettingsDialog::setupSourceTable(const QStringList &dataSeriesNames)
{
    int sourceNameCol = 0;
    int offsetCol = 1;
    int scaleCol = 2;
    ui->sourceTable->setColumnCount(3);

    ui->sourceTable->setHorizontalHeaderItem(sourceNameCol, new QTableWidgetItem(tr("Data Series Name")));
    ui->sourceTable->setHorizontalHeaderItem(offsetCol, new QTableWidgetItem(tr("Offset")));
    ui->sourceTable->setHorizontalHeaderItem(scaleCol, new QTableWidgetItem(tr("Scale Type")));

    QHeaderView *hHeader = ui->sourceTable->horizontalHeader();
    hHeader->setResizeMode(sourceNameCol, QHeaderView::Stretch);
    hHeader->setResizeMode(offsetCol, QHeaderView::ResizeToContents);
    hHeader->setResizeMode(scaleCol, QHeaderView::ResizeToContents);

    QHeaderView *vHeader = ui->sourceTable->verticalHeader();
    vHeader->setVisible(false);

    foreach (QString name, dataSeriesNames) {
        int row = ui->sourceTable->rowCount();
        ui->sourceTable->setRowCount(row + 1);

        QTableWidgetItem *sourceItem = new QTableWidgetItem(name);
        // the item showing the data series name should not be editable
        sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable);
        ui->sourceTable->setItem(row, sourceNameCol, sourceItem);

        QSpinBox *offsetSpinner = new QSpinBox(ui->sourceTable);
        offsetSpinner->setValue(0);
        offsetSpinner->setMaximum(std::numeric_limits<int>::max());
        ui->sourceTable->setCellWidget(row, offsetCol, offsetSpinner);

        QComboBox *scaleCombo = new QComboBox();
        scaleCombo->addItems(QStringList() << "LOG" << "LIN");
        ui->sourceTable->setCellWidget(row, scaleCol, scaleCombo);
    }
}

PlotSettings PlotSettingsDialog::getSettings(const QStringList &dataSeriesNames, QWidget *parent)
{
    PlotSettingsDialog *d = new PlotSettingsDialog(dataSeriesNames);
    d->exec();

    // TODO(Steffi)
    return PlotSettings();
}

void PlotSettingsDialog::onSameScaleStateChanged(int state)
{
    if (state == Qt::Unchecked) {
        foreach (QAbstractButton *b, ui->scaleChoicesGroup->buttons()) {
            b->setVisible(false);
        }
    } else if (state == Qt::Checked) {
        foreach (QAbstractButton *b, ui->scaleChoicesGroup->buttons()) {
            b->setVisible(true);
        }
    }
}
