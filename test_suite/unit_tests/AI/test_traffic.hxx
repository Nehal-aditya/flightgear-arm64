/*
 * SPDX-FileCopyrightText: (C) 2000 James Turner
 * SPDX_FileComment: AI Traffic tests
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "test_suite/FGTestApi/testStringUtils.hxx"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <memory>

#include <simgear/props/props.hxx>

class SGGeod;

class FGAIAircraft;

// The flight plan unit tests.
class TrafficTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(TrafficTests);
    CPPUNIT_TEST(testPushback);
    CPPUNIT_TEST(testPushbackCargo);
    CPPUNIT_TEST(testPushbackCargoInProgress);
    CPPUNIT_TEST(testPushbackCargoInProgressDownWindEast);
    CPPUNIT_TEST(testPushbackCargoInProgressDownWindWest);
    CPPUNIT_TEST(testPushbackCargoInProgressNotBeyond);
    CPPUNIT_TEST(testPushbackCargoInProgressNotBeyondNorth);
    CPPUNIT_TEST(testPushbackCargoInProgressBeyond);
    CPPUNIT_TEST(testPushbackCargoInProgressBeyondNorth);
    CPPUNIT_TEST(testChangeRunway);
    CPPUNIT_TEST(testPushforward);
    CPPUNIT_TEST(testPushforwardSpeedy);
    CPPUNIT_TEST(testPushforwardParkYBBN);
    CPPUNIT_TEST(testPushforwardParkYBBNRepeatGa);
    CPPUNIT_TEST(testPushforwardParkYBBNRepeatGaDelayed);
    CPPUNIT_TEST(testPushforwardParkYBBNRepeatGate);
    CPPUNIT_TEST_SUITE_END();

public:
    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // Pushback Tests
    void testPushback();
    void testPushbackCargo();
    void testPushbackCargoInProgress();
    void testPushbackCargoInProgressDownWindEast();
    void testPushbackCargoInProgressDownWindWest();
    void testPushbackCargoInProgressNotBeyond();
    void testPushbackCargoInProgressNotBeyondNorth();
    void testPushbackCargoInProgressBeyond();
    void testPushbackCargoInProgressBeyondNorth();
    void testChangeRunway();
    //GA Tests with forward push
    void testPushforward();
    void testPushforwardSpeedy();
    void testPushforwardParkYBBN();
    void testPushforwardParkYBBNRepeatGa();
    void testPushforwardParkYBBNRepeatGaDelayed();
    void testPushforwardParkYBBNRepeatGate();

private:
    long currentWorldTime;
    FGAIAircraft* flyAI(SGSharedPtr<FGAIAircraft> aiAircraft, std::string fName);
};
