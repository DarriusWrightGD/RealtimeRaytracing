import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import "../QMLComponents" as Components

Grid {
    id: root
    property int cellSide: 5
    anchors.fill: parent
    rows: height/cellSide; columns: width/cellSide
    clip: true
    Repeater {
        model: root.columns*root.rows
        Rectangle {
            width: root.cellSide; height: root.cellSide
            color: (index%2 == 0) ? "gray" : "white"
        }
    }
}

	