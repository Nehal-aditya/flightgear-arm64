// FGCommonInput.hxx -- common functions for all Input subsystems
//
// Written by Torsten Dreyer, started August 2009
// Based on work from David Megginson, started May 2001.
//
// Copyright (C) 2009 Torsten Dreyer, Torsten (at) t3r _dot_ de
// Copyright (C) 2001 David Megginson, david@megginson.com
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson <david@megginson.com>
// SPDX-FileCopyrightText: 2009 Torsten Dreyer

#pragma once

#include <vector>
#include <simgear/structure/SGBinding.hxx>
#include <simgear/compiler.h>

#if defined( SG_WINDOWS )
#define TGT_PLATFORM	"windows"
#elif defined ( SG_MAC )
#define TGT_PLATFORM    "mac"
#else
#define TGT_PLATFORM	"unix"
#endif

class FGCommonInput {
public:
    using binding_list_t = SGBindingList;

    /*
   read all "binding" nodes directly under the specified base node and fill the
   vector of SGBinding supplied in binding_list. Reads all the mod-xxx bindings and
   add the corresponding SGBindings.
   */
    static void read_bindings(const SGPropertyNode* base, binding_list_t* binding_list, int modifiers, const std::string& module);
};
