#include "FloatSlider.h"


FloatSlider::FloatSlider(char * name, float min, float max, float * value, float gran) :
	min(min), max(max), value(value), gran(gran)
{
	sliderName = new QLabel(name);
	sliderValue = new QLabel("0.0");
	floatSlider = new QSlider();
	floatSlider->setOrientation(Qt::Horizontal);
	floatSlider->setMinimum(0);
	floatSlider->setMaximum(gran);
	
	floatSlider->setValue((*value - min) / (max - min) * gran);

	sliderLayout = new QHBoxLayout();
	sliderLayout->addWidget(sliderName);
	sliderLayout->addWidget(floatSlider);
	sliderLayout->addWidget(sliderValue);



	setLayout(sliderLayout);
}


FloatSlider::~FloatSlider(void)
{
}

float FloatSlider::getValue()
{
	return min + (max - min) * (floatSlider->value() / gran);


}

void FloatSlider::setValue()
{
	*value = getValue();//(*value - min) / (max - min);
	//floatSlider->setValue(*value * gran);
}

void FloatSlider::update()
{
	setValue();
	sliderValue->setText(QString::number(getValue()));
}