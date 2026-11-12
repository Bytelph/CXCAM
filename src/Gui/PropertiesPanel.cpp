#include "Gui/PropertiesPanel.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

PropertiesPanel::PropertiesPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({"属性", "值"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 示例属性行（后续绑定到选中对象）
    auto addRow = [&](const QString& k, const QString& v) {
        const int r = table->rowCount();
        table->insertRow(r);
        table->setItem(r, 0, new QTableWidgetItem(k));
        table->setItem(r, 1, new QTableWidgetItem(v));
    };
    addRow("名称", "Body");
    addRow("类型", "实体");
    addRow("材料", "铝合金 6061");
    addRow("单位", "毫米 (mm)");

    layout->addWidget(table);
}
