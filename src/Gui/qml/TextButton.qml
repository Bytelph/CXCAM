import QtQuick
import CXCAM.Theme 1.0

// 文字型按钮（标题栏/工具栏通用）
Rectangle {
    id: btn
    property string text: ""
    signal clicked()

    property color fg: Theme.textDim
    property color fgHover: Theme.textBright
    property color bgHover: Theme.bgElevated

    implicitWidth: label.implicitWidth + 20
    implicitHeight: 26
    radius: 3
    color: mouse.containsMouse ? bgHover : "transparent"

    Text {
        id: label
        anchors.centerIn: parent
        text: btn.text
        color: mouse.containsMouse ? fgHover : fg
        font.pixelSize: 12
        font.family: Theme.fontFamily
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: btn.clicked()
    }
}
