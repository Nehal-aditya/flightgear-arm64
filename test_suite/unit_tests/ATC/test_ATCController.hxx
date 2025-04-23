/*
 * SPDX-FileName: test_ATCController.hxx
 * SPDX-FileComment: Tests for the ATCController
 * SPDX-FileCopyrightText: Copyright (C) 2024 Keith Paterson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <memory>

#include <simgear/props/props.hxx>
#include <simgear/math/SGRect.hxx>
#include <simgear/structure/SGSharedPtr.hxx>

// The unit tests for the QuadTree.
class ATCControllerTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(ATCControllerTests);
    CPPUNIT_TEST(testTransponder);
    CPPUNIT_TEST_SUITE_END();


public:
    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // The tests.
    /**Filling of the Quadtree without split*/
    void testTransponder();
};
