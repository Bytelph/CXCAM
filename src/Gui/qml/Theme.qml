pragma Singleton
import QtQuick

// 主题色板：跟随系统浅色/深色（无法判定时默认深色）。
// 3D 视口画布（Viewport/ViewCube）保持深色不随主题，与 Fusion 360 一致。
QtObject {
    readonly property bool dark: (typeof SystemTheme !== "undefined") ? SystemTheme.dark : true

    // 背景层次
    readonly property color bg:            dark ? "#1e1e1e" : "#fafafa"   // 主背景
    readonly property color bgPanel:       dark ? "#262626" : "#f0f0f0"   // 工具栏
    readonly property color bgPanelAlt:    dark ? "#2b2b2b" : "#e8e8e8"   // 面板/标题栏
    readonly property color bgHover:       dark ? "#2f2f2f" : "#e2e2e2"   // 悬停
    readonly property color bgElevated:    dark ? "#3c3c3c" : "#d4d4d4"   // 强调悬停
    readonly property color bgPressed:     dark ? "#4a4a4a" : "#c8c8c8"

    // 边框
    readonly property color border:        dark ? "#141414" : "#d0d0d0"
    readonly property color borderLight:   dark ? "#3c3c3c" : "#c8c8c8"

    // 文字
    readonly property color text:          dark ? "#e0e0e0" : "#2a2a2a"
    readonly property color textDim:       dark ? "#c8c8c8" : "#3a3a3a"
    readonly property color textMuted:     dark ? "#888888" : "#777777"
    readonly property color textBright:    dark ? "#ffffff" : "#000000"

    // 强调色
    readonly property color accent:        dark ? "#f59e0b" : "#d97706"   // 琥珀色
    readonly property color selection:     dark ? "#3a5f8a" : "#b8d4f0"   // 选中
    readonly property color closeHover:    "#e81123"   // 关闭按钮悬停

    // 字体
    readonly property string fontFamily: "Microsoft YaHei"   // 微软雅黑
}
