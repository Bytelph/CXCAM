#pragma once

#include <QQuickFramebufferObject>
#include <QPointF>

#include <memory>

class QMouseEvent;
class QWheelEvent;
class GraphicsViewportRenderer;

// 中央 3D 视口：以 QQuickFramebufferObject 承载 OpenCASCADE AIS 渲染
// OCCT 的驱动 / 场景 / 视图等对象全部封装在 GraphicsViewportRenderer（.cpp）中，
// 本头文件保持 Qt 友好、不引入 OCCT 头。
class GraphicsViewport : public QQuickFramebufferObject {
    Q_OBJECT
public:
    explicit GraphicsViewport(QQuickItem* parent = nullptr);
    ~GraphicsViewport() override;

    Renderer* createRenderer() const override;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    friend class GraphicsViewportRenderer;

    // GUI 线程累积的输入状态，renderer 在 synchronize() 中读取
    struct Input {
        Qt::MouseButtons buttons;
        QPointF pos;
        int wheelDelta = 0;
    };

    std::unique_ptr<Input> m_input;
};
