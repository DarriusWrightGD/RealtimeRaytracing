
import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components
import "QMLComponents/componentCreation.js" as MyScript

Rectangle{

	signal changeModelName();


	function setCameraLookAt(x, y,z)
	{
		cameraPanel.setLookAt(x,y,z);
	}

	function setCameraPosition(x,y,z)
	{
		cameraPanel.setPosition(x,y,z);
	}

	function setCameraDistance(value)
	{
		cameraPanel.distance = value;
	}

	function setLightPosition(x,y,z)
	{
		lightPanel.setPosition(x,y,z);
		console.log("setLight Position");
	}

	function setMaterialType( type)
	{
		meshPanel.setMaterialType(type);
	}

	function setLightColor(r,g,b)
	{

	}
	
	function setMeshColor(x,y,z)
	{
		meshPanel.setColor(x,y,z);
	}

	function setRefractionIndex(value)
	{
		meshPanel.setRefractionIndex(value);
	}

	property int type : 0;
	property int lastType : 0;
	anchors.fill : parent;
	id: root;
	objectName :"rootRect"
	
	 gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#3E393A"
            }
            GradientStop {
                position: 1.0; color: "#000000"
            }
    }
	
	signal update(int type);
	signal updateQML(int index , int type)
	signal materialChanged(int type)
	signal colorChanged(real x, real y, real z);
	signal lightPositionChanged(real x, real y, real z);
	signal cameraPositionChanged(real x, real y, real z);
	signal cameraLookAtChanged(real x, real y, real z);
	signal cameraDistanceChanged(real value);
	signal indexOfRefractionChanged(real value);
	
	Component.onCompleted : 
	{
		root.updateQML.connect(updateGameObject);
		meshPanel.indexChanged.connect(materialChanged);
		meshPanel.colorChanged.connect(colorChanged);
		meshPanel.indexOfRefractionChanged.connect(indexOfRefractionChanged);
		lightPanel.positionChanged.connect(lightPositionChanged);
		cameraPanel.positionChanged.connect(cameraPositionChanged);
		cameraPanel.lookAtChanged.connect(cameraLookAtChanged);
		cameraPanel.distanceValueChanged.connect(cameraDistanceChanged);
		//root.update.connect(lightContainer.change);
		//lVisible = false;
	}


	property var updateGameObject : (function(index, type){
			lightContainer.visible = false;
			cameraContainer.visible = false;
			meshContainer.visible = false;

			switch(type)
			{
				case  0 : // mesh
				meshContainer.visible = true;
				break;
				case  1 :  // light
				lightContainer.visible = true;
				break;
				case 2 :  // camera
				cameraContainer.visible = true;
				break;
			}

		}
	)

	property var updatePanel : (function(type){
		//lightContainer.visible = false;
	})



	function updateType(type)
	{
	//	cameraContainer.destroy();
		update(type);
	//	console.log(type);

	}
	
	Rectangle
	{
		id : cameraContainer
		color : "#00000000";

		visible : false;
		height : 100;
		width : parent.width;

		Components.CameraPanel
		{
			id : cameraPanel;
			width : parent.width;
			height : parent.height;
		}
	}
	
	
	
	Rectangle
	{
		
		
		height : 100;
		id : lightContainer;
		color : "#00000000";
		objectName : "light";
		
		width : parent.width ;
		visible : true;
		Components.LightPanel
		{
			id : lightPanel;

		}
	}

	
	Rectangle
	{
		id : meshContainer;

		objectName : "mesh"
		color : "#00000000";
		width : parent.width;
		height : parent.height;
		//opacity: 1 ;
		visible : false;
		Components.MeshPanel
		{
			id : meshPanel;
			width : parent.width;
			height : parent.height;
		}

	}





}

