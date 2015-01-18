import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components
import QtGraphicalEffects 1.0
Rectangle
{
	property alias title : panelText.text;
	height : 30;
	color :"#00000000" ;
	width : parent.width;

	Rectangle
	{
		width : parent.width * 0.96;
		height : parent.height;
		color :"#81ACD8" ;
		radius: 8;
		x: parent.width * 0.02;

			 gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#ACCFF2"
            }
            GradientStop {
                position: 1.0; color: "#81ACD8"
            }
		 }

		Image
		{
			id : downImage;
			source : "images/plus.png"
			x : 10;
			y : (parent.height * 0.3);
			height : parent.height*0.4;
			width  : parent.height*0.4;
		}
		 ColorOverlay {
			anchors.fill: downImage
			source: downImage
			color: "#3E393A"  
		}


		Text
		{
			text : "Default"
			id: panelText;
			color: "#3E393A"  
			x: parent.width  * .25;
			y: parent.height * 0.1;
			//horizontalAlignment : Text.AlignHCenter;
			//verticalAlignment : Text.AlignVCenter;
			//anchors.left : downImage.right;
			//horizontalAlignment : Text.AlignHCenter;
			font.pixelSize : parent.height*0.6;
		}
	}
}