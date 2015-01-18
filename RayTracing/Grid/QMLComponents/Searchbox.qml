import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

FocusScope  {
    id: focusScope
    width: 292; height: 40

    Text  {
        id: typeSomething
        anchors.fill: parent; anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: "Search..."
        color: "white"
        font.italic: true
    }
 

 
    TextInput  {
        id: textInput
        anchors  { left: parent.left; leftMargin: 8; right: clear.left; rightMargin: 8; verticalCenter: parent.verticalCenter }
        focus: true
        selectByMouse: true
		color:"white"
    }
	
    Image  {
        id: clear
        anchors  { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
        source: "images/search.png"
        opacity: 1

    }
 
	ColorOverlay {
        anchors.fill: clear
        source: clear
        color: "#81ACD8"  
    }

    states: State  {
        name: "hasText"; when: textInput.text != ''
        PropertyChanges  { target: typeSomething; opacity: 0 }
        PropertyChanges  { target: clear; opacity: 1 }
    }
 
    transitions: [
        Transition  {
            from: ""; to: "hasText"
            NumberAnimation  { exclude: typeSomething; properties: "opacity" }
        },
        Transition  {
            from: "hasText"; to: ""
            NumberAnimation  { properties: "opacity" }
        }
    ]
}