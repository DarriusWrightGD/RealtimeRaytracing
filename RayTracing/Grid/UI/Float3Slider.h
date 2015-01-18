#pragma once
#include "FloatSlider.h"
#include <glm.hpp>
class Float3Slider : public QWidget
{
public:
	Float3Slider(char * name, glm::vec3 min, glm::vec3 max, float * value , glm::vec3 gran = glm::vec3(100.0f));
	~Float3Slider(void);
	void update();

private:
	FloatSlider * sliderVector[3];
	float * value;
	glm::vec3 min;
	glm::vec3 max; 
	glm::vec3 gran;

	QHBoxLayout * sliderLayout;
	QLabel * vectorLabel;

};

