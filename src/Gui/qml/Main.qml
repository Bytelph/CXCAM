import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import CXCAM.Theme 1.0

ApplicationWindow {
    id: root
    width: 1440
    height: 900
    visible: true
    title: "CXCAM — 计算机辅助制造"
    color: Theme.bg

    // 无边框窗口
    flags: Qt.Window | Qt.FramelessWindowHint

    // Windows 11 圆角：普通状态圆角，最大化/全屏不圆角（Win10 无此 API 自动回退）
    onVisibleChanged: if (visible) WindowController.applyWindowCorners(root)
    onVisibilityChanged: WindowController.applyWindowCorners(root)

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Ribbon {
            Layout.fillWidth: true
            target: root
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // 3D 视口铺满中央区域（左右通铺，右侧属性面板已移除）
            Viewport {
                anchors.fill: parent
            }

            // 项目树：半透明悬浮在视口左侧（上下边距一致）
            ProjectPanel {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.topMargin: 12
                anchors.leftMargin: 12
                anchors.bottomMargin: 12
                width: 260
            }
        }

        StatusBar {
            Layout.fillWidth: true
        }
    }

    // —— 四周缩放把手（无边框窗口手动提供边缘拖拽缩放）——
    WindowResizeHandle { target: root; edges: 2;  cursorShape: Qt.SizeVerCursor;  anchors.top: parent.top;       anchors.left: parent.left;   anchors.right: parent.right;  height: 5; z: 100 }
    WindowResizeHandle { target: root; edges: 8;  cursorShape: Qt.SizeVerCursor;  anchors.bottom: parent.bottom; anchors.left: parent.left;   anchors.right: parent.right;  height: 5; z: 100 }
    WindowResizeHandle { target: root; edges: 1;  cursorShape: Qt.SizeHorCursor;  anchors.left: parent.left;     anchors.top: parent.top;      anchors.bottom: parent.bottom; width: 5;  z: 100 }
    WindowResizeHandle { target: root; edges: 4;  cursorShape: Qt.SizeHorCursor;  anchors.right: parent.right;   anchors.top: parent.top;      anchors.bottom: parent.bottom; width: 5;  z: 100 }
    WindowResizeHandle { target: root; edges: 3;  cursorShape: Qt.SizeFDiagCursor; anchors.top: parent.top;       anchors.left: parent.left;   width: 8; height: 8; z: 101 }
    WindowResizeHandle { target: root; edges: 6;  cursorShape: Qt.SizeBDiagCursor; anchors.top: parent.top;       anchors.right: parent.right;  width: 8; height: 8; z: 101 }
    WindowResizeHandle { target: root; edges: 9;  cursorShape: Qt.SizeBDiagCursor; anchors.bottom: parent.bottom; anchors.left: parent.left;   width: 8; height: 8; z: 101 }
    WindowResizeHandle { target: root; edges: 12; cursorShape: Qt.SizeFDiagCursor; anchors.bottom: parent.bottom; anchors.right: parent.right;  width: 8; height: 8; z: 101 }
}
