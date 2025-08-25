// gsdi.cxx - Ground Speed Drift Angle Indicator (known as GSDI or GSDA)
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2006 Melchior Franz

#pragma once

#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>


/**
 * Input properties:
 *
 * /instrumentation/gsdi[n]/serviceable
 * /velocities/uBody-fps
 * /velocities/vBody-fps
 *
 * Output properties:
 *
 * /instrumentation/gsdi[n]/drift-u-kt
 * /instrumentation/gsdi[n]/drift-v-kt
 * /instrumentation/gsdi[n]/drift-speed-kt
 * /instrumentation/gsdi[n]/drift-angle-deg
 */
class GSDI : public SGSubsystem
{
public:
    GSDI(SGPropertyNode *node);
    virtual ~GSDI();

    // Subsystem API.
    void init() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "gsdi"; }

private:
    std::string _name;
    unsigned int _num;

    // input
    SGPropertyNode_ptr _serviceableN;
    SGPropertyNode_ptr _ubodyN;
    SGPropertyNode_ptr _vbodyN;

    // output
    SGPropertyNode_ptr _drift_uN;
    SGPropertyNode_ptr _drift_vN;
    SGPropertyNode_ptr _drift_speedN;
    SGPropertyNode_ptr _drift_angleN;
};
