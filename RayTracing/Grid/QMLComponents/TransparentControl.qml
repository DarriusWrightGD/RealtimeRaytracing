import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : transparentControl;
	color : "#00000000";
	anchors.fill : parent;

	signal transparentColorChanged(real x,real y,real z)
	signal indexOfRefractionChanged(real value)


	function setColor(x,y,z)
	{
		transparentColor.setColor(x,y,z);
	}

	function setRefractionIndex(value)
	{
		refractionIndex.value = value;
		console.log("R");
	}

	Component.onCompleted :
	{
		transparentColor.colorChanging.connect(transparentColorChanged);
	

	}

	Components.Panel
	{
		id : refractionTitle;
		title : "Refraction"
		y: 5;
		height : 35;
		width : parent.width;
		
	
	}

	Components.TextSlider
	{
		color : "#00000000";
		anchors.top : refractionTitle.bottom;


		id : refractionIndex;
		min : 0.0;
		max : 3.0;
		step : 0.1;
		x: parent.width * .2;
		text: "";
		//width : 100;
		height :50;
		y : 15;
		onSliderValueChanged :(function() {
			indexOfRefractionChanged( value);
		})
	}

	Components.Panel
	{
		id : colorTitle;
		title : "Color"
		y: 5;
		height : 35;
		width : parent.width;
		anchors.top : refractionIndex.bottom;
	
	}

	Components.ColorControl
	{
		anchors.top : colorTitle.bottom;
		color : "#00000000";
		anchors.leftMargin : parent.width * .1;
		anchors.topMargin : 150;
		x: parent.width * .1;
		height : 50;
		width : 100;
		y: 100;

		id : transparentColor
		//text: "Color"
	}

}