import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components

Rectangle
{

	signal lookAtChanged(real x,real y,real z);
	signal positionChanged(real x,real y,real z);
	signal distanceValueChanged(real value);

	property var setLookAt :(function(x,y,z){ lookAt.setValues(x,y,z) });
	property var setPosition :(function(x,y,z){ position.setValues(x,y,z) });
	property alias distance : distanceSlider.value;
	
	color : "#00000000";
		

	
	Components.Panel
	{
		id : positionTitle;
		title : "Position"
		y: 5;
		height : 35;
		width : parent.width;
		
	
	}
	
	Components.Float3Slider
	{
		anchors.top : positionTitle.bottom;
	color : "#00000000";
		id: position;
	anchors.leftMargin : 20;
		anchors.topMargin : 10;
		x: parent.width * .1;
		height : 50;
		width : 100;
		Component.onCompleted : {
		
			setMin(-40.0,-40.0,-10.0);
			setMax(40.0,40.0,1000.0);
			setStep(1,1,1);
		}

		onSlidersValueChanged : (function()
		{
			positionChanged(position.xValue, position.yValue, position.zValue);
		})

	}

	Components.Panel
	{
		id : distanceTitle;
		title : "Distance"
		y: 5;
		height : 35;
		width : parent.width;
		anchors.top : position.bottom;
		
		
	
	}
	
	Components.TextSlider
	{
		id : distanceSlider;
		anchors.top : distanceTitle.bottom;
		color : "#00000000";
		min : 0.0;
		max : 1000.0;
		anchors.leftMargin : 20;
		anchors.topMargin : 10;
		x: parent.width * .1;
		height : 50;
		width : 100;
		text : "Distance"

		onSliderValueChanged : (function()
		{
			distanceValueChanged(distanceSlider.value)
			
		})
	}/*

		Components.Panel
		{
			id : lookAtTitle;
			title : "Look At"
			y: 5;
			height : 35;
			width : parent.width;
			anchors.top : position.bottom;
		
		
	
		}

	Components.Float3Slider
	{
		anchors.top : lookAtTitle.bottom;
		id: lookAt;
		
		color : "#00000000";
		anchors.leftMargin : 20;
		anchors.topMargin : 10;
		x: parent.width * .1;
		height : 50;
		width : 100;



		Component.onCompleted : {
		
			setMin(-10.0,-10.0,-10.0);
			setMax(10.0,10.0,10.0);
			setStep(1,1,1);
		}

		onSlidersValueChanged : (function()
		{
			lookAtChanged(lookAt.xValue, lookAt.yValue, lookAt.zValue);
		})

	}

	*/
}
