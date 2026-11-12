#include "Gui/RibbonBar.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

RibbonBar::RibbonBar(QWidget* parent) : QWidget(parent) {
    setObjectName("RibbonBar");

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_tabs = new QTabBar;
    m_tabs->setExpanding(false);
    m_tabs->addTab("建模");
    m_tabs->addTab("加工");
    m_tabs->addTab("仿真");
    m_tabs->addTab("测量");

    m_stack = new QStackedWidget;
    m_stack->addWidget(buildModelWorkspace());
    m_stack->addWidget(buildManufactureWorkspace());
    m_stack->addWidget(buildSimulateWorkspace());
    m_stack->addWidget(buildMeasureWorkspace());

    connect(m_tabs, &QTabBar::currentChanged, m_stack, &QStackedWidget::setCurrentIndex);

    root->addWidget(m_tabs);
    root->addWidget(m_stack);
}

QToolButton* RibbonBar::makeAction(const QString& text) {
    auto* b = new QToolButton;
    b->setText(text);
    b->setToolButtonStyle(Qt::ToolButtonTextOnly);
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

void RibbonBar::addGroup(QHBoxLayout* row, const QString& title, const QStringList& actions) {
    auto* group = new QVBoxLayout;
    group->setSpacing(2);

    auto* label = new QLabel(title);
    label->setStyleSheet("color:#888888; font-size:10px;");
    label->setAlignment(Qt::AlignHCenter);
    group->addWidget(label);

    auto* buttons = new QHBoxLayout;
    buttons->setSpacing(2);
    for (const QString& a : actions) {
        buttons->addWidget(makeAction(a));
    }
    group->addLayout(buttons);

    row->addLayout(group);

    auto* sep = new QFrame;
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("color:#3c3c3c;");
    row->addWidget(sep);
}

QWidget* RibbonBar::buildModelWorkspace() {
    auto* w = new QWidget;
    auto* row = new QHBoxLayout(w);
    row->setContentsMargins(8, 4, 8, 4);
    row->setSpacing(4);

    addGroup(row, "草图", {"直线", "矩形", "圆", "样条"});
    addGroup(row, "实体", {"拉伸", "旋转", "孔", "倒角"});
    addGroup(row, "布尔", {"合并", "相减", "相交"});
    row->addStretch();
    return w;
}

QWidget* RibbonBar::buildManufactureWorkspace() {
    auto* w = new QWidget;
    auto* row = new QHBoxLayout(w);
    row->setContentsMargins(8, 4, 8, 4);
    row->setSpacing(4);

    addGroup(row, "2.5D", {"面铣", "型腔", "轮廓", "钻孔"});
    addGroup(row, "3D", {"平行", "等高", "环绕", "清根"});
    addGroup(row, "验证", {"仿真", "过切检查"});
    row->addStretch();
    return w;
}

QWidget* RibbonBar::buildSimulateWorkspace() {
    auto* w = new QWidget;
    auto* row = new QHBoxLayout(w);
    row->setContentsMargins(8, 4, 8, 4);
    row->setSpacing(4);

    addGroup(row, "几何仿真", {"开始", "暂停", "重置", "余量分析"});
    addGroup(row, "物理仿真", {"切削力", "变形"});
    row->addStretch();
    return w;
}

QWidget* RibbonBar::buildMeasureWorkspace() {
    auto* w = new QWidget;
    auto* row = new QHBoxLayout(w);
    row->setContentsMargins(8, 4, 8, 4);
    row->setSpacing(4);

    addGroup(row, "测头", {"单点", "平面", "圆孔", "找正"});
    addGroup(row, "结果", {"报告", "补偿"});
    row->addStretch();
    return w;
}
