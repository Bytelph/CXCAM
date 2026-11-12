import QtQuick
import CXCAM.Theme 1.0

// 中央 3D 视口：OCCT 可用时加载原生 AIS 视口，否则退回 Canvas 占位
Rectangle {
    id: viewport
    color: "#181b20"

    Loader {
        id: loader
        anchors.fill: parent
        // occAvailable 由 main.cpp 注入；避免无 OCCT 构建下解析 CXCAM.Graphics 模块失败
        sourceComponent: occAvailable ? occViewport : placeholder
    }

    Component {
        id: occViewport
        OccViewport { anchors.fill: parent }
    }

    Component {
        id: placeholder
        Canvas {
            id: canvas
            anchors.fill: parent
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d")
                var w = canvas.width
                var h = canvas.height

                // 深色渐变背景
                var g = ctx.createLinearGradient(0, 0, 0, h)
                g.addColorStop(0.0, "#2b3038")
                g.addColorStop(0.6, "#20242a")
                g.addColorStop(1.0, "#181b20")
                ctx.fillStyle = g
                ctx.fillRect(0, 0, w, h)

                // 网格
                ctx.strokeStyle = "#3a4048"
                ctx.lineWidth = 1
                ctx.beginPath()
                var step = 40
                for (var x = 0; x < w; x += step) { ctx.moveTo(x + 0.5, 0); ctx.lineTo(x + 0.5, h) }
                for (var y = 0; y < h; y += step) { ctx.moveTo(0, y + 0.5); ctx.lineTo(w, y + 0.5) }
                ctx.stroke()

                // 中心坐标轴（X 红 / Y 绿）
                var cx = w / 2
                var cy = h / 2
                ctx.lineWidth = 2
                ctx.strokeStyle = "#d04a4a"
                ctx.beginPath(); ctx.moveTo(cx - 40, cy); ctx.lineTo(cx + 40, cy); ctx.stroke()
                ctx.strokeStyle = "#4ab04a"
                ctx.beginPath(); ctx.moveTo(cx, cy - 40); ctx.lineTo(cx, cy + 40); ctx.stroke()

                // 标签
                ctx.fillStyle = "#ffffff"
                ctx.font = "12px 'Microsoft YaHei', sans-serif"
                ctx.fillText("X", cx + 44, cy + 4)
                ctx.fillText("Y", cx + 4, cy - 44)

                // 底部提示
                ctx.fillStyle = "#888888"
                ctx.font = "11px 'Microsoft YaHei', sans-serif"
                ctx.fillText("视口占位 — 后续接入 OCCT AIS 渲染", 12, h - 12)
            }
        }
    }

    // 右上角视图立方体 + 坐标系由 OCCT AIS_ViewCube 在 3D 视口内渲染，
    // 不再使用 QML 占位组件。
}
