#include "Gui/SystemTheme.h"

#include <QGuiApplication>
#include <QStyleHints>

SystemTheme::SystemTheme(QObject* parent) : QObject(parent) {
    applyScheme();
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &SystemTheme::applyScheme);
}

void SystemTheme::applyScheme() {
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    // Unknown（无法判定）时回退到深色，保持原有设计
    const bool dark = (scheme == Qt::ColorScheme::Dark ||
                       scheme == Qt::ColorScheme::Unknown);
    if (dark != m_dark) {
        m_dark = dark;
        emit darkChanged();
    }
}
