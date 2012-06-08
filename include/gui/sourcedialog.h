#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include "dataprovider.h"
#include "ui_sourcedialog.h"

#include <QDialog>

class SourceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SourceDialog(const DataProvider &dataProvider, bool allInOneOption, QWidget *parent = 0);

    static QList<QStringList> getSources(const DataProvider &dataProvider, bool allInOneOption, QWidget *parent = 0);

private:
    Ui::SourceDialog ui;

    QList<QTreeWidgetItem*> checkedItems;
    QTreeWidgetItem *checkStateChangeSource;

    void setUpSourceTree(const DataProvider &dataProvider);
    QList<QStringList> getResult();

private slots:
    void onItemChanged(QTreeWidgetItem* item, int /*column*/);
    QStringList selectedSeries();
};

#endif // SOURCEDIALOG_H
