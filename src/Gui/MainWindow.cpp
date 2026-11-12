#include "Gui/MainWindow.h"

#include "Gui/ApplicationBar.h"
#include "Gui/BrowserPanel.h"
#include "Gui/PropertiesPanel.h"
#include "Gui/RibbonBar.h"
#include "Gui/ViewportWidget.h"

#include <QDockWidget>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("CXCAM — 计算机辅助制造");
    resize(1440, 900);

    buildAppBar();
    buildRibbon();
    buildDocks();
    buildStatusBar();
}

void MainWindow::buildAppBar() {
    setMenuWidget(new ApplicationBar(this));
}

void MainWindow::buildRibbon() {
    auto* host = new QToolBar("工具栏", this);
    host->setObjectName("ribbonHost");
    host->setMovable(false);
    host->setFloatable(false);
    host->addWidget(new RibbonBar(this));
    addToolBar(Qt::TopToolBarArea, host);
}

void MainWindow::buildDocks() {
    auto* browserDock = new QDockWidget("浏览器", this);
    browserDock->setObjectName("browserDock");
    browserDock->setWidget(new BrowserPanel(browserDock));
    browserDock->setFeatures(QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, browserDock);

    auto* propsDock = new QDockWidget("属性", this);
    propsDock->setObjectName("propsDock");
    propsDock->setWidget(new PropertiesPanel(propsDock));
    propsDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, propsDock);

    setCentralWidget(new ViewportWidget(this));

    resizeDocks({browserDock}, {260}, Qt::Horizontal);
    resizeDocks({propsDock}, {240}, Qt::Horizontal);
}

void MainWindow::buildStatusBar() {
    auto* sb = statusBar();
    sb->showMessage("就绪");

    auto* coord = new QLabel("X: 0.000    Y: 0.000    Z: 0.000");
    auto* unit = new QLabel("单位: mm");
    sb->addPermanentWidget(coord);
    sb->addPermanentWidget(unit);
}
