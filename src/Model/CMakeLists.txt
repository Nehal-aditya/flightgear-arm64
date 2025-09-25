# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2011 James Turner <james@flightgear.org>

include(FlightGearComponent)

set(SOURCES
	acmodel.cxx
	modelmgr.cxx
	validateSharedModels.cxx
	)

set(HEADERS
	acmodel.hxx
	modelmgr.hxx
	validateSharedModels.hxx
	)

flightgear_component(Model "${SOURCES}" "${HEADERS}")
