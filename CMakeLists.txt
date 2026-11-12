cmake_minimum_required(VERSION 3.21)
project(CXCAM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt 元对象系统
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 模块开关（后续按需打开）
option(CXCAM_WITH_OCCT    "构建 OpenCASCADE 几何内核与 AIS 渲染" ON)
option(CXCAM_WITH_OPENVDB "构建 OpenVDB 切削仿真" OFF)
option(CXCAM_BUILD_TESTS  "构建单元测试" OFF)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 QuickLayouts OpenGL)

if(CXCAM_WITH_OCCT)
    # OCCT 安装位置（本机：D:/OCCT/OCCT-install-release）
    set(OpenCASCADE_DIR "D:/OCCT/OCCT-install-release/cmake"
        CACHE PATH "OpenCASCADE 安装目录（含 OpenCASCADEConfig.cmake）")
    find_package(OpenCASCADE REQUIRED)
endif()
if(CXCAM_WITH_OPENVDB)
    find_package(OpenVDB REQUIRED)
endif()

add_subdirectory(src)

if(CXCAM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
