# Graphics 模块：OCCT AIS 渲染层
# 代码取自 Mayo（https://github.com/fougue/mayo src/graphics）+ 精简后的 base 依赖
# 无 Qt/GUI 依赖，可被 GUI、MCP server、headless/batch 复用

file(GLOB GRAPHICS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/*.cpp
)
file(GLOB GRAPHICS_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base/*.h
)

# vendored fmt（编译模式，非 header-only）
set(FMT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/fmt/src/format.cc)

add_library(cxcam_graphics STATIC
    ${GRAPHICS_SOURCES}
    ${GRAPHICS_HEADERS}
    ${FMT_SOURCE}
)

# 头文件根：
#   src/            -> "Graphics/xxx.h" 约定（与 Gui/Main 一致）
#   src/3rdparty    -> <gsl/span> <fmt/format.h> <kdbindings/signal.h>
target_include_directories(cxcam_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty
)

target_compile_definitions(cxcam_graphics PUBLIC
    NOMINMAX
)

if(MSVC)
    target_compile_options(cxcam_graphics PUBLIC /utf-8 /W3)
endif()

# OpenCASCADE：
#   OCCT 导出的 imported target（TKernel、TKV3d、…）没有命名空间，且其 INTERFACE_LINK_LIBRARIES
#   里硬编码了本机已不存在的第三方路径（D:/OCCT/Components/.../freetype.lib），会直接导致链接失败。
#   因此：
#     1. include / library 目录从 OpenCASCADE_DIR（<prefix>/cmake）反推，不依赖 imported target；
#     2. 按 .lib 绝对路径链接，绕过 imported target（target_link_libraries 里写裸名 "TKernel"
#        会被 CMake 解析成同名 imported target，从而再次引入失效路径）。
get_filename_component(OCCT_PREFIX "${OpenCASCADE_DIR}" DIRECTORY)
set(OCCT_INCLUDE_DIR "${OCCT_PREFIX}/inc")
set(OCCT_LIBRARY_DIR "${OCCT_PREFIX}/win64/vc14/lib")

target_include_directories(cxcam_graphics PUBLIC "${OCCT_INCLUDE_DIR}")
target_link_directories(cxcam_graphics PUBLIC "${OCCT_LIBRARY_DIR}")

foreach(_occt_lib IN ITEMS
        # FoundationClasses
        TKernel TKMath
        # ModelingData
        TKG2d TKG3d TKGeomBase TKBRep
        # ModelingAlgorithms
        TKBO TKBool TKGeomAlgo TKHLR TKMesh TKPrim TKShHealing TKTopAlgo
        # Visualization
        TKMeshVS TKOpenGl TKService TKV3d
        # ApplicationFramework
        TKBin TKBinL TKBinXCAF TKCAF TKCDF TKLCAF TKVCAF TKXml TKXmlL
        # DataExchange (XCAF)
        TKXCAF TKXmlXCAF
        # DataExchange (STEP/BRep 读取：TKDE 基类 + TKDESTEP)
        TKDE TKDESTEP TKXSBase)
    target_link_libraries(cxcam_graphics PUBLIC "${OCCT_LIBRARY_DIR}/${_occt_lib}.lib")
endforeach()

if(WIN32)
    target_link_libraries(cxcam_graphics PUBLIC opengl32 user32 gdi32 advapi32)
endif()
