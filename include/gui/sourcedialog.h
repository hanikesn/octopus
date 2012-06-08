#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include "dataprovider.h"
#include "ui_sourcedialog.h"

#include <QDialog>

class SourceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SourceDialog(const DataProvider &dataProvider, QWidget *parent = 0);

    static QStringList getSources(const DataProvider &dataProvider, QWidget *parent = 0);

private:
    Ui::SourceDialog ui;

    QStringList getResult();
};

#endif // SOURCEDIALOG_H
