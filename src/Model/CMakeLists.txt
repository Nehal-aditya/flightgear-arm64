include(FlightGearComponent)

set(SOURCES
	acmodel.cxx
	modelmgr.cxx
	panelnode.cxx
	validateSharedModels.cxx
	)

set(HEADERS
	acmodel.hxx
	modelmgr.hxx
	panelnode.hxx
	validateSharedModels.hxx
	)
    		
flightgear_component(Model "${SOURCES}" "${HEADERS}")