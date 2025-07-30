// pavement.cxx - class to represent complex taxiway specified in v850 apt.dat
//
// SPDX-FileCopyrightText: 2009 Frederic Bouvier
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "pavement.hxx"

FGPavement::FGPavement(PositionedID aGuid, const std::string& aIdent, const SGGeod& aPos) :
  FGPositioned(aGuid, PAVEMENT, aIdent, aPos)
{
}

void FGPavement::addNode(const SGGeod &aPos, bool aClose, bool aLoop, int aPaintCode, int aLightCode)
{
  mNodes.push_back(new SimpleNode(aPos, aClose, aLoop, aPaintCode, aLightCode));
}

void FGPavement::addBezierNode(const SGGeod &aPos, const SGGeod &aCtrlPt, bool aClose, bool aLoop, int aPaintCode, int aLightCode)
{
  mNodes.push_back(new BezierNode(aPos, aCtrlPt, aClose, aLoop, aPaintCode, aLightCode));
}
