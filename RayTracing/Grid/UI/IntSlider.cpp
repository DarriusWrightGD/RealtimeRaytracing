#include "IntSlider.h"


IntSlider::IntSlider(char * name, int min, int max , int * value) : min(min), max(max), value(value)
{

	sliderName = new QLabel(name);
	sliderValue = new QLabel(QString::number(*value));
	intSlider = new QSlider();
	intSlider->setOrientation(Qt::Horizontal);
	intSlider->setMinimum(min);
	intSlider->setMaximum(max);
	intSlider->setValue(*value);
	sliderLayout = new QHBoxLayout();

	sliderLayout->addWidget(sliderName);
	sliderLayout->addWidget(intSlider);
	sliderLayout->addWidget(sliderValue);



	setLayout(sliderLayout);
	connect(intSlider,&QSlider::valueChanged,this,&IntSlider::update);
}


IntSlider::~IntSlider(void)
{
}


void IntSlider::setValue()
{
	*value = getValue();

}

int IntSlider::getValue()
{
	return intSlider->value();

}


void IntSlider::update(int value)
{
	setValue();
	sliderValue->setText(QString::number(getValue()));
	emit valueChanged(getValue());

}