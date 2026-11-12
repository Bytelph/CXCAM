# CXCAM

基于 C++17 / OCCT / Qt 的 CAM 软件。当前阶段为 **M0 地基**：Fusion 360 风格界面骨架。

完整方案见 [`PLAN.md`](PLAN.md)。

## 目录结构

```
src/
├─ Base/         基础库（Math/Log/单位/序列化）
├─ Core/         OCCT 封装（几何服务）
├─ App/          应用框架（Document/UndoRedo/Command）
├─ Gui/          Qt + AIS 界面（当前已实现 UI 骨架）
├─ Cam/          CAM 内核（Job/Tool/Toolpath/CLData）
├─ Sim/          切削仿真（OpenVDB）
├─ Prb/          在机测量
├─ Post/         后处理
├─ ToolLib/      刀具库
├─ Mcp/          MCP Server
└─ Main/         可执行入口（CXCAM）
```

## 依赖

- CMake ≥ 3.21
- Qt 6.x（Widgets）
- MSVC 2019/2022 或 MinGW（C++17）
- 可选：OpenCASCADE（`-DCXCAM_WITH_OCCT=ON`）、OpenVDB（`-DCXCAM_WITH_OPENVDB=ON`）

## 构建（Windows）

```bash
# 1) 配置（指定 Qt 路径）
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 \
      -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"

# 或 Ninja（需先装 ninja 并让 cl 在 PATH 中）
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"

# 2) 构建
cmake --build build --config Release

# 3) 运行
./build/Release/CXCAM.exe   # VS 生成器
./build/CXCAM.exe           # Ninja 生成器
```

## 当前界面

- 顶部应用栏（Logo / 文档标签 / 撤销重做保存）
- Ribbon 工作区切换（建模 / 加工 / 仿真 / 测量）
- 左侧浏览器树、右侧属性面板
- 中央视口（占位，右上角导航立方体）
- 底部状态栏
