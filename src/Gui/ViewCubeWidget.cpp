#include "Gui/ViewCubeWidget.h"

#include <QPainter>
#include <QPolygonF>

ViewCubeWidget::ViewCubeWidget(QWidget* parent) : QWidget(parent) {
    setFixedSize(96, 96);
    setAttribute(Qt::WA_TransparentForMouseEvents);  // 装饰性占位
}

void ViewCubeWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 半透明面板底
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(30, 30, 30, 160));
    p.drawRoundedRect(rect(), 8, 8);

    p.translate(width() / 2.0, height() / 2.0);
    const qreal r = 22.0;

    const QPointF c(0, 0);
    const QPointF t(0, -r);
    const QPointF ur(0.87 * r, -0.5 * r);
    const QPointF lr(0.87 * r, 0.5 * r);
    const QPointF b(0, r);
    const QPointF ll(-0.87 * r, 0.5 * r);
    const QPointF ul(-0.87 * r, -0.5 * r);

    // 三个可见面（顶 / 右 / 左）
    p.setPen(QPen(QColor(0x141414), 1));
    p.setBrush(QColor(0x6fa8dc));
    p.drawPolygon(QPolygonF() << t << ur << c << ul);
    p.setBrush(QColor(0x3f74a3));
    p.drawPolygon(QPolygonF() << ur << lr << c);
    p.setBrush(QColor(0x2f5c85));
    p.drawPolygon(QPolygonF() << ul << c << ll);

    // 轮廓
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(0x0d0d0d), 1));
    p.drawPolygon(QPolygonF() << t << ur << lr << b << ll << ul << t);

    // 顶部标签
    p.setPen(QColor(0xe0e0e0));
    QFont f = p.font();
    f.setPixelSize(9);
    p.setFont(f);
    p.drawText(QRectF(-14, -r - 14, 28, 12), Qt::AlignCenter, "顶");
}
