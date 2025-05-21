// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Translations: automated tests for FGTranslate (header)
 */

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class FGTranslateTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(FGTranslateTests);
    CPPUNIT_TEST(testFGTranslate_defaultTranslation);
    CPPUNIT_TEST(testFGTranslate_en_US);
    CPPUNIT_TEST(testFGTranslate_fr);
    CPPUNIT_TEST(testFGTranslate_nonExistentTranslation);
    CPPUNIT_TEST(testFGTranslate_getWithDefault);
    CPPUNIT_TEST_SUITE_END();

public:
    // Set up function for each test.
    void setUp() {}

    // Clean up after each test.
    void tearDown() {}

    // The tests.
    void testFGTranslate_defaultTranslation();
    void testFGTranslate_fr();
    void testFGTranslate_en_US();
    void testFGTranslate_nonExistentTranslation();
    void testFGTranslate_getWithDefault();

private:
    static void commonBetweenDefaultTranslationAndEn_US();
};
