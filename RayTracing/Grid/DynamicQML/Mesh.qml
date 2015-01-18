
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import "../QMLComponents/componentCreation.js" as MyScript

Rectangle{

	signal changeModelName();
	signal indexChanged(int type);
	signal colorChanged(real x, real y, real z);
	signal indexOfRefractionChanged(real value);

	property int type : 0;
	property int lastType : 0;
	property var update : (function()
	{
		meshContainer
	})
	anchors.fill: parent;
	id: root;
	color : "#00000000";


	signal objectChanged();

	function ch( value)
	{
		console.log("called");
	}
	
	Component.onCompleted : 
	{
		meshPanel.indexChanged.connect(indexChanged);
		meshPanel.colorChanged.connect(colorChanged);
		indexOfRefractionChanged.connect(meshPanel.indexOfRefractionChanged);
		indexOfRefractionChanged.connect(ch);
	}
	Rectangle
	{
		id : meshContainer;
		objectName : "mesh"
		color : "#00000000";

		width : parent.width;
		height : parent.height;

		Components.MeshPanel
		{
			id: meshPanel;
		}

	}


}
