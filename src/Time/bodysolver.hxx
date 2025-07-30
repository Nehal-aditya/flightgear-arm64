/*
 * bodysolver.hxx - given a location on earth and a time of day/date,
 *                  find the number of seconds to various solar system body
 *                  positions.
 *
 * Written by Curtis Olson, started September 2003.
 *
 * SPDX-FileCopyrightText: 2003 Curtis L. Olson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <simgear/compiler.h>

#include <ctime>

class SGGeod;

/**
 * Given the current unix time in seconds, calculate seconds to the
 * specified solar system body angle (relative to straight up.)  Also
 * specify if we want the angle while the body is ascending or descending.
 * For instance noon is when the sun angle is 0 (or the closest it can
 * get.)  Dusk is when the sun angle is 90 and descending.  Dawn is
 * when the sun angle is 90 and ascending.
 */
time_t fgTimeSecondsUntilBodyAngle( time_t cur_time,
                                   const SGGeod& loc,
                                   double target_angle_deg,
                                   bool ascending,
                                   bool sun_not_moon );

/**
 * given a particular time expressed in side real time at prime
 * meridian (GST), compute position on the earth (lat, lon) such that
 * solar system body is directly overhead.  (lat, lon are reported in
 * radians)
 */
void fgBodyPositionGST(double gst, double& lon, double& lat, bool sun_not_moon);
