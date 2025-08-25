/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * SPDX-FileCopyrightText: 2008 Vivian Meazza
 *
 * Radar Altimeter
 *  Written by Vivian MEAZZA, started Feb 2008.
*/

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/props/props.hxx>
#include <simgear/math/SGMath.hxx>

class RadarAltimeter : public SGSubsystem
{
public:
    RadarAltimeter ( SGPropertyNode *node );
    virtual ~RadarAltimeter ();

    // Subsystem API.
    void init() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "radar-altimeter"; }

private:
    void update_altitude();
    void updateSetHeight();

    double getDistanceAntennaToHit(const SGVec3d& h) const;
    SGVec3d getCartAntennaPos()const;

    SGVec3d rayVector(double az, double el) const;

    SGPropertyNode_ptr _Instrument;
    SGPropertyNode_ptr _user_alt_agl_node;
    SGPropertyNode_ptr _rad_alt_warning_node;
    SGPropertyNode_ptr _serviceable_node;
    SGPropertyNode_ptr _sceneryLoaded;

    SGVec3d _antennaOffset; // in aircraft local XYZ frame

    std::string _name;
    int _num;
    double _time;
    double _interval;

    double _min_radalt;
};
