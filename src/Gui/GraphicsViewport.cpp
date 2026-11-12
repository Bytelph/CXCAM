#include "Gui/GraphicsViewport.h"
#include "Gui/qtopengl_utils.h"

#include "Graphics/base/occ_handle.h"
#include "Graphics/graphics_create_driver.h"
#include "Graphics/graphics_scene.h"
#include "Graphics/graphics_utils.h"
#include "Graphics/opengl_utils.h"

#include <AIS_Shape.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_GridType.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <NCollection_Vec2.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_Color.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_View.hxx>

#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QWheelEvent>

#include <cmath>

namespace {

// Qt Quick 场景图负责 FBO 与 buffer swap，故 OCCT 驱动关闭自有 swap / system buffer
Mayo::OccHandle<Graphic3d_GraphicDriver> createQmlGraphicsDriver()
{
    auto driver = Mayo::makeOccHandle<OpenGl_GraphicDriver>(
        Mayo::GraphicsUtils::AspectDisplayConnection_create(), false /*dontInit*/
    );
    driver->ChangeOptions().buffersNoSwap = true;
    driver->ChangeOptions().buffersOpaqueAlpha = true;
    driver->ChangeOptions().useSystemBuffer = false;

    // 匹配 Qt 默认的 CompatibilityProfile GL 上下文（否则 OCCT 按 Core Profile 处理，
    // 产生 GL_INVALID_ENUM 并导致后续 FBO 包装失败）
    Mayo::QtOpenGlUtils::setCapsFromSurfaceFormat(
        driver->ChangeOptions(), QSurfaceFormat::defaultFormat());

    const static bool isGpuAccel = Mayo::OpenGlUtils::isHardwareAccelerationAvailable();
    driver->ChangeOptions().contextNoAccel = !isGpuAccel;

    return driver;
}

} // namespace

// ---------------------------------------------------------------------------
// Renderer
// ---------------------------------------------------------------------------

class GraphicsViewportRenderer : public QQuickFramebufferObject::Renderer {
public:
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override
    {
        QOpenGLFramebufferObjectFormat fmt;
        fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_size = size;
        m_fbo = new QOpenGLFramebufferObject(size, fmt);
        return m_fbo;
    }

    void synchronize(QQuickFramebufferObject* item) override
    {
        auto* viewport = static_cast<GraphicsViewport*>(item);
        m_input = *viewport->m_input;
        viewport->m_input->wheelDelta = 0;   // 消费累计的滚轮增量
    }

    void render() override
    {
        if (m_initFailed)
            return;

        if (!m_scene)
            this->initOcct();

        if (m_view.IsNull())
            return;

        this->applyNavigation();

        // Qt 6 的 QQuickFramebufferObject 不会在 render() 前自动绑定 FBO（GL_FRAMEBUFFER_BINDING==0），
        // OCCT 的 initializeGlFramebufferObject 依赖当前已绑定的 FBO，因此这里显式绑定。
        if (m_fbo)
            m_fbo->bind();

        Mayo::QtOpenGlUtils::initializeGlFramebufferObject(m_view);
        Mayo::QtOpenGlUtils::resetGlStateBeforeOcct(m_view);
        m_view->Redraw();
        Mayo::QtOpenGlUtils::resetGlStateAfterOcct(m_view);

        if (m_fbo)
            m_fbo->release();
    }

private:
    void initOcct()
    {
        Mayo::setFunctionCreateGraphicsDriver(&createQmlGraphicsDriver);

        m_scene = std::make_unique<Mayo::GraphicsScene>();
        m_view = m_scene->createV3dView();

        // 用当前（场景图）GL 上下文包装 OCCT 视口
        if (!Mayo::OpenGlUtils::initializeGlWindow(
                m_view,
                static_cast<Aspect_Drawable>(0),
                NCollection_Vec2<int>(m_size.width(), m_size.height()),
                1.0))
        {
            m_initFailed = true;
            return;
        }

        m_view->SetBackgroundColor(Quantity_Color(1.0f, 1.0f, 1.0f, Quantity_TOC_RGB));

        // 网格（XY 平面矩形网格，原点 0,0，步长 10，线框显示）
        auto viewer = m_scene->v3dViewer();
        viewer->SetRectangularGridValues(0., 0., 10., 10., 0.);
        viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);

        // 初始演示几何：一个立方体，验证 AIS 渲染链路
        this->setShape(BRepPrimAPI_MakeBox(100., 60., 30.).Shape());

        // 右上角视图立方体 + 坐标轴（参考 Mayo，使用 OCCT AIS_ViewCube）
        this->addViewCube();

        m_view->MustBeResized();
        m_view->Invalidate();
    }

    // 替换当前显示的几何为指定 shape
    void setShape(const TopoDS_Shape& shape)
    {
        if (shape.IsNull())
            return;

        if (!m_currentShape.IsNull())
            m_scene->eraseObject(m_currentShape);

        m_currentShape = Mayo::makeOccHandle<AIS_Shape>(shape);
        m_scene->addObject(m_currentShape);

        m_view->FitAll(0.1, false);
        m_view->ZFitAll();
        m_view->Invalidate();
    }

    // 在视口右上角显示视图立方体（含 XYZ 坐标轴），点击切换视角（参考 Mayo 的 AIS_ViewCube）
    void addViewCube()
    {
        m_viewCube = Mayo::makeOccHandle<AIS_ViewCube>();
        m_viewCube->SetSize(80.);
        m_viewCube->SetFixedAnimationLoop(true);
        m_viewCube->SetAutoStartAnimation(true);

        // 固定在右上角（TriedronPers 模式），离上边/右边各留出 160 像素间距
        m_viewCube->SetTransformPersistence(
            new Graphic3d_TransformPers(
                Graphic3d_TMF_TriedronPers,
                Aspect_TOTP_RIGHT_UPPER,
                Graphic3d_Vec2i(160, 160)));

        m_scene->addObject(m_viewCube);
    }

    // 拾取右上角视图立方体：命中则切换到对应视角并返回 true
    bool tryPickViewCube(int px, int py)
    {
        if (!m_viewCube)
            return false;

        m_scene->highlightAt(px, py, m_view);
        const Mayo::GraphicsOwnerPtr owner = m_scene->currentHighlightedOwner();
        m_scene->clearSelection();  // 清除检测/高亮，避免残留

        const auto cubeOwner = Mayo::OccHandle<AIS_ViewCubeOwner>::DownCast(owner);
        if (!cubeOwner)
            return false;

        m_view->SetProj(cubeOwner->MainOrientation());
        m_view->FitAll(0.1, false);
        m_view->ZFitAll();
        m_view->Invalidate();
        return true;
    }

    void applyNavigation()
    {
        const bool left = m_input.buttons.testFlag(Qt::LeftButton);
        const bool right = m_input.buttons.testFlag(Qt::RightButton);
        const bool middle = m_input.buttons.testFlag(Qt::MiddleButton);

        const int px = qRound(m_input.pos.x());
        const int py = qRound(m_input.pos.y());
        // OCCT 视口像素坐标以左下角为原点（y 向上），而 Qt 鼠标事件以左上角为原点（y 向下），
        // 必须把 y 翻转后再传给 OCCT，否则平移/旋转/拾取的上下方向都会反。
        const int occtY = m_size.height() - 1 - py;

        const bool prevLeft = m_prevButtons.testFlag(Qt::LeftButton);
        const bool prevRight = m_prevButtons.testFlag(Qt::RightButton);
        const bool prevMiddle = m_prevButtons.testFlag(Qt::MiddleButton);

        // 左键按下：先尝试拾取右上角视图立方体（点击切换视角），否则进入旋转
        if (left && !prevLeft) {
            m_viewCubeHit = this->tryPickViewCube(px, occtY);
            if (!m_viewCubeHit)
                m_view->StartRotation(px, occtY);
        }
        else if (left && prevLeft) {
            if (!m_viewCubeHit)
                m_view->Rotation(px, occtY);
        }

        // 右键 / 中键：平移（y 差量同样按 OCCT 坐标系取反）
        if ((right || middle) && (prevRight || prevMiddle)) {
            const int dx = px - qRound(m_prevPos.x());
            const int dy = py - qRound(m_prevPos.y());
            m_view->Pan(dx, -dy);
        }

        // 滚轮：缩放
        if (m_input.wheelDelta != 0) {
            const double factor = std::exp(double(m_input.wheelDelta) / 240.0);
            m_view->SetZoom(factor);
        }

        m_prevButtons = m_input.buttons;
        m_prevPos = m_input.pos;
    }

    std::unique_ptr<Mayo::GraphicsScene> m_scene;
    Mayo::OccHandle<V3d_View> m_view;
    Mayo::OccHandle<AIS_Shape> m_currentShape;
    Mayo::OccHandle<AIS_ViewCube> m_viewCube;
    QSize m_size;
    bool m_initFailed = false;
    bool m_viewCubeHit = false;

    // Qt 场景图持有该 FBO 的所有权（createFramebufferObject 返回值），此处仅保存指针用于在
    // render() 中显式绑定（Qt 6 不会自动绑定，GL_FRAMEBUFFER_BINDING==0 会导致 OCCT FBO 包装失败）。
    QOpenGLFramebufferObject* m_fbo = nullptr;

    GraphicsViewport::Input m_input;
    Qt::MouseButtons m_prevButtons;
    QPointF m_prevPos;
};

// ---------------------------------------------------------------------------
// GraphicsViewport (QML item)
// ---------------------------------------------------------------------------

GraphicsViewport::GraphicsViewport(QQuickItem* parent)
    : QQuickFramebufferObject(parent),
      m_input(std::make_unique<Input>())
{
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    // OCCT 以 OpenGL 约定渲染（原点在左下角），而 Qt Quick 场景图按左上角采样 FBO 纹理，
    // 必须垂直镜像，否则画面上下颠倒（模型、AIS_ViewCube 的 Y 标签等都会反）。
    setMirrorVertically(true);
}

GraphicsViewport::~GraphicsViewport() = default;

QQuickFramebufferObject::Renderer* GraphicsViewport::createRenderer() const
{
    return new GraphicsViewportRenderer;
}

void GraphicsViewport::mousePressEvent(QMouseEvent* event)
{
    m_input->buttons = event->buttons();
    m_input->pos = event->position();
    update();
    event->accept();
}

void GraphicsViewport::mouseMoveEvent(QMouseEvent* event)
{
    m_input->buttons = event->buttons();
    m_input->pos = event->position();
    update();
    event->accept();
}

void GraphicsViewport::mouseReleaseEvent(QMouseEvent* event)
{
    m_input->buttons = event->buttons();
    m_input->pos = event->position();
    update();
    event->accept();
}

void GraphicsViewport::wheelEvent(QWheelEvent* event)
{
    m_input->wheelDelta += event->angleDelta().y();
    update();
    event->accept();
}
