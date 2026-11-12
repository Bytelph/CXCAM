import QtQuick
import QtQuick.Controls
import QtQuick.Window
import CXCAM.Theme 1.0

// 工作区 ribbon：顶行 = 应用名 + 标签 + 拖动区 + 窗口控制按钮；下行 = 当前标签的工具组。
// 标签与内容均由下方 tabs 数组驱动，新增标签只需在数组里加一项即可自动扩展。
Rectangle {
    id: ribbon
    property var target: null
    height: 160   // 顶行 40 + 内容区 120
    color: Theme.bgPanel
    border.color: Theme.border

    // —— 可扩展标签数据：title = 标签名，groups = 工具组列表 ——
    property var tabs: [
        { title: "文件", groups: [
            { name: "文件操作", actions: ["新建", "打开", "保存", "另存为"] },
            { name: "数据交换", actions: ["导入", "导出", "CLData"] }
        ]},
        { title: "视图", groups: [
            { name: "视角", actions: ["适应", "平移", "旋转", "缩放"] },
            { name: "显示", actions: ["网格", "坐标系", "阴影", "线框"] },
            { name: "导航", actions: ["前视", "俯视", "侧视", "等轴测"] }
        ]},
        { title: "AICAM", groups: [
            { name: "工序", actions: ["新建工序", "复制", "粘贴", "删除"] },
            { name: "2.5D", actions: ["面铣", "型腔", "轮廓", "钻孔"] },
            { name: "3D", actions: ["平行", "等高", "环绕", "清根"] }
        ]},
        { title: "PROBE", groups: [
            { name: "测点", actions: ["单点", "平面", "圆", "球"] },
            { name: "找正", actions: ["3-2-1 找正", "基准"] },
            { name: "输出", actions: ["测量程序", "报告"] }
        ]},
        { title: "SIM", groups: [
            { name: "几何仿真", actions: ["开始", "暂停", "重置", "余量分析"] },
            { name: "物理仿真", actions: ["切削力", "变形", "温度"] },
            { name: "检查", actions: ["过切", "碰撞"] }
        ]},
        { title: "配置", groups: [
            { name: "系统", actions: ["单位", "语言", "路径"] },
            { name: "库", actions: ["刀具库", "机床库", "后处理"] },
            { name: "参数", actions: ["默认参数", "公差"] }
        ]},
        { title: "帮助", groups: [
            { name: "帮助", actions: ["文档", "快捷键", "关于"] },
            { name: "在线", actions: ["官网", "更新", "反馈"] }
        ]}
    ]

    Column {
        anchors.fill: parent
        spacing: 0

        // —— 顶行：应用名 + 标签 + 拖动区 + 窗口控制按钮 ——
        Item {
            id: topRow
            width: parent.width
            height: 40

            Rectangle {
                anchors.fill: parent
                color: Theme.bgPanelAlt
            }

            // 拖动窗口 / 双击最大化。
            // - 非最大化：原生标题栏拖动（含 Aero 吸附）。
            // - 最大化：向下拖 → 恢复窗口并跟随鼠标移动。
            // （位于标签与按钮之后，只接管空白处）
            MouseArea {
                id: topDrag
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton

                property bool dragging: false
                property bool restoring: false
                property real pressX: 0
                property real pressY: 0
                property real grabRatioX: 0.5
                property int grabLocalY: 20

                onPressed: (mouse) => {
                    dragging = false
                    restoring = false
                    pressX = mouse.x
                    pressY = mouse.y
                    if (ribbon.target && ribbon.target.visibility === Window.Maximized) {
                        grabRatioX = ribbon.target.width > 0 ? mouse.x / ribbon.target.width : 0.5
                        grabLocalY = Math.round(mouse.y)
                    }
                }

                onPositionChanged: (mouse) => {
                    if (!ribbon.target) return
                    if (restoring) {
                        WindowController.followCursor(ribbon.target, grabRatioX, grabLocalY)
                        return
                    }
                    if (dragging) return

                    if (ribbon.target.visibility === Window.Maximized) {
                        // 最大化：向下拖 → 恢复并跟随鼠标
                        if (mouse.y - pressY > 12) {
                            dragging = true
                            restoring = true
                            WindowController.restoreUnderCursor(ribbon.target, grabRatioX, grabLocalY)
                        }
                    } else if (Math.abs(mouse.x - pressX) + Math.abs(mouse.y - pressY) > 6) {
                        // 非最大化：原生标题栏拖动
                        dragging = true
                        WindowController.startSystemMove(ribbon.target)
                    }
                }

                onReleased: { dragging = false; restoring = false }
                onCanceled: { dragging = false; restoring = false }

                onDoubleClicked: if (ribbon.target) WindowController.toggleMaximize(ribbon.target)
            }

            Text {
                id: logo
                text: "CXCAM"
                color: Theme.textBright
                font.pixelSize: 13
                font.bold: true
                font.family: Theme.fontFamily
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
            }

            Row {
                id: tabs
                property int currentIndex: 0
                anchors.left: logo.right
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                Repeater {
                    model: ribbon.tabs
                    Rectangle {
                        width: tabText.implicitWidth + 28
                        height: 40
                        radius: 4
                        color: index === tabs.currentIndex ? Theme.bg : (tabMouse.containsMouse ? Theme.bgElevated : "transparent")
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Text {
                            id: tabText
                            text: modelData.title
                            color: index === tabs.currentIndex ? Theme.textBright : (tabMouse.containsMouse ? Theme.textBright : Theme.textDim)
                            font.pixelSize: 14
                            font.bold: true
                            font.family: Theme.fontFamily
                            anchors.centerIn: parent
                        }

                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: index === tabs.currentIndex ? parent.width : (tabMouse.containsMouse ? parent.width * 0.5 : 0)
                            height: 2
                            color: index === tabs.currentIndex ? Theme.accent : (tabMouse.containsMouse ? Theme.textMuted : "transparent")
                            Behavior on width { NumberAnimation { duration: 120 } }
                            Behavior on color { ColorAnimation { duration: 120 } }
                        }

                        MouseArea {
                            id: tabMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: tabs.currentIndex = index
                        }
                    }
                }
            }

            // 窗口控制按钮
            Row {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                WindowButton {
                    type: "min"
                    onClicked: WindowController.minimize(ribbon.target)
                }
                WindowButton {
                    type: (ribbon.target && ribbon.target.visibility === Window.Maximized) ? "restore" : "max"
                    onClicked: WindowController.toggleMaximize(ribbon.target)
                }
                WindowButton {
                    type: "close"
                    onClicked: WindowController.close(ribbon.target)
                }
            }
        }

        // —— 内容行：当前标签的工具组（淡入淡出切换）——
        Item {
            id: content
            width: parent.width
            height: 120

            Repeater {
                model: ribbon.tabs
                RibbonPage {
                    current: index === tabs.currentIndex
                    groups: modelData.groups
                }
            }
        }
    }
}
