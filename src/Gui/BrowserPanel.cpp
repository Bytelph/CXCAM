#include "Gui/BrowserPanel.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

BrowserPanel::BrowserPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tree = new QTreeWidget;
    m_tree->setHeaderHidden(true);
    m_tree->setColumnCount(1);
    m_tree->setAnimated(true);

    auto* doc = new QTreeWidgetItem(m_tree, {"文档"});
    doc->setExpanded(true);

    auto* part = new QTreeWidgetItem(doc, {"零件"});
    part->setExpanded(true);
    new QTreeWidgetItem(part, {"Body"});
    new QTreeWidgetItem(part, {"毛坯 (Stock)"});

    auto* setup = new QTreeWidgetItem(doc, {"设置 (Setup)"});
    setup->setExpanded(true);
    new QTreeWidgetItem(setup, {"刀具库"});
    new QTreeWidgetItem(setup, {"工序 (Operations)"});

    auto* results = new QTreeWidgetItem(doc, {"结果"});
    results->setExpanded(true);
    new QTreeWidgetItem(results, {"仿真结果"});
    new QTreeWidgetItem(results, {"测量报告"});

    layout->addWidget(m_tree);
}
