import QtQuick
import CXCAM.Theme 1.0

// 左侧项目树：浅灰底板悬浮在 3D 视口之上。
// 底板随悬停淡入淡出（悬停 50% / 离开 0），文字始终 100% 不透明，保持清晰可读。
Rectangle {
    id: panel
    color: "transparent"   // 仅作整体命中区域（空白处也能触发悬停），本身不显示

    // 底板：浅灰 + 圆角边框，随悬停淡入淡出
    Rectangle {
        anchors.fill: parent
        color: "#e8e8e8"
        radius: 6
        border.color: Qt.rgba(0, 0, 0, 0.12)
        border.width: 1
        opacity: panelHover.hovered ? 0.5 : 0.0
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    // 内容层：文字不透明度恒为 1，不受底板透明度影响
    Column {
        anchors.fill: parent
        anchors.margins: 4

        // 面板标题
        Text {
            width: parent.width
            height: 28
            text: "项目"
            color: "#1a1a1a"
            font.pixelSize: 12
            font.bold: true
            font.family: Theme.fontFamily
            verticalAlignment: Text.AlignVCenter
            leftPadding: 10
        }

        Column {
            width: parent.width
            anchors.topMargin: 4

            TreeSection {
                title: "项目"
                expanded: true

                TreeSection {
                    title: "零件"
                    level: 1
                    expanded: true
                    TreeRow { label: "Body"; level: 2 }
                    TreeRow { label: "毛坯 (Stock)"; level: 2 }
                }
                TreeSection {
                    title: "设置 (Setup)"
                    level: 1
                    expanded: true
                    TreeRow { label: "刀具库"; level: 2 }
                    TreeRow { label: "工序 (Operations)"; level: 2 }
                }
                TreeSection {
                    title: "结果"
                    level: 1
                    expanded: true
                    TreeRow { label: "仿真结果"; level: 2 }
                    TreeRow { label: "测量报告"; level: 2 }
                }
            }
        }
    }

    HoverHandler {
        id: panelHover
    }
}
