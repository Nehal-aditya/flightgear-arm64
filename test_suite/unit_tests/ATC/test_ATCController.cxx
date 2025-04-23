/*
 * SPDX-FileName: test_ATCController.cxx
 * SPDX-FileComment: Tests for the ATCController
 * SPDX-FileCopyrightText: Copyright (C) 2024 Keith Paterson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "test_ATCController.hxx"

#include "config.h"
#include "test_suite/FGTestApi/testGlobals.hxx"

#include "ATC/atc_mgr.hxx"
#include "ATC/ATCController.hxx"
#include "ATC/GroundController.hxx"
#include <AIModel/AIManager.hxx>
#include <AIModel/performancedb.hxx>
#include <Airports/airport.hxx>
#include <Airports/airportdynamicsmanager.hxx>
#include <Airports/dynamics.hxx>

#include <Main/globals.hxx>

#include <string>
#include <memory>

using std::string;

/////////////////////////////////////////////////////////////////////////////

class TestATCController : public FGATCController
{
public:
    string getTransponderCode(const string& fltRules)
    {
        return genTransponderCode(fltRules);
    };

    int getFrequency() { return 1; };
    void announcePosition(int id, FGAIFlightPlan* intendedRoute, int currentRoute,
                          double lat, double lon,
                          double hdg, double spd, double alt, double radius, int leg,
                          FGAIAircraft* aircraft) {};
    void updateAircraftInformation(int id, SGGeod geod,
                                   double heading, double speed, double alt, double dt) {};
    void render(bool) {};
    std::string getName() const { return "test"; };
    void update(double) {};
};

// Set up function for each test.
void ATCControllerTests::setUp()
{
    FGTestApi::setUp::initTestGlobals("ATCControllerTests");
    FGAirport::clearAirportsCache();

    globals->get_subsystem_mgr()->add<PerformanceDB>();
    globals->get_subsystem_mgr()->add<FGATCManager>();
    globals->get_subsystem_mgr()->add<FGAIManager>();
    globals->get_subsystem_mgr()->add<flightgear::AirportDynamicsManager>();

    globals->get_subsystem_mgr()->bind();
    globals->get_subsystem_mgr()->init();
    globals->get_subsystem_mgr()->postinit();

}

// Clean up after each test.
void ATCControllerTests::tearDown()
{
    FGTestApi::tearDown::shutdownTestGlobals();
}

void ATCControllerTests::testTransponder()
{
    /*
    FGAirportRef egph = FGAirport::getByIdent("EGPH");

    egph->testSuiteInjectGroundnetXML(SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "EGPH.groundnet.xml");
    FGGroundController testObject(egph->getDynamics());
    */
    TestATCController testController;
    const string val1 = testController.getTransponderCode("FLT");
    const string val2 = testController.getTransponderCode("FLT");
    CPPUNIT_ASSERT(val1.compare(val2));
}

