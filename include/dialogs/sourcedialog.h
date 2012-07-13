#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include <QDialog>

class DataProvider;
class QTreeWidgetItem;
namespace Ui {
    class SourceDialog;
}

/*
  The source dialog shows all data series currently available from the data provider
and allows the user to choose from them.
  Optionally, the caller can pass it a list of data series to be preselected.
  The dialog also offers a checkbox for choosing whether all selected data series should
be shown in one track or each in their own track. Whether or not this choice is made
available to the user depends on the parameters passed to the dialog by its caller.
*/
class SourceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SourceDialog(const DataProvider &dataProvider,
                          const QString &dialogTitle,
                          bool allInOneOption,
                          const QStringList &preselected,
                          QWidget *parent = 0);

    ~SourceDialog();

    /**
     * @return One list per track to be changed or created, with the full names
     *      of the data series to be shown in that track.
     *      An empty list if the dialog was canceled.
     */
    static QList<QStringList> getSources(const DataProvider &dataProvider,
                                         const QString &dialogTitle,
                                         bool allInOneOption = true,
                                         const QStringList &preselected = QStringList(),
                                         QWidget *parent = 0);

private:
    Ui::SourceDialog* ui;

    QList<QTreeWidgetItem*> checkedItems;

    /**
     * The item whose check state was changed by the user, which triggered the
     *      check state change for its child items.
     */
    QTreeWidgetItem *checkStateChangeSource;

    /**
     * Sets up the checkbox tree for the dialog.
     */
    void setUpSourceTree(const DataProvider &dataProvider, const QStringList &preselectedItems);

    /**
     * @return The result of the dialog to be returned to the caller.
     */
    QList<QStringList> getResult();

    /**
     * @return A list of the full names of the selected data series.
     */
    QStringList selectedSeries();

private slots:
    /**
     * This slot is called if the check state of one of the items is changed.
     */
    void onItemChanged(QTreeWidgetItem* item, int /*column*/);
};

#endif // SOURCEDIALOG_H
