import QtQuick
import CXCAM.Graphics 1.0

// 原生 OCCT AIS 视口（QQuickFramebufferObject 承载）
// 仅在 CXCAM_WITH_OCCT=ON 时由 Viewport.qml 动态加载，避免无 OCCT 构建下解析失败
GraphicsViewport {
    anchors.fill: parent
}
