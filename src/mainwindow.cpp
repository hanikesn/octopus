#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <sstream>

#include "dataprovider.h"
#include "gui/presentationarea.h"
#include "gui/mainview.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

namespace boost { namespace property_tree { namespace json_parser
{
    // Create necessary escape sequences from illegal characters
    template<>
    std::basic_string<char> create_escapes(const std::basic_string<char> &s)
    {
        std::basic_string<char> result;
        std::basic_string<char>::const_iterator b = s.begin();
        std::basic_string<char>::const_iterator e = s.end();
        while (b != e)
        {
            // This assumes an ASCII superset. But so does everything in PTree.
            // We escape everything outside ASCII, because this code can't
            // handle high unicode characters.
            if (*b == 0x20 || *b == 0x21 || (*b >= 0x23 && *b <= 0x2E) ||
                (*b >= 0x30 && *b <= 0x5B) || (*b >= 0x5D && *b <= 0xFF)  //it fails here because char are signed
                || (*b >= -0x80 && *b < 0 ) ) // this will pass UTF-8 signed chars
                result += *b;
            else if (*b == char('\b')) result += char('\\'), result += char('b');
            else if (*b == char('\f')) result += char('\\'), result += char('f');
            else if (*b == char('\n')) result += char('\\'), result += char('n');
            else if (*b == char('\r')) result += char('\\'), result += char('r');
            else if (*b == char('/')) result += char('\\'), result += char('/');
            else if (*b == char('"'))  result += char('\\'), result += char('"');
            else if (*b == char('\\')) result += char('\\'), result += char('\\');
            else
            {
                const char *hexdigits = "0123456789ABCDEF";
                typedef make_unsigned<char>::type UCh;
                unsigned long u = (std::min)(static_cast<unsigned long>(
                                                 static_cast<UCh>(*b)),
                                             0xFFFFul);
                int d1 = u / 4096; u -= d1 * 4096;
                int d2 = u / 256; u -= d2 * 256;
                int d3 = u / 16; u -= d3 * 16;
                int d4 = u;
                result += char('\\'); result += char('u');
                result += char(hexdigits[d1]); result += char(hexdigits[d2]);
                result += char(hexdigits[d3]); result += char(hexdigits[d4]);
            }
            ++b;
        }
        return result;
    }
} } }


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    projectName(QString::fromUtf8("ü"))
{    
    ui.setupUi(this);
    pa = new PresentationArea(&trackScene, dataProvider, ui.hScrollBar);

    saveAction = new QAction(tr("Save"), this);
    loadAction = new QAction(tr("Load..."), this);

    ui.mainView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui.mainView->setScene(&trackScene);
    ui.mainView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);    
    ui.mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    connect(ui.mainView, SIGNAL(changedRange(qint64, qint64)), pa, SLOT(onRangeChanged(qint64, qint64)));
    connect(ui.mainView, SIGNAL(verticalScroll()), this, SLOT(onVerticalScroll()));
    connect(this, SIGNAL(verticalScroll(QRectF)), pa, SIGNAL(verticalScroll(QRectF)));
    connect(this, SIGNAL(changedWindowSize(QSize)), pa, SLOT(onChangedWindowSize(QSize)));
    connect(pa, SIGNAL(exportRange(qint64,qint64)), this, SLOT(onExportRange(qint64,qint64)));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(onLoad()));

    setUpButtonBars();
    setUpMenu();

}

MainWindow::~MainWindow()
{
    delete pa;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    /* One resizeEvent is fired right after initialisation of window.
       We dont want to receive this --> wait for first "actual" resizeEvent.*/
    if(event->oldSize().isValid())
        emit changedWindowSize(ui.mainView->size());
}

void MainWindow::onImportAction()
{
    // TODO:
    qDebug() << "Import";
}

void MainWindow::onExportAction()
{
    // TODO:
    qDebug() << "Export";
}

void MainWindow::onPlayAction()
{
    // TODO:
    qDebug() << "Play";
}

void MainWindow::setUpButtonBars()
{
    addTrackButton.setIcon(QIcon(":/buttons/toolbar/icons/add.png"));
    importButton.setIcon(QIcon(":/buttons/toolbar/icons/import.png"));
    exportButton.setIcon(QIcon(":/buttons/toolbar/icons/export.png"));
    playButton.setIcon(QIcon(":/buttons/toolbar/icons/play.png"));

    // add buttons to the horizontal layout in the toolbar
    layout.addWidget(&addTrackButton);
    layout.addWidget(&importButton);
    layout.addWidget(&exportButton);

    // buttonBar at the bottom:
    // set up spacers so they get as much space as possible (button between is then centered)
    spacerLeft = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    spacerRight = new QSpacerItem(100, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    ui.bottomButtonBar->addSpacerItem(spacerLeft);
    ui.bottomButtonBar->addWidget(&playButton);
    ui.bottomButtonBar->addSpacerItem(spacerRight);

    toolBarWidget.setLayout(&layout);

    connect(&addTrackButton, SIGNAL(clicked()), pa, SLOT(onAddTrack()));
    connect(&importButton, SIGNAL(clicked()), this, SLOT(onImportAction()));
    connect(&exportButton, SIGNAL(clicked()), this, SLOT(onExportAction()));
    connect(&playButton, SIGNAL(clicked()), this, SLOT(onPlayAction()));

    ui.mainToolBar->addWidget(&toolBarWidget);
    addToolBar(Qt::LeftToolBarArea, ui.mainToolBar);
}

void MainWindow::onVerticalScroll()
{    
    emit verticalScroll(ui.mainView->mapToScene(ui.mainView->viewport()->geometry()).boundingRect());
}

void MainWindow::onExportRange(qint64 begin, qint64 end)
{
    qDebug() << "MainWindow::onExportRange " << begin << ":" << end;
}

void MainWindow::setUpMenu()
{
    menu.setTitle(tr("File"));
    menu.addAction(saveAction);
    menu.addAction(loadAction);
    ui.menuBar->addMenu(&menu);
}

void MainWindow::onSave()
{
    using boost::property_tree::ptree;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    if(fileName.isEmpty()) return;


    //TODO(domi): set name of project

    ptree pt;
    //auto const& data = projectName.toUtf8();
    pt.put("projectName", projectName);


    pa->save(&pt);
    std::string fn = fileName.toLocal8Bit().constData();
    write_json(fn, pt);
}

void MainWindow::onLoad()
{
    using boost::property_tree::ptree;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"));
    qDebug() << "load file: " << fileName;

    ptree pt;
    std::string fn = fileName.toLocal8Bit().constData();
    read_json(fn, pt);

    QString projectname = pt.get<QString>("projectName");

    qDebug() << projectname;

}
