// presets.hxx -- Wrap environment presets
//
// Written by Torsten Dreyer, January 2011
//
// SPDX-FileCopyrightText: 2010 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/Singleton.hxx>
#include <simgear/props/props.hxx>

namespace Environment {

/**
 * @brief A wrapper for presets of environment properties
 * mainly set from the command line with --wind=270@10,
 * visibility=1600 etc.
 */
namespace Presets {

class PresetBase {
public:
    PresetBase( const char * overrideNodePath );
    virtual void disablePreset() { setOverride(false); }
protected:
    void setOverride( bool value );
private:
    std::string _overrideNodePath;
    SGPropertyNode_ptr _overrideNode;
};

class Ceiling : public PresetBase {
public:
    Ceiling();
    void preset( double elevation, double thickness );
private:
    SGPropertyNode_ptr _elevationNode;
    SGPropertyNode_ptr _thicknessNode;
};

typedef simgear::Singleton<Ceiling> CeilingSingleton;

class Turbulence : public PresetBase {
public:
    Turbulence();
    void preset( double magnitude_norm );
private:
    SGPropertyNode_ptr _magnitudeNode;
};

typedef simgear::Singleton<Turbulence> TurbulenceSingleton;

class Wind : public PresetBase {
public:
    Wind();
    void preset( double min_hdg, double max_hdg, double speed, double gust );
private:
    SGPropertyNode_ptr _fromNorthNode;
    SGPropertyNode_ptr _fromEastNode;
};

typedef simgear::Singleton<Wind> WindSingleton;

class Visibility : public PresetBase {
public:
    Visibility();
    void preset( double visibility_m );
private:
    SGPropertyNode_ptr _visibilityNode;
};

typedef simgear::Singleton<Visibility> VisibilitySingleton;

} // namespace Presets

} // namespace Environment
