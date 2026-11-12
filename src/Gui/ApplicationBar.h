#pragma once

#include <QWidget>

// 顶部应用栏：Logo + 文档标签 + 撤销/重做/保存
class ApplicationBar : public QWidget {
    Q_OBJECT
public:
    explicit ApplicationBar(QWidget* parent = nullptr);
};
