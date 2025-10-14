// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Roman Ludwicki

#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// The unit tests.
class VectorTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(VectorTests);
    CPPUNIT_TEST(testRemoveString);
    CPPUNIT_TEST(testRemoveInt);
    CPPUNIT_TEST(testRemoveAt);
    CPPUNIT_TEST(testPop);
    CPPUNIT_TEST(testVecIndex);
    CPPUNIT_TEST(testAppend);
    CPPUNIT_TEST(testRange);
    CPPUNIT_TEST(testSetSize);
    CPPUNIT_TEST(testSubVec);
    CPPUNIT_TEST_SUITE_END();

public:
    // Set up function for each test.
    void setUp();

    // Clean up after each test.
    void tearDown();

    // The tests.
    void testRemoveString();
    void testRemoveInt();
    void testRemoveAt();
    void testPop();
    void testVecIndex();
    void testAppend();
    void testRange();
    void testSetSize();
    void testSubVec();
};
