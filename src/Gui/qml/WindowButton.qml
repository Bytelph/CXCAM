import QtQuick
import CXCAM.Theme 1.0

// 标题栏窗口控制按钮：最小化 / 最大化 / 还原 / 关闭
Rectangle {
    id: btn
    property string type: "min"   // min | max | restore | close
    signal clicked()

    width: 46
    height: 40
    color: mouse.containsMouse
           ? (type === "close" ? Theme.closeHover : Theme.bgElevated)
           : "transparent"

    Canvas {
        id: icon
        anchors.centerIn: parent
        width: 12
        height: 12
        property color fg: (btn.type === "close" && mouse.containsMouse) ? "#ffffff" : Theme.textDim
        onFgChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = fg
            ctx.fillStyle = fg
            ctx.lineWidth = 1.1
            ctx.lineCap = "round"

            if (btn.type === "min") {
                ctx.beginPath()
                ctx.moveTo(2, 6)
                ctx.lineTo(10, 6)
                ctx.stroke()
            } else if (btn.type === "max") {
                ctx.strokeRect(2, 2, 8, 8)
            } else if (btn.type === "restore") {
                ctx.strokeRect(3.5, 1.5, 7, 7)
                ctx.strokeRect(1.5, 3.5, 7, 7)
            } else if (btn.type === "close") {
                ctx.beginPath()
                ctx.moveTo(2, 2)
                ctx.lineTo(10, 10)
                ctx.moveTo(10, 2)
                ctx.lineTo(2, 10)
                ctx.stroke()
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: btn.clicked()
    }
}
