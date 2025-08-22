// SPDX-FileCopyrightText: Copyright (C) 2018 Edward d'Auvergne
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>


// The unit tests of the Add-on system.
class AddonManagementTests : public CppUnit::TestFixture
{
    // Set up the test suite.
    CPPUNIT_TEST_SUITE(AddonManagementTests);
    CPPUNIT_TEST(testAddon);
    CPPUNIT_TEST(testAddonVersion);
    CPPUNIT_TEST(testAddonVersionSuffix);
    CPPUNIT_TEST_SUITE_END();

public:
    // Set up function for each test.
    void setUp() {}

    // Clean up after each test.
    void tearDown() {}

    // The tests.
    void testAddon();
    void testAddonVersion();
    void testAddonVersionSuffix();
};
