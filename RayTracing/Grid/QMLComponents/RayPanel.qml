import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components


Rectangle
{

	signal numberOfBouncesChanged(int change)
	signal numberOfSamplesChanged(int change)
	
	Components.TextSlider
	{
		id : bounces;
		text: "Bounces"
		min : 0;
		max : 10;
		step : 1;
		height : 50;
		sliderWidth : 150;
		anchors.leftMargin : 10;
		fixedValue : 0;

		onSliderValueChanged : (function()
		{
			numberOfBouncesChanged(value);
		})

	}


	Components.TextSlider
	{
		
		anchors.top : bounces.bottom;
		text: "Samples"
		min : 1;
		max : 10;
		step : 1;
		sliderWidth : 150;
		fixedValue : 0;

		onSliderValueChanged : (function()
		{
			numberOfSamplesChanged(value);
		})

	}
}