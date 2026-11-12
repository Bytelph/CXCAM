#pragma once

#include <QObject>
#include <QRect>

class QWindow;

// 无边框窗口的本地窗口行为桥：QML 通过它调用 startSystemMove / startSystemResize
// 等原生方法，实现 Windows 下的窗口拖动、边缘缩放吸附与最小化/最大化/关闭。
class WindowController : public QObject {
    Q_OBJECT
public:
    explicit WindowController(QObject* parent = nullptr);

    Q_INVOKABLE void startSystemMove(QWindow* window);
    Q_INVOKABLE void startSystemResize(QWindow* window, int edges);
    Q_INVOKABLE void minimize(QWindow* window);
    Q_INVOKABLE void toggleMaximize(QWindow* window);
    Q_INVOKABLE void close(QWindow* window);
    Q_INVOKABLE void applyWindowCorners(QWindow* window);

    // 最大化时向下拖拽：先把窗口恢复到鼠标下，之后持续跟随鼠标移动
    Q_INVOKABLE void restoreUnderCursor(QWindow* window, qreal grabRatioX, int grabLocalY);
    Q_INVOKABLE void followCursor(QWindow* window, qreal grabRatioX, int grabLocalY);

private:
    QRect m_normalGeometry;  // 最大化前的窗口几何，供恢复定位与跟随
};
