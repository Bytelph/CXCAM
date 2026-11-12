/****************************************************************************
** Copyright (c) 2016, Fougue SAS <https://www.fougue.pro>
** SPDX-License-Identifier: BSD-2-Clause
****************************************************************************/

#pragma once

#include "base/occ_handle.h"

#include <Graphic3d_GraphicDriver.hxx>

#include <functional>

namespace Mayo {

using FunctionCreateGraphicsDriver = std::function<OccHandle<Graphic3d_GraphicDriver>()>;

// Creates a graphics driver via the installed factory(default: desktop OpenGL)
OccHandle<Graphic3d_GraphicDriver> graphicsCreateDriver();

// Installs a custom graphics driver factory(eg Qt-managed buffer swap / offscreen FBO)
void setFunctionCreateGraphicsDriver(FunctionCreateGraphicsDriver fn);

} // namespace Mayo
