// simulates ridge lift
//
// Written by Patrice Poly
//
// Entirely based  on the paper :
// http://carrier.csi.cam.ac.uk/forsterlewis/soaring/sim/fsx/dev/sim_probe/sim_probe_paper.html
// by Ian Forster-Lewis, University of Cambridge, 26th December 2007
//
// SPDX-FileCopyrightText: 2009 Patrice Poly <p.polypa@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later


#pragma once

#include <string>

#include <simgear/props/tiedpropertylist.hxx>

class FGRidgeLift : public SGSubsystem
{
public:
    FGRidgeLift();
    ~FGRidgeLift();

    // Subsystem API.
    void bind() override;
    void init() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "ridgelift"; }

    inline double getStrength() const { return strength; };

    inline double get_probe_elev_m( int index ) const { return probe_elev_m[index]; };
    inline double get_probe_lat_deg( int index ) const { return probe_lat_deg[index]; };
    inline double get_probe_lon_deg( int index ) const { return probe_lon_deg[index]; };
    inline double get_slope( int index ) const { return slope[index]; };

private:
    static const double dist_probe_m[5];

    double strength;
    double timer;

    double probe_lat_deg[5];
    double probe_lon_deg[5];
    double probe_elev_m[5];

    double slope[4];

    double lift_factor;

    SGPropertyNode_ptr _enabled_node;
    SGPropertyNode_ptr _ridge_lift_fps_node;

    SGPropertyNode_ptr _surface_wind_from_deg_node;
    SGPropertyNode_ptr _surface_wind_speed_node;

    SGPropertyNode_ptr _user_altitude_agl_ft_node;
    SGPropertyNode_ptr _user_longitude_node;
    SGPropertyNode_ptr _user_latitude_node;
    SGPropertyNode_ptr _ground_elev_node;

    simgear::TiedPropertyList _tiedProperties;
};
