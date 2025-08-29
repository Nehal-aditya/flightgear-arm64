# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2011 James Turner <james@flightgear.org>
include(FlightGearComponent)

set(SOURCES
	SchedFlight.cxx
	Schedule.cxx
	TrafficMgr.cxx
	)

set(HEADERS
	SchedFlight.hxx
	Schedule.hxx
	TrafficMgr.hxx
)


flightgear_component(Traffic "${SOURCES}" "${HEADERS}")
