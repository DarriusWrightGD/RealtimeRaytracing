import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle
{
	id : textSlider
	anchors.leftMargin : 60;
	anchors.rightMargin : 600;
	property alias min : slider.minimumValue;
	property alias max : slider.maximumValue;
	property alias step : slider.stepSize;
	property string text : "Value";
	property alias value : slider.value;
	property alias fixedValue : slider.fixedValue;
	property var onSliderValueChanged : (function(){})
	property alias sliderWidth : slider.width;
	property alias fontSize : label.font.pixelSize;


	signal valueSliderChanged(int value)

	Component.onCompleted: {
		label.text = text + " : " + slider.value;
		//slider.onValueChanged.connect(valueSliderChanged)
		//valueSliderChanged.connect(onSliderValueChanged);
	}

	
	Text
	{
		id: label
		anchors.left: slider.right
		anchors.leftMargin : 10;
		text : "1"
		color : "#81ACD8" ;
		font.pixelSize : 16;
		//width : 100;
		//height : 32;
		horizontalAlignment : Text.AlignHCenter;
		verticalAlignment : Text.AlignVCenter;
	}

	Slider
	{

		id: slider;
		
		maximumValue : 10.0;
		minimumValue  :0.0;
		updateValueWhileDragging :true;
		
		stepSize: 1.0;
		property int fixedValue : 2;
		value : 1.0;
		onValueChanged : 
		{
			label.text =  text + " : " +  slider.value.toFixed(fixedValue);
			valueSliderChanged(slider.value);
			onSliderValueChanged();
		}
	
	}
}