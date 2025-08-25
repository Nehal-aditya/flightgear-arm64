// vertical_speed_indicator.hxx - a regular VSI.
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2002 David Megginson (public domain)

#pragma once

#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>


/**
 * Model a non-instantaneous VSI tied to the static port.
 *
 * Input properties:
 *
 * /instrumentation/"name"/serviceable
 * "static_port"/pressure-inhg
 *
 * Output properties:
 *
 * /instrumentation/"name"/indicated-speed-fpm
 * /instrumentation/"name"/indicated-speed-mps
 * /instrumentation/"name"/indicated-speed-kts
 */
class VerticalSpeedIndicator : public SGSubsystem
{
public:
    VerticalSpeedIndicator ( SGPropertyNode *node );
    virtual ~VerticalSpeedIndicator ();

    // Subsystem API.
    void init() override;
    void reinit() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "vertical-speed-indicator"; }

private:
    double _casing_pressure_Pa = 0.0;
    double _casing_airmass_kg = 0.0;
    double _casing_density_kgpm3 = 0.0;
    double _orifice_massflow_kgps = 0.0;

    const std::string _name;
    const int _num;
    std::string _static_pressure;
    std::string _static_temperature;

    SGPropertyNode_ptr _serviceable_node;
    SGPropertyNode_ptr _pressure_node;
    SGPropertyNode_ptr _temperature_node;
    SGPropertyNode_ptr _speed_fpm_node;
    SGPropertyNode_ptr _speed_mps_node;
    SGPropertyNode_ptr _speed_kts_node;
};
