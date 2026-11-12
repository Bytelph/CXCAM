import QtQuick
import CXCAM.Theme 1.0

// ribbon 工具按钮
Rectangle {
    id: btn
    property string text: ""
    signal clicked()

    width: 60
    height: 30
    radius: 4
    color: mouse.containsMouse ? Theme.bgElevated : "transparent"

    Text {
        anchors.centerIn: parent
        text: btn.text
        color: mouse.containsMouse ? Theme.textBright : Theme.text
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
