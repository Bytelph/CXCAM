#pragma once

#include <QWidget>

// 右侧属性面板（Fusion 的 Properties / Data panel）
class PropertiesPanel : public QWidget {
    Q_OBJECT
public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
};
