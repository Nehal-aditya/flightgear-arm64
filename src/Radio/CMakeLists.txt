# SPDX-FileCopyrightText: 2010 James Turner
# SPDX-License-Identifier: GPL-2.0-or-later

include(FlightGearComponent)

set(SOURCES
	antenna.cxx
	radio.cxx
	)

set(HEADERS
	antenna.hxx
	radio.hxx
	)


flightgear_component(Radio "${SOURCES}" "${HEADERS}")
