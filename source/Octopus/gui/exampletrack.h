#ifndef EXAMPLETRACK_H
#define EXAMPLETRACK_H

#include <QWidget>
class QLabel;

class ExampleTrack : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleTrack(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);

    void resizeEvent(QResizeEvent * re);
    
signals:
    
public slots:

    void adjustVisibleRange(int position, int rangeMin, int rangeMax);

private:
    static const QString NORMAL_STYLE;

    QLabel *actionArea;
    QLabel *customPlot;

};

#endif // EXAMPLETRACK_H
