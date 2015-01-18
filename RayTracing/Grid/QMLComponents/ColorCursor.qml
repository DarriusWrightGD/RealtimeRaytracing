import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components		
		
Item {
	width: 110; height: 110
	Item {
		id: pickerCursor
		Rectangle {
			x: -10; y: -10
			width: 20; height: 20; radius: 10
			border.color: "black"; border.width: 2
		}
	}
	MouseArea {
		anchors.fill: parent
		function handleMouse(mouse) {
			if (mouse.buttons & Qt.LeftButton) {
				pickerCursor.x = Math.max(0, 
					Math.min(width,  mouse.x));
				pickerCursor.y = Math.max(0, 
					Math.min(height, mouse.y));
			}
		}
		onPositionChanged: handleMouse(mouse)
		onPressed: handleMouse(mouse)
	}
}