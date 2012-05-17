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

private:
    static const QString NORMAL_STYLE;

    QLabel *actionArea;
    
};

#endif // EXAMPLETRACK_H
