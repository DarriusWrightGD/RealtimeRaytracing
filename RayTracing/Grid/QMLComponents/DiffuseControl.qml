import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : diffuseControl;
	signal diffuseColorChanged(real x,real y,real z)
	anchors.fill : parent;
	color : "#00000000";

	
	function setColor(x,y,z)
	{
		diffuseColor.setColor(x,y,z);
	}

	anchors.topMargin : 50;
	height : 50

	Component.onCompleted :
	{
		diffuseColor.colorChanging.connect(diffuseColorChanged);
	}

	Components.Panel
	{
		id : colorTitle;
		title : "Color"
		y: 5;
		height : 35;
		width : parent.width;
	}

	Components.ColorControl
	{
		anchors.top : colorTitle.bottom;
		color : "#00000000";
		anchors.leftMargin : 20;
		anchors.topMargin : 50;
		x: parent.width * .5;
		height : 50;
		width : 100;
		id : diffuseColor
		//text: "Color"
	}
}