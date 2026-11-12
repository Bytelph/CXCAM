#include "Gui/Style.h"

namespace Style {

QString darkTheme() {
    return QStringLiteral(R"qss(
QWidget {
    background-color: #1e1e1e;
    color: #e0e0e0;
    font-size: 12px;
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
}
QMainWindow, QDialog {
    background-color: #1e1e1e;
}

#ApplicationBar {
    background-color: #2b2b2b;
    border-bottom: 1px solid #141414;
}
#ApplicationBar QLabel#appLogo {
    color: #ffffff;
    font-weight: 600;
    font-size: 13px;
}
#ApplicationBar QToolButton {
    background: transparent;
    border: none;
    padding: 4px 10px;
    color: #c8c8c8;
}
#ApplicationBar QToolButton:hover {
    background: #3c3c3c;
    color: #ffffff;
}
#ApplicationBar QToolButton#docTab {
    background: #1e1e1e;
    color: #ffffff;
    border: 1px solid #3c3c3c;
    border-bottom: none;
    padding: 4px 14px;
}

QToolBar {
    background: #262626;
    border: none;
    spacing: 2px;
}

QTabBar::tab {
    background: transparent;
    color: #c8c8c8;
    padding: 8px 20px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected {
    color: #ffffff;
    border-bottom: 2px solid #f59e0b;
}
QTabBar::tab:hover {
    color: #ffffff;
}

#RibbonBar QToolButton {
    background: transparent;
    border: none;
    border-radius: 4px;
    color: #d0d0d0;
    padding: 6px 12px;
}
#RibbonBar QToolButton:hover {
    background: #3c3c3c;
    color: #ffffff;
}
#RibbonBar QToolButton:pressed {
    background: #4a4a4a;
}

QDockWidget {
    color: #e0e0e0;
    font-weight: 600;
}
QDockWidget::title {
    background: #2b2b2b;
    padding: 6px 8px;
    border-bottom: 1px solid #141414;
    text-align: left;
}

QTreeWidget {
    background-color: #1e1e1e;
    border: none;
    outline: none;
}
QTreeWidget::item {
    padding: 3px 2px;
}
QTreeWidget::item:hover {
    background: #2f2f2f;
}
QTreeWidget::item:selected {
    background: #3a5f8a;
    color: #ffffff;
}

QTableWidget {
    background: #1e1e1e;
    border: none;
    outline: none;
    gridline-color: #2a2a2a;
}
QHeaderView::section {
    background: #2b2b2b;
    color: #d0d0d0;
    border: none;
    padding: 4px 6px;
}

QStatusBar {
    background: #2b2b2b;
    border-top: 1px solid #141414;
    color: #c8c8c8;
}
QStatusBar QLabel {
    color: #c8c8c8;
}

QScrollBar:vertical {
    background: #1e1e1e;
    width: 10px;
}
QScrollBar::handle:vertical {
    background: #4a4a4a;
    border-radius: 5px;
    min-height: 24px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0;
}
QScrollBar:horizontal {
    background: #1e1e1e;
    height: 10px;
}
QScrollBar::handle:horizontal {
    background: #4a4a4a;
    border-radius: 5px;
    min-width: 24px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0;
}
)qss");
}

}  // namespace Style
