#pragma once

#include <QObject>

// 暴露系统主题（浅色/深色）给 QML。
// Windows 下通过 QStyleHints::colorScheme() 读取 AppsUseLightTheme 注册表项，
// 并随系统切换实时更新。
class SystemTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool dark READ isDark NOTIFY darkChanged)
public:
    explicit SystemTheme(QObject* parent = nullptr);

    bool isDark() const { return m_dark; }

signals:
    void darkChanged();

private:
    void applyScheme();

    bool m_dark = true;
};
