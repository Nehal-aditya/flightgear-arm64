// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Automated tests for the FGLocale class
 */

#include "config.h"

#include <string>

#include <simgear/props/props.hxx>
#include <simgear/props/props_io.hxx>

#include <Add-ons/AddonManager.hxx>
#include <Main/fg_props.hxx>
#include <Main/globals.hxx>
#include <Main/locale.hxx>

#include "test_FGLocale.hxx"
#include "test_suite/FGTestApi/testGlobals.hxx"

using namespace std::string_literals;
namespace props = simgear::props;

using std::string;
using flightgear::addons::AddonManager;

// Tests for FGLocale::clear()
void FGLocaleTests::test_clear()
{
    // Tell initTestGlobals() not to initialize FGLocale
    FGTestApi::setUp::initTestGlobals("test_clear", {}, false);
    const auto locale = globals->get_locale();

    SGPropertyNode* intl = globals->get_props()->getNode("/sim/intl");
    CPPUNIT_ASSERT(intl != nullptr);

    // Take a snapshot of the /sim/intl tree
    SGPropertyNode_ptr snapshot = new SGPropertyNode();
    CPPUNIT_ASSERT(copyProperties(intl, snapshot));

    // Test that FGLocale::clear() restores /sim/intl to its pristine state
    for (const string language: {"default", "en", "fr"}) {
        locale->selectLanguage(language);
        locale->clear();
        CPPUNIT_ASSERT(props::Compare()(snapshot, intl));
    }

    // Now with aircraft translations
    const auto acDir = SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Aircraft" / "Test";
    fgSetString("/sim/aircraft-dir", acDir.utf8Str());

    for (const string language: {"default", "en", "fr"}) {
        locale->selectLanguage(language);
        locale->loadAircraftTranslations();
        locale->clear();
        CPPUNIT_ASSERT(props::Compare()(snapshot, intl));
    }

    // Now with add-on translations
    const auto& addonManager = AddonManager::createInstance();
    const auto addonDir =
        SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Add-ons" / "Test";
    addonManager->registerAddon(addonDir);

    for (const string language: {"default", "en", "fr"}) {
        locale->selectLanguage(language);
        locale->loadAddonTranslations();
        locale->clear();
        CPPUNIT_ASSERT(props::Compare()(snapshot, intl));
    }

    // The following is useful for finding the differences when the property
    // trees differ.
    //
    // globals->get_subsystem_mgr()->bind();
    // globals->get_subsystem_mgr()->init();
    // FGTestApi::setUp::initStandardNasal();
    // globals->get_subsystem_mgr()->postinit();
    //
    // bool ok = FGTestApi::executeNasal(R"(
    //     print(); props.dump(props.getNode("/sim/intl")); print();
    // )");
    // CPPUNIT_ASSERT(ok);
}

void FGLocaleTests::tearDown()
{
    FGTestApi::tearDown::shutdownTestGlobals();
}
