import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components

Item {
    property real value: (1 - pickerCursor.y/height)
    width: 15; height: 300
    Item {
        id: pickerCursor
        width: parent.width
        Rectangle {
            x: -3; y: -height*0.5
            width: parent.width + 4; height: 7
            border.color: "black"; border.width: 1
            color: "transparent"
            Rectangle {
                anchors.fill: parent; anchors.margins: 2
                border.color: "white"; border.width: 1
                color: "transparent"
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                pickerCursor.y = Math.max(0, Math.min(height, mouse.y))
            }
        }
        onPositionChanged: handleMouse(mouse)
        onPressed: handleMouse(mouse)
    }
}