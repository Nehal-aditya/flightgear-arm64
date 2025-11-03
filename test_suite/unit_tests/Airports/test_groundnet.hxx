/*
 * SPDX-FileName: test_groundnet.hxx
 * SPDX-FileComment: Tests for airport ground handling code
 * SPDX-FileCopyrightText: 2021 Keith Paterson
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <memory>

#include <simgear/props/props.hxx>

class SGGeod;

// The groundnet unit tests.
class GroundnetTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(GroundnetTests);
    CPPUNIT_TEST(testLoad);
    CPPUNIT_TEST(testIntersections);
    CPPUNIT_TEST(testIntersections2);
    CPPUNIT_TEST(testShortestRoute);
    CPPUNIT_TEST(testShortestRouteCrossingRunway);
    CPPUNIT_TEST(testShortestRouteNotCrossingRunway);
    CPPUNIT_TEST(testFind);
    CPPUNIT_TEST(testFindNearestNodeOnRunwayEntry);

    CPPUNIT_TEST_SUITE_END();


public:
    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // The tests.
    void testLoad();
    void testIntersections();
    void testIntersections2();
    void testShortestRoute();
    void testShortestRouteCrossingRunway();
    void testShortestRouteNotCrossingRunway();
    void testFind();
    void testFindNearestNodeOnRunwayEntry();
};
