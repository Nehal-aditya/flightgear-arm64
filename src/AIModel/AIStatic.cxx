// FGAIStatic - FGAIBase-derived class creates an AI static object
//
// Written by David Culp, started Jun 2005.
//
// SPDX-FileCopyrightText: Copyright (C) 2005  David P. Culp - davidculp2@comcast.net
// SPDX-License-Identifier: GPL-2.0-or-later

#include <config.h>

#include <Main/fg_props.hxx>

#include "AIStatic.hxx"


FGAIStatic::FGAIStatic() : FGAIBase(object_type::otStatic, false) {
    _searchOrder = ModelSearchOrder::DATA_ONLY;
}


void FGAIStatic::update(double dt) {
   FGAIBase::update(dt);
   Transform();
}
