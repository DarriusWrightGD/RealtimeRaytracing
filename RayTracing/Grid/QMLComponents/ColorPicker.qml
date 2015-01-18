import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components



Rectangle {
    width: 230; height: 200
    color: "#3C3C3C"
    Row {
        anchors.fill: parent
        spacing: 3

        // saturation/brightness picker box
        Components.SBPicker {
            id: sbPicker
            hueColor : "green"
            width: parent.height; height: parent.height
        }

        // hue picking slider
        Item {
            width: 12; height: parent.height
            Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { position: 1.0;  color: "#FF0000" }
                    GradientStop { position: 0.85; color: "#FFFF00" }
                    GradientStop { position: 0.76; color: "#00FF00" }
                    GradientStop { position: 0.5;  color: "#00FFFF" }
                    GradientStop { position: 0.33; color: "#0000FF" }
                    GradientStop { position: 0.16; color: "#FF00FF" }
                    GradientStop { position: 0.0;  color: "#FF0000" }
                }
            }
            Components.ColorSlider { id: hueSlider; anchors.fill: parent }
        }

        // alpha (transparency) picking slider
        Item {
            id: alphaPicker
            width: 12; height: parent.height
            Components.CheckerBoard { cellSide: 4 }
            //  alpha intensity gradient background
            Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#FF000000" }
                    GradientStop { position: 1.0; color: "#00000000" }
                }
            }
            Components.ColorSlider { id: alphaSlider; anchors.fill: parent }
        }

    }
}