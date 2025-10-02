// SPDX-FileCopyrightText: (C) 2025  James Turner <james@flightgear.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "test_controls.hxx"

#include <cmath>
#include <cstring>
#include <memory>

#include "cppunit/TestAssert.h"
#include "simgear/structure/exception.hxx"
#include "test_suite/FGTestApi/testGlobals.hxx"

#include <Aircraft/controls.hxx>
#include <Main/fg_props.hxx>

ControlsTests::~ControlsTests() = default;

// Set up function for each test.
void ControlsTests::setUp()
{
    FGTestApi::setUp::initTestGlobals("Controls");
    controls.reset(new FGControls);
    controls->bind();
}

// Clean up after each test.
void ControlsTests::tearDown()
{
    controls->unbind();
    FGTestApi::tearDown::shutdownTestGlobals();
}

void ControlsTests::testClamp()
{
    fgSetDouble("/controls/flight/elevator", 0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, fgGetDouble("/controls/flight/elevator"), 1e-3);

    fgSetDouble("/controls/flight/elevator", 1.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

    // check clamping
    fgSetDouble("/controls/flight/elevator", 1.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

    controls->set_elevator(-9999.0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

    fgSetDouble("/controls/flight/elevator", 999);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, fgGetDouble("/controls/flight/elevator"), 1e-3);

    // check set after clamped
    fgSetDouble("/controls/flight/elevator", -0.5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5, fgGetDouble("/controls/flight/elevator"), 1e-3);
}

void ControlsTests::testNANRejection()
{
    CPPUNIT_ASSERT_THROW(fgSetDouble("/controls/flight/rudder-trim", std::nan("")), sg_range_exception);

    CPPUNIT_ASSERT_THROW(fgSetDouble("/controls/engines/engine[3]/condition", std::nan("")), sg_range_exception);
    CPPUNIT_ASSERT_THROW(fgSetDouble("/controls/engines/engine[0]/throttle", std::nan("")), sg_range_exception);

    CPPUNIT_ASSERT_THROW(controls->set_condition(FGControls::ALL_ENGINES, std::nan("")), sg_range_exception);
    CPPUNIT_ASSERT_THROW(controls->set_aileron(std::nan("")), sg_range_exception);
}
