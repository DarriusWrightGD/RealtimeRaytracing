#pragma once
#include <QtWidgets\qwidget.h>
#include <QtWidgets\qslider.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qlayout.h>

class FloatSlider : public QWidget
{
public:
	FloatSlider(char * name, float min, float max, float * value, float gran = 100.0f);
	~FloatSlider(void);
	void update();

private:
	QLabel * sliderName;
	QLabel * sliderValue;
	QSlider * floatSlider;
	QHBoxLayout * sliderLayout;
	float * value;
	float min;
	float max;
	float gran;

	void setValue();
	float getValue();
};

