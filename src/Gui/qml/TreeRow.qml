import QtQuick
import CXCAM.Theme 1.0

// 树叶子节点
Item {
    id: row
    property string label: ""
    property int level: 0

    width: parent.width
    height: 24

    Rectangle {
        anchors.fill: parent
        color: mouse.containsMouse ? "#d8d8d8" : "transparent"
    }
    Text {
        anchors.left: parent.left
        anchors.leftMargin: 8 + row.level * 14 + 12
        anchors.verticalCenter: parent.verticalCenter
        text: row.label
        color: "#333333"   // 浅灰面板上的深色正文文字
        font.pixelSize: 12
        font.family: Theme.fontFamily
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }
}
