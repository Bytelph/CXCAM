import QtQuick
import CXCAM.Theme 1.0

// ribbon 工具组：一排按钮在上、分类标题在下，左侧可选分割线
Item {
    id: group
    property string title: ""
    property var actions: []
    property bool showDivider: false

    implicitWidth: contentColumn.implicitWidth + (group.showDivider ? 13 : 0)

    // 组间分割线
    Rectangle {
        id: divider
        width: group.showDivider ? 1 : 0
        color: Theme.borderLight
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 8
        anchors.bottomMargin: 8
    }

    Column {
        id: contentColumn
        spacing: 8
        anchors.left: divider.right
        anchors.leftMargin: group.showDivider ? 12 : 0
        anchors.verticalCenter: parent.verticalCenter

        // 按钮行（上）
        Row {
            id: buttonsRow
            spacing: 2
            Repeater {
                model: group.actions
                RibbonButton { text: modelData }
            }
        }

        // 分类标题（下，居中于按钮行）
        Text {
            text: group.title
            width: buttonsRow.width
            horizontalAlignment: Text.AlignHCenter
            color: Theme.textMuted
            font.pixelSize: 10
            font.family: Theme.fontFamily
        }
    }
}
