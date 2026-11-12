#include "Gui/WindowController.h"

#include <QCursor>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

WindowController::WindowController(QObject* parent) : QObject(parent) {}

void WindowController::startSystemMove(QWindow* window) {
    if (!window) return;
#ifdef Q_OS_WIN
    // 原生标题栏拖动（非最大化状态下使用，含 Aero 吸附）。
    ReleaseCapture();
    SendMessage(reinterpret_cast<HWND>(window->winId()), WM_NCLBUTTONDOWN, HTCAPTION, 0);
#else
    window->startSystemMove();
#endif
}

void WindowController::startSystemResize(QWindow* window, int edges) {
    // edges 为 Qt::Edges 的位组合（Left=1 / Top=2 / Right=4 / Bottom=8）
    if (window) window->startSystemResize(static_cast<Qt::Edges>(edges));
}

void WindowController::minimize(QWindow* window) {
    if (window) window->showMinimized();
}

void WindowController::toggleMaximize(QWindow* window) {
    if (!window) return;
    if (window->visibility() == QWindow::Maximized)
        window->showNormal();
    else {
        m_normalGeometry = window->geometry();  // 记录最大化前的几何
        window->showMaximized();
    }
}

void WindowController::restoreUnderCursor(QWindow* window, qreal grabRatioX, int grabLocalY) {
    if (!window) return;
    if (window->visibility() == QWindow::Maximized)
        window->showNormal();
    followCursor(window, grabRatioX, grabLocalY);
}

void WindowController::followCursor(QWindow* window, qreal grabRatioX, int grabLocalY) {
    if (!window) return;
    // 用恢复后的窗口宽度计算横向抓取偏移，使标题栏上抓取点始终落在鼠标下方
    const int w = m_normalGeometry.width() > 0 ? m_normalGeometry.width() : window->width();
    const int offsetX = int(grabRatioX * w);
    const QPoint cursor = QCursor::pos();
    window->setPosition(cursor.x() - offsetX, cursor.y() - grabLocalY);
}

void WindowController::close(QWindow* window) {
    if (window) window->close();
}

void WindowController::applyWindowCorners(QWindow* window) {
    if (!window) return;
#ifdef Q_OS_WIN
    // Windows 11（build 22000+）DWM 圆角；最大化/全屏时不圆角。
    // Win10 及更早没有该属性，DwmSetWindowAttribute 返回错误，忽略即可（本身无圆角）。
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#ifndef DWMWCP_DONOTROUND
#define DWMWCP_DONOTROUND 1
#endif
    const auto hwnd = reinterpret_cast<HWND>(window->winId());
    const auto vis = window->visibility();
    const int pref = (vis == QWindow::Maximized || vis == QWindow::FullScreen)
                         ? DWMWCP_DONOTROUND
                         : DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
#endif
}
