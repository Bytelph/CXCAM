#pragma once

#include <QWidget>

class QTreeWidget;

// 左侧浏览器树（Fusion 的 Browser：文档/零件/设置/结果）
class BrowserPanel : public QWidget {
    Q_OBJECT
public:
    explicit BrowserPanel(QWidget* parent = nullptr);

private:
    QTreeWidget* m_tree = nullptr;
};
