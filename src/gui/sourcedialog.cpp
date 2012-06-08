#include "gui/sourcedialog.h"

SourceDialog::SourceDialog(const DataProvider &dataProvider, QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
}

QStringList SourceDialog::getSources(const DataProvider &dataProvider, QWidget *parent)
{
    SourceDialog *d = new SourceDialog(dataProvider, parent);
    d->exec();
    return d->getResult();
}

QStringList SourceDialog::getResult()
{
    // TODO(Steffi)
    return QStringList();
}
