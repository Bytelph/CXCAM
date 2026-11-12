#pragma once

#include <QMainWindow>

// 主窗口：组装应用栏 / ribbon / 左右面板 / 中央视口 / 状态栏
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildAppBar();
    void buildRibbon();
    void buildDocks();
    void buildStatusBar();
};
