#pragma once

#include <QWidget>

// 视口右上角导航立方体（装饰性占位，后续接视角导航）
class ViewCubeWidget : public QWidget {
    Q_OBJECT
public:
    explicit ViewCubeWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override { return {96, 96}; }
};
