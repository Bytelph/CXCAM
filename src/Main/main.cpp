#include <QGuiApplication>
#include <QFont>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "Gui/SystemTheme.h"
#include "Gui/WindowController.h"

#ifdef CXCAM_HAS_OCCT
#  include "Gui/GraphicsViewport.h"
#  include "Gui/qtopengl_utils.h"
#endif

int main(int argc, char** argv) {
#ifdef CXCAM_HAS_OCCT
    // 必须在 QGuiApplication 之前：
    //   * 强制桌面 OpenGL（OCCT TKOpenGl 不支持 ANGLE/GLES）
    //   * 开启 GL 上下文共享，并设置默认 QSurfaceFormat
    Mayo::QtOpenGlUtils::platformSetup(argc, argv);
#endif

    QGuiApplication app(argc, argv);
    app.setApplicationName("CXCAM");
    app.setOrganizationName("CXCAM");
    app.setApplicationVersion("0.1.0");

    // 全系字体：微软雅黑
    app.setFont(QFont(QStringLiteral("Microsoft YaHei")));

    // 无边框窗口控制桥（拖动 / 缩放 / 最小化 / 最大化 / 关闭）
    auto* windowController = new WindowController(&app);

    // 系统主题（浅色/深色）检测，暴露给 QML
    auto* systemTheme = new SystemTheme(&app);

    // 深色主题单例（QML 中以 Theme.xxx 访问）
    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/qml/Theme.qml")),
                             "CXCAM.Theme", 1, 0, "Theme");

#ifdef CXCAM_HAS_OCCT
    // OCCT AIS 视口（QQuickFramebufferObject）
    qmlRegisterType<GraphicsViewport>("CXCAM.Graphics", 1, 0, "GraphicsViewport");
#endif

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("WindowController", windowController);
    engine.rootContext()->setContextProperty("SystemTheme", systemTheme);
#ifdef CXCAM_HAS_OCCT
    engine.rootContext()->setContextProperty("occAvailable", true);
#else
    engine.rootContext()->setContextProperty("occAvailable", false);
#endif

    // qml.qrc 位于 cxcam_gui 静态库中，需显式初始化资源
    Q_INIT_RESOURCE(qml);

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](QObject* obj, const QUrl& objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
