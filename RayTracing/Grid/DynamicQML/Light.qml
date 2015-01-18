
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle{

	signal changeModelName();
	property int type : 0;
	property int lastType : 0;
	anchors.fill: parent;
	id: root;
	color : "#00000000";



	//signal update(int type);
	
	Component.onCompleted : 
	{
		
	}

	Rectangle
	{
		id : lightContainer;
		anchors.leftMargin : 20;
		anchors.topMargin : 20;
		color : "#00000000";
		anchors.fill: parent;
		opacity : 1;

		Components.LightPanel
		{

		}
	}
}