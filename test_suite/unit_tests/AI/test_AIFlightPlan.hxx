/*
 * SPDX-FileName: test_AIFlightPlan.hxx
 * SPDX-FileCopyrightText: Copyright (C) 2020 James Turner
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <AIModel/AIFlightPlan.hxx>
#include <Airports/airport.hxx>

#include <memory>

#include <simgear/props/props.hxx>

class SGGeod;

// The AI flight plan unit tests.
class AIFlightPlanTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(AIFlightPlanTests);
    CPPUNIT_TEST(testAIFlightPlan);
    CPPUNIT_TEST(testAIFlightPlanLeftCircle);
    CPPUNIT_TEST(testAIFlightPlanLoadXML);
    CPPUNIT_TEST(testLeftTurnFlightplanXML);
    CPPUNIT_TEST(testRightTurnFlightplanXML);

    CPPUNIT_TEST(testCreateApproach);
    CPPUNIT_TEST(testCreatePushbackWithRoute);
    CPPUNIT_TEST(testCreatePushbackWithoutRoute);
    CPPUNIT_TEST(testCreatePushForward);
    CPPUNIT_TEST(testCreatePushbackNoRoute);
    CPPUNIT_TEST_SUITE_END();


public:
    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // The tests.
    void testAIFlightPlan();
    void testAIFlightPlanLeftCircle();
    void testAIFlightPlanLoadXML();
    void testLeftTurnFlightplanXML();
    void testRightTurnFlightplanXML();
    // The tests for "create"
    void testCreateApproach();
    void testCreateTaxiRunwayDeparture();
    void testCreatePushbackWithRoute();
    void testCreatePushbackWithoutRoute();
    void testCreatePushForward();
    void testCreatePushbackNoRoute();

private:
    void printWaypoints(FGAIFlightPlan* aiFP);
};
