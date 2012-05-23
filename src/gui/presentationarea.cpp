#include "presentationarea.h"
#include "track.h"
#include <QVBoxLayout>

PresentationArea::PresentationArea(QWidget *parent) :
    QWidget(parent)
{
    trackLayout = new QVBoxLayout(this);
    trackLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);


}

PresentationArea::~PresentationArea()
{

}

void PresentationArea::addTrack(Track *t)
{
    trackLayout->addWidget(t);
}
