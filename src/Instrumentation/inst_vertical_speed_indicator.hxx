// inst_vertical_speed_indicator.hxx -- Instantaneous VSI (emulation calibrated to standard atmosphere).
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2004 Erik Hofman

#pragma once


#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>


class SGInterpTable;


/**
 * Model an instantaneous VSI tied to the external pressure.
 *
 * Input properties:
 *
 * /instrumentation/inst-vertical-speed-indicator/serviceable
 * /environment/pressure-inhg
 * /environment/pressure-sea-level-inhg
 * /sim/freeze/master
 *
 * Output properties:
 *
 * /instrumentation/inst-vertical-speed-indicator/indicated-speed-fps
 * /instrumentation/inst-vertical-speed-indicator/indicated-speed-fpm
 */
class InstVerticalSpeedIndicator : public SGSubsystem
{
public:
    InstVerticalSpeedIndicator ( SGPropertyNode *node );
    virtual ~InstVerticalSpeedIndicator ();

    // Subsystem API.
    void init() override;
    void reinit() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "inst-vertical-speed-indicator"; }

private:
    std::string _name;
    int _num;

    double _internal_pressure_inhg;
    double _internal_sea_inhg;

    double _speed_ft_per_s;

    SGPropertyNode_ptr _serviceable_node;
    SGPropertyNode_ptr _freeze_node;
    SGPropertyNode_ptr _pressure_node;
    SGPropertyNode_ptr _sea_node;
    SGPropertyNode_ptr _speed_up_node;
    SGPropertyNode_ptr _speed_node;
    SGPropertyNode_ptr _speed_min_node;

    SGInterpTable * _pressure_table;
    SGInterpTable * _altitude_table;
};
