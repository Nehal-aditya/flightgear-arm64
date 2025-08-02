// SPDX-FileCopyrightText: 2025 James Turner <james@flightgear.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "testFGInterface.hxx"

#include <cmath>
#include <cstring>
#include <memory>

#include "cppunit/TestAssert.h"
#include "simgear/math/SGGeod.hxx"
#include "simgear/structure/exception.hxx"
#include "test_suite/FGTestApi/testGlobals.hxx"

#include <FDM/flight.hxx>

class TestFDM : public FGInterface
{
public:
    void testSetPosition(const SGGeod& geod)
    {
        _updatePosition(geod);
    }
};

FGInterfaceTests::FGInterfaceTests() = default;
FGInterfaceTests::~FGInterfaceTests() = default;

// Set up function for each test.
void FGInterfaceTests::setUp()
{
    FGTestApi::setUp::initTestGlobals("FGInterface");

    flight.reset(new TestFDM);
    flight->bind();
    // normally called by derived FDM class
    flight->common_init();
}

// Clean up after each test.
void FGInterfaceTests::tearDown()
{
    flight->unbind();
    FGTestApi::tearDown::shutdownTestGlobals();
}

// void FGInterfaceTests::testClamp()
// {
//     fgSetDouble("/controls/flight/elevator", 0.5);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, fgGetDouble("/controls/flight/elevator"), 1e-3);

//     fgSetDouble("/controls/flight/elevator", 1.0);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

//     // check clamping
//     fgSetDouble("/controls/flight/elevator", 1.1);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

//     controls->set_elevator(-9999.0);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

//     fgSetDouble("/controls/flight/elevator", 999);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

//     // check set after clamped
//     fgSetDouble("/controls/flight/elevator", -0.5);
//     CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, fgGetDouble("/controls/flight/elevator"), 1e-3);
// }

void FGInterfaceTests::testNANRejection()
{
    CPPUNIT_ASSERT_THROW(flight->set_Altitude(std::nan("")), sg_range_exception);

    const SGGeod nanGeod = SGGeod::fromDegFt(std::nan(""), std::nan(""), std::nan(""));
    CPPUNIT_ASSERT_THROW(flight->testSetPosition(nanGeod), sg_range_exception);

    CPPUNIT_ASSERT_THROW(flight->testSetPosition(SGGeod::invalid()), sg_range_exception);

    const SGGeod stupidGeod = SGGeod::fromDeg(-900, 10000.0);
    CPPUNIT_ASSERT_THROW(flight->testSetPosition(stupidGeod), sg_range_exception);
}
