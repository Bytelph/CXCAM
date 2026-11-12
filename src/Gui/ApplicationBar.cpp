#include "Gui/ApplicationBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QToolButton>

ApplicationBar::ApplicationBar(QWidget* parent) : QWidget(parent) {
    setObjectName("ApplicationBar");
    setFixedHeight(40);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(4);

    auto* logo = new QLabel("CXCAM");
    logo->setObjectName("appLogo");
    layout->addWidget(logo);

    layout->addSpacing(16);

    auto* docTab = new QToolButton;
    docTab->setObjectName("docTab");
    docTab->setText("未命名.ccpj");
    docTab->setPopupMode(QToolButton::InstantPopup);  // 后续接最近文件菜单
    layout->addWidget(docTab);

    layout->addStretch();

    auto* undo = new QToolButton;
    undo->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    undo->setToolTip("撤销");
    layout->addWidget(undo);

    auto* redo = new QToolButton;
    redo->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    redo->setToolTip("重做");
    layout->addWidget(redo);

    auto* save = new QToolButton;
    save->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    save->setToolTip("保存");
    layout->addWidget(save);
}
