# CXCAM 方案文档

> 版本：v0.1（2026-11-09）
> 状态：方案设计阶段

## 1. 项目概述

CXCAM 是一款以 FreeCAD 为参考、基于 C++17 与 OpenCASCADE Technology (OCCT) 的 CAM 软件，核心模块：

| 模块 | 功能 |
|---|---|
| **AICAM** | 加工刀路自动生成 |
| **SIM** | 几何—物理切削仿真 |
| **PRB** | 在机测量（探针） |
| 辅助模块 | 后处理、刀具库、MCP（支持 Claude / Codex） |

项目文件格式：**`.ccpj`**。

---

## 2. 设计原则

1. **内核与界面分离**：App（无 UI 领域内核）/ Gui（表现层）解耦，参考 FreeCAD 的 App/Gui 拆分。
2. **加工数据流单向、可审计、可复现**：所有刀路以 CLData 为中间表示，后处理只做翻译。
3. **模块插件化**：每个模块是独立静态/动态库，可单独编译、测试、替换。
4. **确定性优先**：刀路生成、仿真、测量均可复现（日志 + 参数快照）。
5. **单视口显示**：无主副窗口，一个视口内通过显示集合切换模式。

---

## 3. 技术选型

| 类别 | 选择 | 说明 |
|---|---|---|
| 语言 | C++17 | — |
| 几何内核 | OCCT 7.8+ | BRep、布尔、网格化、AIS |
| **显示层** | **AIS（OCCT 原生）** | 单视口独占，PBR + 光追 |
| UI 框架 | Qt 6 | 与 OCCT 集成最好 |
| 线性代数 | Eigen | 力模型、姿态、变换 |
| 并行 | OpenMP / TBB | 刀路计算、仿真加速（OpenVDB 依赖 TBB） |
| **切削仿真** | **OpenVDB** | 稀疏体素，3/5 轴统一材料去除 |
| 压缩容器 | miniz / libzip | `.ccpj` 项目文件 |
| 数据 | SQLite | 刀具库 |
| 日志 | spdlog | 全链路可审计 |
| MCP/JSON | nlohmann/json | MCP 消息、配置、CLData、后处理表 |
| 构建 | CMake + vcpkg/conan | 依赖管理 |

### 3.1 显示层决策

**AIS 全栈，不用 VTK、不用 Coin3D。** 理由：

1. 几何内核已是 OCCT，AIS 直接渲染 `TopoDS_Shape`，**零转换**；VTK/Coin3D 需自建 BRep→网格桥接层。
2. 单视口下统一显示层，避免多引擎的上下文同步与依赖负担。
3. AIS 覆盖 CAM 90% 显示需求（零件、毛坯、刀路曲线、刀具运动、选择高亮）。
4. AIS 内置 PBR + 阴影 + SSAO + MSAA + 胶片色调映射 + 光线追踪，是开源 CAD 交互视口里"逼真高级"效果最好的方案。

**视觉"高级感"配置**：

```cpp
Graphic3d_RenderingParams& p = view->ChangeRenderingParams();
// 交互模式（主视口日常）
p.Method          = Graphic3d_RM_RASTERIZATION;
p.NbMsaaSamples   = 8;
p.IsShadowEnabled = true;
p.ShadowMapResolution = 2048;
p.IsAmbientOcclusionEnabled = true;
p.IsAntialiasingEnabled    = true;
p.IsReflectionEnabled = true;
p.ToneMappingMethod = Graphic3d_ToneMappingMethod_Filmic;

// 材质走 PBR（金属度/粗糙度）
Graphic3d_MaterialAspect mat(Graphic3d_NameOfMaterial_Steel);
mat.SetPbr(true);
mat.SetMetallic(0.9f);
mat.SetRoughness(0.3f);

// 截图/输出时切光追，出照片级，不换引擎
p.Method = Graphic3d_RM_RAYTRACING;
p.IsGlobalIlluminationEnabled = true;
p.RaytracingDepth = 8;
```

---

## 4. 总体架构（分层）

```
┌─────────────────────────────────────────────────────┐
│  GUI 层 (Qt + AIS 单视口)                            │
│   ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐  │
│   │ CAD/建模 │ │ AICAM  │ │  SIM   │ │  PRB   │  │
│   └─────────┘ └─────────┘ └─────────┘ └─────────┘  │
│   后处理 / 刀具库 / MCP 面板                          │
├─────────────────────────────────────────────────────┤
│  应用框架层：Document / FeatureTree / Undo-Redo /     │
│  Selection / 命令模式 / 工作台(Workbench)            │
├─────────────────────────────────────────────────────┤
│  内核层 (无 UI，纯 C++17)                            │
│   ┌───────────┐ ┌───────────┐ ┌───────────┐        │
│   │  CAM 内核 │ │  几何服务  │ │  MCP 服务 │        │
│   │ Toolpath  │ │  OCCT封装  │ │  (Server) │        │
│   │ CLData    │ └───────────┘ └───────────┘        │
│   │ Simulation│                                    │
│   └───────────┘                                    │
├─────────────────────────────────────────────────────┤
│  基础库：Math(Eigen) / Log(spdlog) / 序列化 / 单位   │
└─────────────────────────────────────────────────────┘
```

核心点：**CAM 内核、几何服务、MCP 服务都无 GUI 依赖**，MCP 服务器和批处理/无头模式可直接复用同一套内核。

---

## 5. 显示层设计（单视口）

### 5.1 结构

```
一个 QOpenGLWidget + 一个 V3d_View + 一个 AIS_InteractiveContext
    └─ 里面同时挂所有对象，靠"显示/隐藏"切模式
```

模式切换 = 切换"显示集合"，不是切换 widget：

```cpp
class CamViewport : public QOpenGLWidget {
    Handle(V3d_View)                 m_view;
    Handle(AIS_InteractiveContext)   m_ctx;

    Handle(AIS_Shape) m_part;    // 零件
    Handle(AIS_Shape) m_stock;   // 毛坯（仿真时原地更新）
    Handle(AIS_InteractiveObject) m_toolpath; // 刀路
    Handle(AIS_InteractiveObject) m_tool;     // 刀具

    void setMode(Mode m);  // Modeling / Toolpath / Simulate / Result
};
```

| 模式 | 显示内容 |
|---|---|
| 建模 | 零件、毛坯 |
| 刀路 | 零件 + 刀路(CLData 曲线) + 刀具 |
| 仿真 | 毛坯(实时更新网格) + 刀具扫掠 |
| 结果 | 零件 + 场数据（颜色映射） |

### 5.2 切换实现

`AIS_InteractiveContext` 的 **Display / Erase** 控制对象进/出显示列表，纯改显示列表、不重建几何、不换窗口，切换零成本：

```cpp
// 按模式分组切换
void switchMode(DisplayMode m) {
    for (auto& o : m.hidden) ctx->Erase(o, false);
    for (auto& o : m.shown) ctx->Display(o, 0, 0, false);
    ctx->UpdateCurrentViewer();
}
```

### 5.3 切削仿真的显示（关键）

AIS 的瓶颈不在 OpenVDB 的 CSG（快），而在**渲染回填**：OCCT 标准 AIS 路径无增量更新，几何一变即整个 VBO 全量重传，成本 ∝ 三角形数 × 帧率。

| 网格规模 | 更新频率 | AIS 表现 |
|---|---|---|
| 5万~50万 三角 | 5~15 fps | ✅ 流畅 |
| 50万~200万 | 5~15 fps | ⚠️ 掉帧 |
| 200万+ 或 60fps | 高频 | ❌ 卡顿 |

CAM 仿真"播放"不需要 60fps，**多段刀路批处理、按 5~15fps 更新**即可。

**仿真显示对象用接口隔离，支持平滑升级**：

```cpp
class ISimStockView {
    virtual void updateMesh(const MeshData&) = 0;
};
```

- **Tier 1（起步）**：`AIS_Shape` 原地更新——只 Display 一次，每帧换三角化 + `Redisplay`。
- **Tier 2（掉帧时升级）**：自定义 `AIS_InteractiveObject` 直管 `Graphic3d_Buffer`，绕过 BRep 三角化，直接灌顶点数组，仍在 AIS 体系内。

原则：**先用 Tier 1 验证 OpenVDB 数据流和精度，实测掉帧再换 Tier 2，不预先过度优化。**

---

## 6. 模块详细设计

### 6.1 核心数据模型

```
Part/Job         —— 加工任务：零件 + 毛坯 + 坐标系(WCS) + 机床
  ├─ Stock        —— 毛坯（BRep）
  ├─ Tool         —— 刀具（几何 + 切削参数）
  ├─ Setup        —— 装夹/工位
  ├─ Operation    —— 一道工序（策略 + 参数 + 生成的刀路）
  │    └─ Toolpath  —— 刀路（CLData 序列）
  └─ Machine      —— 机床运动学（用于后处理与机床仿真）
```

**CLData（ISO 3592 / APT 标准）是核心中间层**，AICAM 的刀路、SIM 验证后的修正、PRB 的测量程序、后处理的 G代码全部经它解耦：

```
GOTO / x, y, z [, i, j, k]      // 直线定位 + 刀轴矢量
CIRCLE / x, y, z, i, j, k, r    // 圆弧
FEDRAT / xxx                    // 进给
SPINDL / rpm, dir               // 主轴
TOOLNO / n                      // 换刀
RAPID / COOLNT / ...            // 快移、冷却等
```

### 6.2 AICAM 模块（刀路自动生成）

```
┌────────────────────────────────────────────┐
│  特征识别（Feature Recognition）            │
│   面/边 → 型腔、槽、孔、平面、轮廓、自由曲面 │
├────────────────────────────────────────────┤
│  策略引擎（Strategy）                       │
│  2.5D: 面铣/型腔/轮廓/钻孔/槽              │
│  3D:  平行/等高/环绕/清根/残料             │
│  5轴: 侧刃/点铣/刀轴优化(后续)            │
├────────────────────────────────────────────┤
│  刀位计算（Path Generation）                │
│  偏置曲线 / 等参线 / 投影 / 干涉检查        │
└────────────────────────────────────────────┘
        ↓ 输出 CLData
```

**自动化层次**（务实，不一步到位"AI 全自动"）：

1. **L1 特征识别**：从 BRep 自动识别加工特征，映射到策略（确定性算法）。
2. **L2 策略推荐**：根据特征 + 刀具 + 材料推荐策略和参数（规则/决策树）。
3. **L3 智能辅助**：通过 MCP 让 Claude/Codex 参与决策（选策略、调参数、解释刀路）。

关键技术点：型腔偏置曲线（Voronoi/中轴线）、清根残料计算（前一刀具 Z-map 或布尔差）、干涉检查（刀柄/刀杆 vs 零件）。

### 6.3 SIM 模块（切削仿真）

分两层，难度差异大，务必分开规划。

**A. 几何仿真（材料去除 / 碰撞 / 过切）—— 第一优先，采用 OpenVDB**

- 表示方式：**Level Set**（`FloatGrid` 存有符号距离场），表面平滑，需周期重归一化（`signedFloodFill` / `levelSetRebuild`）。
- 体素尺寸 = 精度旋钮（建议 = 最小刀具直径 / 20~30）。

```
毛坯 BRep ──meshToVolume──▶ FloatGrid(stock)
                              │
每段刀路: 构造刀具扫掠体 ──meshToVolume──▶ FloatGrid(tool)
                              │
                     csgDifference(stock, tool)   ← 材料去除
                              │
                     周期性 levelSetRebuild
                              │
                    volumeToMesh ──▶ 三角网格 ──▶ 回填 AIS → Redisplay
```

```cpp
openvdb::FloatGrid::Ptr stock =
    openvdb::tools::meshToVolume<openvdb::FloatGrid>(stockMesh, xform);
openvdb::tools::csgDifference(*stock, *toolSweptGrid);  // 减料
std::vector<openvdb::Vec3s> pts;
std::vector<openvdb::Vec4I> quads;
openvdb::tools::volumeToMesh(*stock, pts, quads);       // 回网格
```

**刀具扫掠体构造**（不能每步盖章，会有空隙）：
- 直线 GOTO 段：平底刀扫掠体 = 棱柱体 + 两端圆盘；球头刀 = 胶囊体，解析可构造。
- 圆弧/5 轴段：扫掠包络算法，或"沿路径密集采样 + union"起步。

**精度分层**：粗仿真（低体素）交互预览，精仿真（高体素）过切/余量判定。

**B. 物理仿真（切削力 / 变形 / 颤振）—— 研究级，分阶段**

1. **力学模型**（mechanistic model）：切削力系数算每齿受力（工业界成熟做法，可做）。
2. **刀具变形**：悬臂梁模型（力 + 刀具刚度）→ 偏斜误差。
3. **颤振/热/应力场**：FEM 或稳定性叶瓣图 → 研究级，后期。

### 6.4 PRB 模块（在机测量 / 探针）

```
测量特征定义 → 测头路径规划(趋近/回退/避障) → 测量程序生成
                                              ↓
                             测量结果 ← 机床回传(宏输出/串口/文件)
                                              ↓
                          名义值对比 + 公差判定 + SPC + 补偿回写
```

- 测量类型：单点、平面（3+点）、圆/孔（3+点）、槽、线、坐标变换（找正）。
- 输出：Renishaw Inspection Plus、海德汉、西门子 CYCLE、发那科 G31 等测头宏。
- **闭环价值**：测量结果 → 坐标系/余量补偿 → 回写 AICAM 重新生成刀路（核心卖点）。
- 关键点：测头路径避障（保护性移动）、超程/碰撞防护、结果格式解析。

### 6.5 后处理模块

- 输入 CLData，输出 G代码；用**配置表 + 脚本**，不硬编码。
- 内置模板：Fanuc、Siemens 840D、Heidenhain、Haas、Mazak。
- 关键：圆弧象限处理、G90/G91 状态机、程序号/行号、循环展开（钻孔 CYCLE→G81）、刀轴变换（5 轴 RTCP）。

### 6.6 刀具库模块

- 刀具定义：平底/球头/圆鼻/钻头/镗刀/螺纹 + 刀柄 + 组件。
- SQLite 存库，支持导入 ISO 13399 / 常见格式。
- 切削参数（每齿进给、线速度）随材料与刀具材质关联，供 AICAM 参数推荐。

### 6.7 MCP 模块（支持 Claude + Codex）

把 CAM 内核包装成 **MCP Server**，让 Claude Code 和 Codex CLI 都能驱动软件：

```
Tools（AI 可调用）：
  create_part / load_part / define_tool / create_job
  recognize_features / generate_toolpath / simulate / verify
  post_process / probe_cycle

Resources（AI 可读）：
  当前文档树、刀路列表、仿真结果、测量报告

Prompts（提示模板）：
  引导 AI 走完整流程
```

设计要点：

- MCP 走 **stdio + JSON-RPC 2.0**，与 nlohmann/json 对接；后续可加 HTTP/SSE。
- **无头模式**：MCP Server 直接加载内核库，不启动 GUI（内核无 UI 依赖的原因）。
- "AI 生成刀路" = **LLM 决策 + 确定性内核执行**，避免 AI 直接编刀路坐标导致撞刀的可信度问题。
- **安全兜底**：AI 通过 MCP 改刀路/参数，必须强制过切检查。

---

## 7. 项目文件格式 `.ccpj`

### 7.1 选型：ZIP 容器（非纯 JSON）

BRep 几何是二进制大块，塞 JSON 既慢又无法部分加载。采用 **ZIP 压缩容器 + 内部结构化文件**（同 FreeCAD `.FCStd`、Office `.docx`）：

- 几何走 OCCT 原生二进制 BRep（往返无损、最快）。
- 参数/刀路走 JSON（可读、可 diff、与 MCP 栈统一）。
- 加缩略图支持文件管理器预览。

### 7.2 内部结构

```
project.ccpj  (ZIP 容器)
├─ project.json         —— 元数据：格式版本、软件版本、单位、坐标系、时间戳
├─ Document.json        —— 文档树：对象图、Feature 树、依赖关系、各对象参数
├─ Geometry/
│    └─ body_01.brep    —— BRep 几何（BRepTools::Write 二进制）
├─ Toolpaths/
│    └─ op_01.cldata    —— CLData 刀路（JSON，含策略参数快照）
├─ Sim/
│    └─ stock.vdb       —— OpenVDB 网格（仿真中间态，可选）
├─ Post/
│    └─ nc_01.gcode     —— 后处理输出（可选）
├─ ToolLib/
│    └─ tools.json      —— 本工程引用的刀具
└─ thumbnail.png        —— 预览图
```

### 7.3 序列化策略

| 内容 | 格式 | 读写方式 |
|---|---|---|
| BRep 几何 | 二进制 BREP | `BRepTools::Write/Read` |
| 参数/元数据 | JSON | `nlohmann/json` |
| 刀路 CLData | JSON | 结构化序列化（含参数快照，保证可复现） |
| 仿真体素 | `.vdb` | `openvdb::io::File` |
| 缩略图 | PNG | 存盘时从 AIS 视口截图 |

原则：几何与参数分离（懒加载）、版本兼容（未知字段忽略）、增量保存（只写脏对象）、刀路可复现（参数快照）。

---

## 8. 目录结构

```
CXCAM/
├─ CMakeLists.txt
├─ cmake/
├─ src/
│  ├─ Base/          # 基础库：Math、Log、单位、序列化、配置
│  ├─ Core/          # OCCT 封装：几何服务、BRep 工具、网格化
│  ├─ App/           # 应用框架：Document、FeatureTree、UndoRedo、Command
│  ├─ Gui/           # Qt + AIS 单视口、选择、工作台、CamViewport
│  ├─ Cam/           # CAM 内核：Job/Tool/Toolpath/CLData/Feature
│  ├─ Cam/Strategy/  # 各策略实现（Pocket、Profile、Drill、Parallel…）
│  ├─ Sim/           # 几何仿真(OpenVDB) + 物理仿真(力模型)
│  ├─ Prb/           # 在机测量：测头路径、宏生成、结果分析
│  ├─ Post/          # 后处理：CLData→G代码 + 配置表
│  ├─ ToolLib/       # 刀具库（SQLite）
│  ├─ Mcp/           # MCP Server（JSON-RPC、工具注册、无头入口）
│  └─ Main/          # 可执行入口：CXCAM(GUI) / cxcam-mcp(无头)
├─ modules/          # 可动态加载的工作台插件（可选，后期）
├─ resources/        # 图标、后处理配置、刀具模板、探针宏
├─ tests/            # 单元测试 + 集成测试（CTest）
└─ docs/
```

---

## 9. 开发路线图

| 阶段 | 目标 | 关键交付 |
|---|---|---|
| **M0 地基** | 骨架跑通 | OCCT+Qt+AIS 单视口、Document/UndoRedo、CMake、`.ccpj` 读写 |
| **M1 2.5D CAM** | 最小可用 | 面铣/型腔/钻孔/轮廓 → CLData → 基础后处理 → 刀路预览 |
| **M2 几何仿真** | 验证能力 | OpenVDB 材料去除 + 过切/碰撞检查 + 刀具库 |
| **M3 3D + PRB** | 扩展 | 3 轴曲面策略、清根残料、PRB 测量循环与结果对比 |
| **M4 MCP + 智能** | 差异化 | MCP Server 接入 Claude/Codex、AI 辅助策略推荐 |
| **M5 5 轴/物理** | 研究级 | 5 轴刀轴优化、力学模型、变形、机床仿真 |

建议从 **M0+M1 垂直切片**入手：打通"画方块 → 生成型腔刀路 → 出 G代码"最小闭环，再横向铺开。

---

## 10. 风险与难点

1. **几何内核稳定性**：OCCT 布尔在"接近相切/共面"的病态输入下会失败，CAM 偏置/残料大量依赖布尔——需提前建"脏几何修复 + 容差"层，实战中最耗时。
2. **刀路算法正确性 > 性能**：先保证偏置曲线、残料计算**对**，再谈并行；别用多线程把算法搞复杂。
3. **5 轴与物理仿真的边界**：刀轴优化和颤振是学术界还在研究的问题，明确"产品功能" vs "研究预留接口"，别让研究级目标拖死项目。
4. **MCP 安全与可信**：AI 改刀路/参数必须强制校验 + 仿真兜底。
5. **许可证**：OCCT 是 LGPL，动态链接没问题；勿静态链接闭源发布。

---

## 11. 待定决策

1. **轴数范围**：先 3 轴（含 3+2 定位）还是直接 5 轴联动？（建议 3 轴起步，5 轴 M5 预留）
2. **物理仿真深度**：SIM 先做几何仿真还是同时上切削力？（建议几何仿真先行，物理仿真 M5）
3. **UI 风格**：FreeCAD 式 Workbench 还是 Fusion 式单一窗口多面板？（影响 App/Gui 框架选型）
