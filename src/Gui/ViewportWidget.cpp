#include "Gui/ViewportWidget.h"
#include "Gui/ViewCubeWidget.h"

#include <QPainter>
#include <QResizeEvent>

ViewportWidget::ViewportWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("Viewport");
    setMinimumSize(400, 300);

    m_cube = new ViewCubeWidget(this);
    m_cube->show();
}

void ViewportWidget::resizeEvent(QResizeEvent*) {
    m_cube->move(width() - m_cube->width() - 12, 12);
}

void ViewportWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Fusion 风格深色渐变背景
    QLinearGradient g(0, 0, 0, height());
    g.setColorAt(0.0, QColor(0x2b, 0x30, 0x38));
    g.setColorAt(0.6, QColor(0x20, 0x24, 0x2a));
    g.setColorAt(1.0, QColor(0x18, 0x1b, 0x20));
    p.fillRect(rect(), g);

    // 网格
    p.setPen(QColor(0x3a, 0x40, 0x48));
    const int step = 40;
    for (int x = 0; x < width(); x += step) {
        p.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += step) {
        p.drawLine(0, y, width(), y);
    }

    // 中心坐标轴示意（X 红 / Y 绿）
    const int cx = width() / 2;
    const int cy = height() / 2;
    p.setPen(QPen(QColor(0xd0, 0x4a, 0x4a), 2));
    p.drawLine(cx - 40, cy, cx + 40, cy);
    p.setPen(QPen(QColor(0x4a, 0xb0, 0x4a), 2));
    p.drawLine(cx, cy - 40, cx, cy + 40);
    p.setPen(Qt::white);
    p.drawText(cx + 44, cy + 4, "X");
    p.drawText(cx + 4, cy - 44, "Y");

    // 底部提示
    p.setPen(QColor(0x88, 0x88, 0x88));
    p.drawText(12, height() - 12, "视口占位 — 后续接入 OCCT AIS 渲染");
}
