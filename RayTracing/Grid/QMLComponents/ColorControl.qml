
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : colorControl;
	signal colorChanged(real x,real y,real z)
	signal colorChanging(real x,real y,real z)
	anchors.fill : parent
	//property alias text : color.text;
	
	function setColor(x,y,z)
	{
		color.setValues(x,y,z)
	}


	Component.onCompleted : {
		
	}

	Components.Float3Slider
	{
		id : color;
		anchors.topMargin : 50;
		height : 50
		width : parent.width;
		Component.onCompleted : 
		{
			setMin(0.0,0.0,0.0);
			setMax(1.0,1.0,1.0);
			setStep(.1,.1,.1);
			setText("R", "G", "B");
		}

		onSlidersValueChanged : (function(){
			colorChanging(xValue, yValue,zValue);
		})
	}

}