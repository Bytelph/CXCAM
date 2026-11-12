set(GUI_HEADERS
    WindowController.h
    SystemTheme.h
)

set(GUI_SOURCES
    WindowController.cpp
    SystemTheme.cpp
)

# OCCT AIS 视口（QQuickFramebufferObject 承载）
if(CXCAM_WITH_OCCT)
    list(APPEND GUI_HEADERS qtopengl_utils.h GraphicsViewport.h)
    list(APPEND GUI_SOURCES qtopengl_utils.cpp GraphicsViewport.cpp)
endif()

add_library(cxcam_gui STATIC ${GUI_SOURCES} ${GUI_HEADERS} qml.qrc)

# 头文件以 src/ 为根，故用 "Gui/xxx.h" 形式 include
target_include_directories(cxcam_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(cxcam_gui PUBLIC
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
    Qt6::OpenGL
)

# OCCT 渲染（cxcam_graphics 的 PUBLIC 依赖会透传 OCCT include/lib 与链接库）
if(CXCAM_WITH_OCCT)
    target_link_libraries(cxcam_gui PUBLIC cxcam_graphics)
    # 供 main.cpp 条件编译（OCCT 视口注册 / platformSetup）
    target_compile_definitions(cxcam_gui PUBLIC CXCAM_HAS_OCCT)
endif()

# DWM 圆角（Windows 11）
if(WIN32)
    target_link_libraries(cxcam_gui PUBLIC dwmapi)
endif()
