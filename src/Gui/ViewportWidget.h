#pragma once

#include <QWidget>

class ViewCubeWidget;

// 中央 3D 视口（占位实现，后续替换为 OCCT AIS 渲染）
class ViewportWidget : public QWidget {
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    ViewCubeWidget* m_cube = nullptr;
};
