// environment-ctrl.hxx -- controller for environment information.
//
// Written by David Megginson, started May 2002.
//
// SPDX-FileCopyrightText: 2002 David Megginson <david@megginson.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>

namespace Environment {

class LayerInterpolateController : public SGSubsystem
{
public:
    static LayerInterpolateController * createInstance( SGPropertyNode_ptr rootNode );
};

} // namespace Environment
