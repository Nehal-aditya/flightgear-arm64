// SPDX-FileCopyrightText: 1999-2002 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "flight.hxx"


/**
 *  UFO.cxx -- interface to the "UFO" flight model
 *
 * Slightly modified from MagicCarpet.cxx by Jonathan Polley, April 2002
 */

class FGUFO : public FGInterface
{
private:
    class lowpass
    {
    private:
        static double _dt;
        double _coeff;
        double _last;
        bool _initialized;

    public:
        lowpass(double coeff) : _coeff(coeff), _initialized(false) {}
        static inline void set_delta(double dt) { _dt = dt; }
        double filter(double value) {
            if (!_initialized) {
                _initialized = true;
                return _last = value;
            }
            double c = _dt / (_coeff + _dt);
            return _last = value * c + _last * (1.0 - c);
        }
    };

    lowpass *Throttle;
    lowpass *Aileron;
    lowpass *Elevator;
    lowpass *Rudder;
    lowpass *Aileron_Trim;
    lowpass *Elevator_Trim;
    lowpass *Rudder_Trim;
    SGPropertyNode_ptr Speed_Max;

public:
    FGUFO( double dt );
    ~FGUFO();

    // Subsystem API.
    void init() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "ufo"; }
};
