/*
 * SPDX-FileComment: a vacuum pump connected to the aircraft engine.
 * SPDX-FileCopyrightText: 2002 David Megginson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <simgear/math/sg_types.hxx>
#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>


/**
 * Model a vacuum-pump system.
 *
 * Multiple pumps (i.e. for a multiengine aircraft) can be specified.
 *
 * Input properties:
 *
 * "rpm1"
 * "rpm2"
 * "..."
 * /environment/pressure-inhg
 * /systems/"name"/serviceable
 *
 * Output properties:
 *
 * /systems/"name"/suction-inhg
 */
class VacuumSystem : public SGSubsystem
{
public:
    VacuumSystem( SGPropertyNode *node );
    VacuumSystem( int i );
    virtual ~VacuumSystem ();

    // Subsystem API.
    void bind() override;
    void init() override;
    void reinit() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "vacuum"; }

private:
    std::string _name;
    int _num;
    string_list _rpms;
    double _scale;
    SGPropertyNode_ptr _serviceable_node;
    std::vector<SGPropertyNode_ptr> _rpm_nodes;
    SGPropertyNode_ptr _pressure_node;
    SGPropertyNode_ptr _suction_node;
};
