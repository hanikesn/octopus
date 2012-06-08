#include "gui/sourcedialog.h"

#include <QDebug>

SourceDialog::SourceDialog(const DataProvider &dataProvider, QWidget *parent) :
    QDialog(parent),
    checkStateChangeSource(0)
{
    ui.setupUi(this);
    setUpSourceTree(dataProvider);
    connect(ui.sourceTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
}

void SourceDialog::setUpSourceTree(const DataProvider &dataProvider)
{
    ui.sourceTree->header()->setVisible(false);
    foreach (QString s, dataProvider.getDataSeriesList()) {
        QStringList components = s.split(".", QString::SkipEmptyParts);

        QString deviceName = components.takeFirst();
        QList<QTreeWidgetItem*> itemsWithSameName = ui.sourceTree->findItems(deviceName, Qt::MatchFixedString);
        QTreeWidgetItem *parentItem = 0;
        if (itemsWithSameName.isEmpty()) {
            parentItem = new QTreeWidgetItem(ui.sourceTree);
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
                childItem->setCheckState(0, Qt::Unchecked);
                childItem->setText(0, comp);
            } else {
                childItem = itemsWithSameName.takeFirst();
            }
            parentItem = childItem;
        }
    }

    // TODO(Steffi): Remove
    ui.sourceTree->expandAll();
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
        if (ui.sourceTree->indexOfTopLevelItem(item) == -1) {
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

QStringList SourceDialog::getSources(const DataProvider &dataProvider, QWidget *parent)
{
    SourceDialog *d = new SourceDialog(dataProvider, parent);
    d->exec();
    return d->getResult();
}

QStringList SourceDialog::getResult()
{
    if (result() == QDialog::Accepted) {
        return selectedSeries();
    } else {
        return QStringList();
    }
}

QStringList SourceDialog::selectedSeries() {
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
