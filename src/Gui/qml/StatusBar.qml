import QtQuick
import CXCAM.Theme 1.0

// 底部状态栏
Rectangle {
    id: bar
    height: 24
    color: Theme.bgPanelAlt
    border.color: Theme.border

    Text {
        text: "就绪"
        color: Theme.textDim
        font.pixelSize: 11
        font.family: Theme.fontFamily
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: unitText
        text: "单位: mm"
        color: Theme.textDim
        font.pixelSize: 11
        font.family: Theme.fontFamily
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: "X: 0.000    Y: 0.000    Z: 0.000"
        color: Theme.textDim
        font.pixelSize: 11
        font.family: Theme.fontFamily
        anchors.right: unitText.left
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }
}
