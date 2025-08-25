// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2002 David Megginson (public domain)
// SPDX-FileCopyrightText: 2023 Benedikt Wolf

#pragma once

#include <Instrumentation/AbstractInstrument.hxx>

#include "gyro.hxx"


/**
 * Model an electrically-powered attitude indicator.
 *
 * Config:
 *   gyro/spin-up-sec     If given, seconds to spin up until power-norm (from 0->100%)
 *   gyro/spin-down-sec   If given, seconds the gyro will loose spin without power (from 100%->0)
 *
 * Input properties:
 *
 * /instrumentation/"name"/config/tumble-flag
 * /instrumentation/"name"/serviceable
 * /instrumentation/"name"/caged-flag
 * /instrumentation/"name"/tumble-norm
 * /orientation/pitch-deg
 * /orientation/roll-deg
 * /systems/electrical/outputs/attitude-indicator-electric
 *
 * Output properties:
 *
 * /instrumentation/"name"/indicated-pitch-deg
 * /instrumentation/"name"/indicated-roll-deg
 * /instrumentation/"name"/tumble-norm
 * /instrumentation/"name"/off-flag
 */
class AttitudeIndicatorElectric : public AbstractInstrument
{
public:
    AttitudeIndicatorElectric ( SGPropertyNode *node );
    virtual ~AttitudeIndicatorElectric ();

    // Subsystem API.
    void init() override;
    void reinit() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "attitude-indicator-electric"; }

private:
    Gyro _gyro;
    double _gyro_spin_up, _gyro_spin_down;

    SGPropertyNode_ptr _tumble_flag_node;
    SGPropertyNode_ptr _caged_node;
    SGPropertyNode_ptr _tumble_node;
    SGPropertyNode_ptr _pitch_in_node;
    SGPropertyNode_ptr _roll_in_node;
    SGPropertyNode_ptr _pitch_int_node;
    SGPropertyNode_ptr _roll_int_node;
    SGPropertyNode_ptr _pitch_out_node;
    SGPropertyNode_ptr _roll_out_node;
    SGPropertyNode_ptr _off_node;
    SGPropertyNode_ptr _spin_node, _gyro_spin_up_node, _gyro_spin_down_node;

    double spin_thresh;
    double max_roll_error;
    double max_pitch_error;
};
