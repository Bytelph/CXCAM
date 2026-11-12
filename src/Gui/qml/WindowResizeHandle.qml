import QtQuick
import QtQuick.Window

// 无边框窗口边缘缩放把手。edges 为 Qt::Edges 位组合：
// Left=1 / Top=2 / Right=4 / Bottom=8（角=两方向之和）
MouseArea {
    id: handle
    property var target: null
    property int edges: 0

    acceptedButtons: Qt.LeftButton
    hoverEnabled: true
    visible: target && target.visibility !== Window.Maximized

    onPressed: {
        if (target)
            WindowController.startSystemResize(target, edges)
    }
}
