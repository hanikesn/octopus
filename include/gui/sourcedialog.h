#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include "dataprovider.h"
#include "ui_sourcedialog.h"

#include <QDialog>

class SourceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SourceDialog(const DataProvider &dataProvider,
                          const QString &dialogTitle,
                          bool allInOneOption,
                          const QStringList &preselected,
                          QWidget *parent = 0);

    /**
     * @return One list per track, with the full names of the selected data series. An empty list if the dialog was canceled.
     */
    static QList<QStringList> getSources(const DataProvider &dataProvider,
                                         const QString &dialogTitle,
                                         bool allInOneOption = true,
                                         const QStringList &preselected = QStringList(),
                                         QWidget *parent = 0);

private:
    Ui::SourceDialog ui;

    QList<QTreeWidgetItem*> checkedItems;
    QTreeWidgetItem *checkStateChangeSource;

    void setUpSourceTree(const DataProvider &dataProvider, const QStringList &preselectedItems);

    /**
     * @return The result of the dialog to be returned to the caller.
     */
    QList<QStringList> getResult();

private slots:
    void onItemChanged(QTreeWidgetItem* item, int /*column*/);

    /**
     * @return A list of the full names of the selected data series.
     */
    QStringList selectedSeries();
};

#endif // SOURCEDIALOG_H
