import QtQuick
import QtQuick.Window
import CXCAM.Theme 1.0

// 自定义标题栏：应用名 + 拖动区 + 窗口控制按钮（已移除文档标签与撤销/重做/保存）
Rectangle {
    id: bar
    property var target: null
    height: 40
    color: Theme.bgPanelAlt
    border.color: Theme.border

    // 拖动窗口 / 双击最大化
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: { if (bar.target) WindowController.startSystemMove(bar.target) }
        onDoubleClicked: { if (bar.target) WindowController.toggleMaximize(bar.target) }
    }

    Text {
        text: "CXCAM"
        color: Theme.textBright
        font.pixelSize: 13
        font.bold: true
        font.family: Theme.fontFamily
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
    }

    // 窗口控制按钮
    Row {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        WindowButton {
            type: "min"
            onClicked: WindowController.minimize(bar.target)
        }
        WindowButton {
            type: (bar.target && bar.target.visibility === Window.Maximized) ? "restore" : "max"
            onClicked: WindowController.toggleMaximize(bar.target)
        }
        WindowButton {
            type: "close"
            onClicked: WindowController.close(bar.target)
        }
    }
}
