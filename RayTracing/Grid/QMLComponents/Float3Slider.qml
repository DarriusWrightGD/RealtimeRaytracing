import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components


Rectangle
{
	color : "#00000000";
	


	property alias xMin : xSlider.min;
	property alias yMin : ySlider.min;
	property alias zMin : zSlider.min;
						   
	property alias xMax : xSlider.max;
	property alias yMax : ySlider.max;
	property alias zMax : zSlider.max;

	property alias xStep  : xSlider.step;
	property alias yStep  : ySlider.step;
	property alias zStep  : zSlider.step;
	
	property alias xValue  : xSlider.value;
	property alias yValue  : ySlider.value;
	property alias zValue  : zSlider.value;

	property alias xText  : xSlider.text;
	property alias yText  : ySlider.text;
	property alias zText  : zSlider.text;
	
	function setText(x,y,z)
	{
		xText = x;
		yText = y;
		zText = z;
	}

	property var onSlidersValueChanged : (function(){})
	//property alias text : label.text;


	signal valueSlidersChanged(double x, double y, double z);

	Component.onCompleted : {
		//label.text = text
		setMin(-1.0,-1.0,-1.0);
		setMax(1.0,1.0,1.0);
		setStep(0.1,0.1,0.1);

	}


	function setMin( x,  y,  z)
	{
		xSlider.min = x;
		ySlider.min = y;
		zSlider.min = z;
	}

	function setMax( x,  y,  z)
	{
		xSlider.max = x;
		ySlider.max = y;
		zSlider.max = z;	
	}

	function setStep( x,  y , z)
	{
		xSlider.step = x;
		ySlider.step = y;
		zSlider.step = z;
	}

	function setValues(x,y,z)
	{
		xValue = x;
		yValue = y;
		zValue = z;
	}
	/*
	Text
	{
		id: label
		text : "Default"
		color :"#81ACD8" ;
		font.pixelSize: 16;
		//width : 50;
		//height: 16;

		horizontalAlignment : Text.AlignHCenter;
		verticalAlignment : Text.AlignVCenter;
	}
	*/



	RowLayout
	{
	anchors.leftMargin : 20;
	//anchors.left: label.right
	Components.TextSlider
	{

		id : xSlider;
		text : "x";
		width : 135; // safe
		sliderWidth : 50; //safe

		
		onSliderValueChanged : (function()
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
			onSlidersValueChanged();
			
		})
	}
	Components.TextSlider
	{
		id : ySlider;
		text : "y";
		width : 135;
	
		sliderWidth : 50;


		onSliderValueChanged : (function()
		{
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
			onSlidersValueChanged();
		})
	}
	Components.TextSlider
	{
		id : zSlider;
		text : "z";
		width : 135;

		sliderWidth : 50;

		onSliderValueChanged : (function(){
			valueSlidersChanged(xSlider.value, ySlider.value, zSlider.value);
			onSlidersValueChanged();
		})
	}
	}
}