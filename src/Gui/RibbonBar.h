#pragma once

#include <QWidget>
#include <QStringList>

class QTabBar;
class QStackedWidget;
class QHBoxLayout;
class QToolButton;

// 工作区切换工具栏（Fusion 的 ribbon：顶部标签 + 各组工具按钮）
class RibbonBar : public QWidget {
    Q_OBJECT
public:
    explicit RibbonBar(QWidget* parent = nullptr);

private:
    QWidget* buildModelWorkspace();
    QWidget* buildManufactureWorkspace();
    QWidget* buildSimulateWorkspace();
    QWidget* buildMeasureWorkspace();

    QToolButton* makeAction(const QString& text);
    void addGroup(QHBoxLayout* row, const QString& title, const QStringList& actions);

    QTabBar* m_tabs = nullptr;
    QStackedWidget* m_stack = nullptr;
};
