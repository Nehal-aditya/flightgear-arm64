// SPDX-FileCopyrightText: (C) 2025  James Turner <james@flightgear.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <memory>

#include <simgear/structure/SGSharedPtr.hxx>

#include <Aircraft/controls.hxx>

// The flight plan unit tests.
class ControlsTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(ControlsTests);
    CPPUNIT_TEST(testClamp);
    CPPUNIT_TEST(testNANRejection);
    CPPUNIT_TEST_SUITE_END();

    SGSharedPtr<FGControls> controls;

public:
    ~ControlsTests();

    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // The tests.
    void testClamp();
    void testNANRejection();
};
