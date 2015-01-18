import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle
{
	id : specularControl;
	signal specularColorChanged(real x,real y,real z)
	anchors.fill : parent;


	function setColor(x,y,z)
	{
		specularColor.setColor(x,y,z);
	}


	anchors.topMargin : 50;
	height : 50
	Component.onCompleted :
	{
		specularColor.colorChanging.connect(specularColorChanged);
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
		height : 50;
		width : parent.width;
		anchors.topMargin : 50;
		x: parent.width * .1;
		anchors.leftMargin :  parent.width * .1;

		id : specularColor
		//text: "Color"
	}


}