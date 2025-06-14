// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Automated tests for FGLocale (header)
 */

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class FGLocaleTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(FGLocaleTests);
    CPPUNIT_TEST(test_clear);
    CPPUNIT_TEST_SUITE_END();

public:
    // Set up function for each test.
    void setUp() {}

    // Clean up after each test.
    void tearDown();

    // The tests.
    void test_clear();
};
