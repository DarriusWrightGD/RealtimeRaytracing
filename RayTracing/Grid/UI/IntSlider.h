#pragma once
#include <QtWidgets\qwidget.h>
#include <QtWidgets\qslider.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qlayout.h>
class IntSlider : public QWidget
{
	Q_OBJECT
public:
	IntSlider(char * name, int min, int max , int * value);
	~IntSlider(void);



 signals:
	void valueChanged(int);
private  slots : 
	void update(int);
private:
	QLabel * sliderValue;
	QLabel * sliderName;
	QSlider * intSlider;
	QHBoxLayout * sliderLayout;
	int * value;
	int min;
	int max;


	void setValue();
	int getValue();

};

