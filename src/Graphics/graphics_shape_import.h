/****************************************************************************
** CXCAM — Graphics 模块：STEP / BRep 几何导入（无 Qt 依赖）
****************************************************************************/

#pragma once

#include <TopoDS_Shape.hxx>

#include <string>

namespace Mayo {

//! 读取 STEP 文件（.step/.stp）为单个 TopoDS_Shape。
//! 失败时返回空 shape（shape.IsNull() == true），并置 *ok（若非空）为 false。
TopoDS_Shape importStepFile(const std::string& filepath, bool* ok = nullptr);

//! 读取原生 BRep 文件（.brep/.rle）为单个 TopoDS_Shape。
//! 失败时返回空 shape，并置 *ok（若非空）为 false。
TopoDS_Shape importBRepFile(const std::string& filepath, bool* ok = nullptr);

} // namespace Mayo
