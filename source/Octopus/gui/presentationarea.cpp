#include "presentationarea.h"
#include "exampletrack.h"
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

void PresentationArea::addTrack(ExampleTrack *et)
{
    trackLayout->addWidget(et);
}
