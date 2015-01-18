import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "QMLComponents" as Components

import GameObject 1.0

Rectangle
{
	anchors.fill: parent;
	//color : "#00000000";
	signal modelChanged(int index, int t);

	
	 gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#3E393A"
            }
            GradientStop {
                position: 1.0; color: "#000000"
            }
    }
	signal numberOfBouncesChanged(int change);
	signal numberOfSamplesChanged(int change);

	Component.onCompleted: {
		rayPanel.numberOfBouncesChanged.connect(numberOfBouncesChanged);
		rayPanel.numberOfSamplesChanged.connect(numberOfSamplesChanged);
	}

	Rectangle
	{
		id : searchBar;
		x: 2;
		y : 10;
		width: search.width;
		height: search.height;
		color: parent.color;
		border.color: "#969696"
		border.width: 2
		radius: 10
		Components.Searchbox
		{
			id : search
		}
	}
	Rectangle
	{
		id: gameObjectUIContainer
		anchors.top : searchBar.bottom;
		anchors.topMargin : 20;
		width : parent.width;
		height : parent.height - rayPanel.height;
		anchors.leftMargin : 200;
		color : "#00000000";

		ListView {
			id : gameObjectsListView;
			width: parent.width; height: 100

			clip : true;
			model: gameObject;
			
			focus : true;
			delegate: Rectangle {

				width:  gameObjectUIContainer.width - 10;
				height: 20
				color : ListView.isCurrentItem ?  "#81ACD8": "#00000000";
				radius: 3
				x: 4;
				property alias textColor : nameText.color;
				anchors.leftMargin : 3;
				anchors.rightMargin : 3;

				Text 
				{ 
					id : nameText
					text:  "   "+ model.modelData.name 
					color:(gameObjectsListView.currentIndex == index) ?   "#3E393A" : "#81ACD8" 
					font.pixelSize : 16;
					//color : ""

				}
				MouseArea
				{
					anchors.fill: parent
					hoverEnabled : true;
					onClicked : 
					{
						gameObjectsListView.currentItem.color = "#00000000";
						gameObjectsListView.currentItem.textColor = "#81ACD8";

						parent.color = "#81ACD8"
						parent.textColor = "#3E393A"
						
						gameObjectsListView.currentIndex = index;
						//modelChanged(model.modelData.index,model.modelData.type);
						modelChanged(model.modelData.index,model.modelData.type);
						
					}
					onEntered : 
					{
						if(gameObjectsListView.currentIndex != index)
						{
							parent.color = "#efefef";
							nameText.color = "#3E393A";
						}
					}
					onExited : 
					{
						if(gameObjectsListView.currentIndex != index)
						{
							parent.color =  "#00000000";
							nameText.color =  "#81ACD8" 
						}
					}
				}

			}

			
		}
		Components.RayPanel
		{
			id : rayPanel;
			width : parent.fill;
			height : 200;
			anchors.top : parent.bottom;
		}
	}			
}
