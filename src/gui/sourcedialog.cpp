#include "gui/sourcedialog.h"

#include "dataprovider.h"
#include "ui_sourcedialog.h"

#include <QDebug>

SourceDialog::SourceDialog(const DataProvider &dataProvider,
                           const QString &dialogTitle,
                           bool allInOneOption,
                           const QStringList &preselected,
                           QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SourceDialog()),
    checkStateChangeSource(0)
{
    ui->setupUi(this);
    setWindowTitle(dialogTitle);
    setUpSourceTree(dataProvider, preselected);
    connect(ui->sourceTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));

    ui->allInOneOption->setVisible(allInOneOption);
    // If option is visible, default checkState is Qt::Unchecked.
    // If option is invisible, default checkState is Qt::Checked.
    ui->allInOneOption->setChecked(!allInOneOption);
}

SourceDialog::~SourceDialog()
{
    delete ui;
}

void SourceDialog::setUpSourceTree(const DataProvider &dataProvider, const QStringList &preselectedItems)
{
    foreach (QString s, dataProvider.getDataSeriesList()) {
        bool preselect = false;
        if (preselectedItems.contains(s)) {
            preselect = true;
        }
        QStringList components = s.split(".", QString::SkipEmptyParts);

        QString deviceName = components.takeFirst();
        QList<QTreeWidgetItem*> itemsWithSameName = ui->sourceTree->findItems(deviceName, Qt::MatchFixedString);
        QTreeWidgetItem *parentItem = 0;
        if (itemsWithSameName.isEmpty()) {
            parentItem = new QTreeWidgetItem(ui->sourceTree);
            parentItem->setCheckState(0, Qt::Unchecked);
            parentItem->setText(0, deviceName);
        } else {
            parentItem = itemsWithSameName.takeFirst();
        }

        foreach (QString comp, components) {
            itemsWithSameName.clear();
            for (int i = 0; i < parentItem->childCount(); i++) {
                if (parentItem->child(i)->text(0) == comp) {
                    itemsWithSameName.append(parentItem->child(i));
                }
            }
            QTreeWidgetItem *childItem = 0;
            if (itemsWithSameName.isEmpty()) {
                childItem = new QTreeWidgetItem(parentItem);
                childItem->setText(0, comp);
                if (preselect && components.indexOf(comp) == components.length() - 1) {
                    childItem->setCheckState(0, Qt::Checked);
                    checkedItems.append(childItem);
                    parentItem->setExpanded(true);
                } else {
                    childItem->setCheckState(0, Qt::Unchecked);
                }
            } else {
                childItem = itemsWithSameName.takeFirst();
            }
            parentItem = childItem;
        }
    }
}

void SourceDialog::onItemChanged(QTreeWidgetItem *item, int /*column*/)
{
    if (!checkStateChangeSource) {
        checkStateChangeSource = item;
    }

    if (item->checkState(0) == Qt::Checked) {
        checkedItems.append(item);
    } else {
        checkedItems.removeAll(item);

        // If a non-top-level item was unchecked, uncheck its parent, too.
        if (ui->sourceTree->indexOfTopLevelItem(item) == -1) {
            item->parent()->setCheckState(0, Qt::Unchecked);
        }
    }

    // If this item was the source for the checkState change, change all children, grandchildren, greatgrandchildren, ... , too.
    if (checkStateChangeSource == item) {
        checkStateChangeSource = 0;
        for (int i = 0; i < item->childCount(); i++) {
            item->child(i)->setCheckState(0, item->checkState(0));
        }
    }
}

QList<QStringList> SourceDialog::getSources(const DataProvider &dataProvider,
                                            const QString &dialogTitle,
                                            bool allInOneOption,
                                            const QStringList &preselected,
                                            QWidget *parent)
{
    SourceDialog *d = new SourceDialog(dataProvider, dialogTitle, allInOneOption, preselected, parent);
    d->exec();
    return d->getResult();
}

QList<QStringList> SourceDialog::getResult()
{
    QList<QStringList> sources;
    if (result() == QDialog::Accepted && ui->allInOneOption->checkState() == Qt::Checked) {
        sources.append(selectedSeries());
    } else if (result() == QDialog::Accepted && ui->allInOneOption->checkState() == Qt::Unchecked) {
        foreach (QString s, selectedSeries()) {
            sources.append(QStringList(s));
        }
    }
    return sources;
}

QStringList SourceDialog::selectedSeries()
{
    QStringList selection;

    foreach (QTreeWidgetItem *item, checkedItems) {
        if (item->childCount() == 0) {
            QString fullName = item->text(0);
            QTreeWidgetItem *parent = item->parent();
            // the parent of a top-level item is a null-pointer
            while (parent) {
                fullName.prepend(parent->text(0) + ".");
                parent = parent->parent();
            }
            selection.append(fullName);
        }
    }

    return selection;
}
