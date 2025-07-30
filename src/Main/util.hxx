// util.hxx - general-purpose utility functions.
// SPDX-FileCopyrightText: 2002 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <simgear/misc/sg_path.hxx>

/**
 * Move a value towards a target.
 *
 * This function was originally written by Alex Perry.
 *
 * @param current The current value.
 * @param target The target value.
 * @param timeratio The percentage of smoothing time that has passed
 *        (elapsed time/smoothing time)
 * @return The new value.
 */
double fgGetLowPass (double current, double target, double timeratio);


/**
 * Set the read and write lists of allowed paths patterns for SGPath::validate()
 */
void fgInitAllowedPaths();

namespace flightgear
{
    /**
     * @brief getAircraftAuthorsText - get the aircraft authors as a single
     * string value. This will combine the new (structured) authors data if
     * present, otherwise just return the old plain string
     * @return
     */
    std::string getAircraftAuthorsText();

    /**
     * @brief low-pass filter for bearings, etc in degrees. Inputs can have any sign
     * and output is always in the range [-180.0 .. 180.0]. Ranges of inputs do not
     * have to be consistent, they will be normalised before filtering.
     *
     * @param current - current value in degrees
     * @param target  - target (new) value in degrees.
     * @param timeratio -
     */
    double lowPassPeriodicDegreesSigned(double current, double target, double timeratio);

    /**
     * @brief low-pass filter for bearings, etc in degrees. Inputs can have any sign
     * and output is always in the range [0.0 .. 360.0]. Ranges of inputs do not
     * have to be consistent, they will be normalised before filtering.
     *
     * @param current - current value in degrees
     * @param target  - target (new) value in degrees.
     * @param timeratio -
     */
    double lowPassPeriodicDegreesPositive(double current, double target, double timeratio);

    /**
    * @brief exponential filter
    *
    * @param current - current value
    * @param target  - target (new) value
    * @param timeratio -
    */
    double filterExponential(double current, double target, double timeratio);
    } // namespace flightgear
