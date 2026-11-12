import QtQuick
import CXCAM.Theme 1.0

// 可展开的树节点（无子节点时自动隐藏箭头）
Column {
    id: section
    property string title: ""
    property bool expanded: true
    property int level: 0
    default property alias content: contentColumn.data

    width: parent.width

    // 表头
    Item {
        width: parent.width
        height: 26
        Rectangle {
            anchors.fill: parent
            color: headerMouse.containsMouse ? "#d8d8d8" : "transparent"
        }
        Text {
            anchors.left: parent.left
            anchors.leftMargin: 8 + section.level * 14
            anchors.verticalCenter: parent.verticalCenter
            text: (contentColumn.children.length > 0
                   ? (section.expanded ? "▾ " : "▸ ")
                   : "") + section.title
            color: "#1a1a1a"   // 浅灰面板上的深色标题文字
            font.pixelSize: 12
            font.bold: true
            font.family: Theme.fontFamily
        }
        MouseArea {
            id: headerMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: section.expanded = !section.expanded
        }
    }

    Column {
        id: contentColumn
        width: parent.width
        visible: section.expanded
    }
}
