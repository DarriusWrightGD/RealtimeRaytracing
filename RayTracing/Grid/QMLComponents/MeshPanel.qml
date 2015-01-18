import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents/componentCreation.js" as MyScript
import "../QMLComponents/" as Components
Rectangle 
{
	signal indexChanged(int type);
	signal colorChanged(real x, real y, real z);
	signal indexOfRefractionChanged(real value);

	anchors.fill : parent;
	radius : 5;
			color : "#00000000";

	function setColor(x,y,z)
	{
		diffuse.setColor(x,y,z);
		specular.setColor(x,y,z);
		transparent.setColor(x,y,z);
	}
	function  setMaterialType( type)
	{
		materialChoice.currentIndex = type; 
	}

	function setRefractionIndex(value)
	{
		transparent.setRefractionIndex(value);
	}

	id : root;
	function completed(object)
	{
		object.anchors.top = materialUI.anchors.bottom;
	}

	Component.onCompleted : 
	{
		diffuse.diffuseColorChanged.connect(colorChanged);
		specular.specularColorChanged.connect(colorChanged);
		transparent.transparentColorChanged.connect(colorChanged);
		transparent.indexOfRefractionChanged.connect(indexOfRefractionChanged);
	}




	Rectangle 
	{
		id : materialUI;
		color : "#00000000";
		
	anchors.fill : parent;


		Rectangle {
		id : wrap;
		color : "#00000000";

		//anchors.fill : parent;
		height : 200;
		width : parent.width;
		Components.Panel
		{
			id : materialTitle;
			title : "Material"
			y: 5;
			height : 35;
			width : parent.width;
		}
		ComboBox
		{

			id : materialChoice;
			

			currentIndex : 0;
			property int lastIndex : currentIndex ;
			width : 200;
			height : 20;
			anchors.leftMargin : 20;
			anchors.topMargin : 10;
			x: parent.width * .1;
			
			anchors.top : materialTitle.bottom;
			model : ["Diffuse", "Specular", "Transparent"]

			onCurrentIndexChanged :{
				 indexChanged(currentIndex);

				 

				 
				 if(currentIndex != lastIndex)
				 {
					//materialControls.destroy();
					specular.opacity = 0;
					diffuse.opacity = 0;
					transparent.opacity = 0;

					transparent.visible = false;
					diffuse.visible = false;
					specular.visible = false;

					transparent.focus = false;
					diffuse.focus = false;
					specular.focus = false;



					if(currentIndex == 0)
					{
						//diffuse
						
						//materialControls =  MyScript.createObjects("DiffuseControl", root);
						diffuse.opacity = 1;
						diffuse.visible = true;
						diffuse.focus = true;
					}
					else if( currentIndex == 1)
					{
						//specular
						//materialControls =  MyScript.createObjects("SpecularControl", root);
						specular.opacity = 1;
						specular.visible = true;
						specular.focus = true;

					}
					else if(currentIndex == 2)
					{
						//glass
						//materialControls =  MyScript.createObjects("TransparentControl", materialUI);
						transparent.opacity = 1;
						transparent.visible = true;
						transparent.focus = true;

					}
					//materialControls.parent = root;
					//materialControls.anchors.top = materialChoice.bottom;
					
					
				 }

				 lastIndex = currentIndex;


			}
		}
		}
		Components.DiffuseControl
		{
			id : diffuse;
			color : "#00000000";

			
			anchors.topMargin : 100;
			
			focus : true;
			opacity : 1;
			anchors.top : wrap.bottom;
//			anchors.fill : parent;

		}

		Components.SpecularControl
		{
			id : specular;
			x : 20;
			y : 150;
		color : "#00000000";
			anchors.topMargin : 100;

			
			opacity : 0;
			visible :false
			
			anchors.fill : parent;

		}
		Components.TransparentControl
		{
			id: transparent;
			x : 20;
		color : "#00000000";
			anchors.topMargin : 100;


			opacity : 0;
			visible : false;
			
			anchors.fill : parent;

		}

	}

}