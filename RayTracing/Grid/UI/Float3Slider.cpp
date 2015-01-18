#include "Float3Slider.h"


Float3Slider::Float3Slider(char * name, glm::vec3 min, glm::vec3 max, float * value , glm::vec3 gran) :
	min(min), max(max), value(value), gran(gran)
{
	sliderLayout = new QHBoxLayout();
	vectorLabel = new QLabel(name);
	sliderVector[0] = new FloatSlider("X", min[0],max[0],&value[0], gran[0]);
	sliderVector[1] = new FloatSlider("Y", min[1],max[1],&value[1], gran[1]);
	sliderVector[2] = new FloatSlider("Z", min[2],max[2],&value[2], gran[2]);


	sliderLayout->addWidget(vectorLabel);
	sliderLayout->addWidget(sliderVector[0]);
	sliderLayout->addWidget(sliderVector[1]);
	sliderLayout->addWidget(sliderVector[2]);

	sliderLayout->setContentsMargins(0,0,0,0);
	sliderLayout->setSpacing(0);
	sliderLayout->setMargin(0);

	setLayout(sliderLayout);

}


Float3Slider::~Float3Slider(void)
{
}

void Float3Slider::update()
{
	for (int i = 0; i < 3; i++)
	{
		sliderVector[i]->update();
	}

}