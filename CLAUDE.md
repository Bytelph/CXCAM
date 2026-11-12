# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

CXCAM is a CAM (computer-aided manufacturing) application: C++17 + Qt 6 (Quick/QML) + OpenCASCADE (OCCT) + OpenVDB, with an MCP server for driving it from Claude/Codex. `PLAN.md` is the authoritative design document (v0.1); `README.md` is a short summary.

**Current stage: M0 "foundation".** A Fusion-360-style QML UI skeleton plus a working OCCT AIS 3D viewport (Mayo-derived `Graphics` module). The `Base/`, `Core/`, `App/`, `Cam/`, `Sim/`, `Prb/`, `Post/`, `ToolLib/`, `Mcp/` directories under `src/` are empty placeholders (`.gitkeep`) for the planned architecture — do not assume their APIs exist yet.

> The UI is **Qt Quick/QML**, not Widgets. `CLAUDE.md` historically said "Widgets"; that was wrong and has been corrected here. The active UI code uses `QGuiApplication` + `QQmlApplicationEngine` (see `src/Main/main.cpp`).

## Build

Windows + MSVC only (no MinGW setup is present). Requires: CMake ≥ 3.21, Qt 6.x (Quick, QuickControls2, QuickLayouts, OpenGL), MSVC 2019/2022+, C++17, OCCT 7.8 (installed at `D:/OCCT/OCCT-install-release`).

Toolchain locations on this machine:
- Qt 6.8.2 (MSVC 2022 64-bit): `D:/Qt/6.8.2/msvc2022_64`
- Visual Studio 2026 Community: `D:/Program Files/Microsoft Visual Studio/18/Community`
- CMake / Ninja (bundled with Qt): `D:/Qt/Tools/CMake_64/bin/cmake.exe`, `D:/Qt/Tools/Ninja/ninja.exe`
- OCCT 7.8.0: `D:/OCCT/OCCT-install-release` (libs `win64/vc14/lib`, DLLs `win64/vc14/bin`, headers `inc`)

Ninja requires the MSVC developer environment on PATH (run `vcvars64.bat` first):

```bash
# from a VS developer prompt, or after `call ".../VC/Auxiliary/Build/vcvars64.bat"`
cmake -S . -B build -G Ninja \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_MAKE_PROGRAM=D:/Qt/Tools/Ninja/ninja.exe \
      -DCMAKE_PREFIX_PATH=D:/Qt/6.8.2/msvc2022_64 \
      -DCXCAM_WITH_OCCT=ON
cmake --build build
```

Or with the Visual Studio generator (no vcvars needed):

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 \
      -DCMAKE_PREFIX_PATH=D:/Qt/6.8.2/msvc2022_64 \
      -DCXCAM_WITH_OCCT=ON
cmake --build build --config Release
```

The executable lands at `<build-dir>/src/Main/CXCAM.exe` (the target is defined in `src/Main/CMakeLists.txt` and no `RUNTIME_OUTPUT_DIRECTORY` is set — `README.md`'s `./build/CXCAM.exe` is wrong).

**`CXCAM_WITH_OCCT` is `ON` by default now** (it was historically `OFF`). If you have an existing build directory configured with it `OFF`, the old cache value persists — pass `-DCXCAM_WITH_OCCT=ON` explicitly (or delete the cache) to enable OCCT.

### Running

`CXCAM.exe` needs Qt + OCCT DLLs on `PATH` and the Qt platform plugin. A ready-made launcher is generated at `build/run_cxcam.bat` (sets `PATH` to Qt bin + OCCT `win64/vc14/bin`, sets `QT_PLUGIN_PATH`, then runs the exe by absolute path). Equivalent manual setup:

```bash
export PATH="/d/Qt/6.8.2/msvc2022_64/bin:/d/OCCT/OCCT-install-release/win64/vc14/bin:$PATH"
export QT_PLUGIN_PATH="D:/Qt/6.8.2/msvc2022_64/plugins"
```

OCCT DLLs (including `TKernel.dll`, `TKOpenGl.dll`, `freetype.dll`) live in `D:/OCCT/OCCT-install-release/win64/vc14/bin`.

There are **no** lint or test commands yet: `tests/` is empty and `CXCAM_BUILD_TESTS` defaults to `OFF`.

VSCode: `.vscode/` is configured for CMake Tools + C/C++ extensions (kit `MSVC 2026 x64 + Ninja`, build dir `build/${buildType}`). `.gitignore` ignores `.vscode/`, so that config is local-only.

## Architecture

Strictly layered, with kernel/UI separation modeled on FreeCAD's App/Gui split:

```
Gui      (Qt Quick/QML + OCCT AIS viewport)      ← presentation only
Graphics (OCCT AIS rendering, Qt-free)           ← reusable by GUI / MCP / headless
App      (Document / FeatureTree / UndoRedo / Command / Workbench)
Core (OCCT geometry services) / Cam (toolpath) / Mcp / Sim / Prb / Post / ToolLib
Base (Math(Eigen) / Log(spdlog) / units / serialization)
```

- The CAM kernel, geometry services, `Graphics` module, and MCP server must have **no Qt/GUI dependency**, so the same kernel is reused by the GUI, the MCP server, and headless/batch modes. `Graphics` is Qt-free by design (it only links OCCT + Qt-free deps).
- **CLData (ISO 3592/APT)** is the central intermediate representation. AICAM toolpaths, SIM-verified results, PRB measurement programs, and post-processor G-code all flow through it; post-processing only translates CLData → G-code. Data model: `Part/Job → Stock, Tool, Setup, Operation → Toolpath → Machine`.
- Modules are independent static/dynamic libraries, buildable and testable separately; workbench plugins may later be dynamically loaded from `modules/`.
- Single viewport: one `QQuickFramebufferObject` + `V3d_View` + `AIS_InteractiveContext`. Switching Modeling/Toolpath/Simulate/Result modes toggles AIS display collections (Display/Erase), not widgets.
- Project file `.ccpj` is a ZIP container: geometry as OCCT binary BRep, parameters/toolpaths as JSON, simulation state as `.vdb`.

## Current implementation (M0)

Real code lives in `src/Graphics`, `src/Gui`, and `src/Main`.

### `src/Graphics` — OCCT AIS rendering module (Qt-free)

Code vendored from [Mayo](https://github.com/fougue/mayo) `src/graphics` (BSD-2-Clause), plus a pruned `base/` dependency closure and vendored `3rdparty/` headers (Microsoft GSL, fmt, KDAB KDBindings). It builds a static lib `cxcam_graphics` (see `src/Graphics/CMakeLists.txt`) exposing `Mayo::GraphicsScene`, object drivers, and the graphics driver factory.

**Important OCCT linkage quirk:** OCCT's exported CMake targets are namespace-less (`TKernel`, `TKV3d`, …) and their `INTERFACE_LINK_LIBRARIES` hardcode paths that don't exist on this machine (`D:/OCCT/Components/.../freetype.lib`). Linking them by bare name (`target_link_libraries(... TKernel)`) still resolves to the imported target and reintroduces the broken path. `src/Graphics/CMakeLists.txt` therefore links each OCCT lib by **absolute `.lib` path** (`${OCCT_PREFIX}/win64/vc14/lib/TKernel.lib`), deriving the prefix from `OpenCASCADE_DIR`.

### `src/Gui` — QML UI + OCCT viewport

- `src/Gui/qml/*.qml` — the whole UI (window chrome, ribbon, browser/properties panels, status bar), compiled into `qml.qrc`.
- `WindowController.{h,cpp}`, `SystemTheme.{h,cpp}` — C++ bridge objects exposed to QML.
- `GraphicsViewport.{h,cpp}` — `QQuickFramebufferObject` hosting the OCCT AIS view (`V3d_View` + `GraphicsScene`), with the GL-state handoff (`initializeGlFramebufferObject` / `resetGlStateBeforeOcct` / `resetGlStateAfterOcct`) and mouse rotation/pan/zoom. OCCT types are hidden behind the `.cpp` (the header is OCCT-free). Registered to QML as `CXCAM.Graphics.GraphicsViewport`.
- `qtopengl_utils.{h,cpp}` — Mayo's Qt GL helpers (`platformSetup`, `initializeGlFramebufferObject`, …).
- `src/Gui/qml/Viewport.qml` loads either `OccViewport.qml` (native OCCT viewport) or a `Canvas` placeholder, chosen by the `occAvailable` context property (set in `main.cpp`).

`src/Gui/CMakeLists.txt` builds static lib `cxcam_gui`; the OCCT files are added only when `CXCAM_WITH_OCCT` is ON, and that block also defines `CXCAM_HAS_OCCT` (used by `main.cpp` to conditionally register the viewport).

### `src/Main` — entry point

`main.cpp` calls `Mayo::QtOpenGlUtils::platformSetup(argc, argv)` (forces desktop OpenGL + shared GL contexts, before `QGuiApplication`), registers the `Theme` singleton and (when OCCT is on) the `GraphicsViewport` type, sets the `occAvailable` context property, and loads `qrc:/qml/Main.qml`.

### Stale/orphaned files (do not touch unless resurrecting them)

`src/Gui/` still contains pre-QML Widgets-era files that are **not compiled** (not listed in `src/Gui/CMakeLists.txt`): `MainWindow.{h,cpp}`, `Style.{h,cpp}`, `ViewportWidget.{h,cpp}`, `ViewCubeWidget.{h,cpp}`, `ApplicationBar.{h,cpp}`, `BrowserPanel.{h,cpp}`, `PropertiesPanel.{h,cpp}`, `RibbonBar.{h,cpp}`. They reference `QMainWindow`/`QWidget` and are superseded by the QML UI.

### Build wiring (top-down)

- Root `CMakeLists.txt`: C++17, `CMAKE_AUTOMOC/AUTORCC/AUTOUIC ON`, `find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 QuickLayouts OpenGL)`, options `CXCAM_WITH_OCCT` (default **ON**) / `CXCAM_WITH_OPENVDB` / `CXCAM_BUILD_TESTS` (default `OFF`), `find_package(OpenCASCADE)` when OCCT is on, then `add_subdirectory(src)`.
- `src/CMakeLists.txt`: `add_subdirectory(Graphics)` (only if OCCT), then `add_subdirectory(Gui)`, `add_subdirectory(Main)`.
- `src/Graphics/CMakeLists.txt`: static lib `cxcam_graphics` (globbed `*.cpp` + `base/*.cpp` + vendored `fmt/src/format.cc`), OCCT linked by absolute path (see above).
- `src/Gui/CMakeLists.txt`: static lib `cxcam_gui` linking `Qt6::Quick/QuickControls2/QuickLayouts/OpenGL` (+ `cxcam_graphics` when OCCT), include root `src/..`.
- `src/Main/CMakeLists.txt`: `CXCAM` executable (`WIN32_EXECUTABLE TRUE`) linking `cxcam_gui`.

**Include convention:** headers are rooted at `src/`, so inside `src/` you write `#include "Gui/MainWindow.h"` (not `#include "MainWindow.h"`), `#include "Graphics/graphics_scene.h"`, `#include "Graphics/base/occ_handle.h"`. This comes from `target_include_directories(<lib> PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)` on each lib and is used throughout. `src/3rdparty` is on the include path for `<gsl/span>`, `<fmt/format.h>`, `<kdbindings/signal.h>`. When adding new modules, keep this `src/`-rooted convention consistent.
