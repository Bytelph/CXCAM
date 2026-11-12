/****************************************************************************
** CXCAM — Graphics 模块：STEP / BRep 几何导入实现
****************************************************************************/

#include "graphics_shape_import.h"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <STEPControl_Reader.hxx>

namespace Mayo {

TopoDS_Shape importStepFile(const std::string& filepath, bool* ok)
{
    if (ok)
        *ok = false;

    STEPControl_Reader reader;
    if (reader.ReadFile(filepath.c_str()) != IFSelect_RetDone)
        return {};

    // 传输所有根实体到内存中的形状集合
    if (reader.TransferRoots() <= 0)
        return {};

    const TopoDS_Shape shape = reader.OneShape();
    if (!shape.IsNull() && ok)
        *ok = true;
    return shape;
}

TopoDS_Shape importBRepFile(const std::string& filepath, bool* ok)
{
    if (ok)
        *ok = false;

    BRep_Builder builder;
    TopoDS_Shape shape;
    if (!BRepTools::Read(shape, filepath.c_str(), builder))
        return {};

    if (!shape.IsNull() && ok)
        *ok = true;
    return shape;
}

} // namespace Mayo
