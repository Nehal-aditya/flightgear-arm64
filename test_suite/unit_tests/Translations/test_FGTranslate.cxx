// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Translations: automated tests for FGTranslate
 *
 * Most tests in this file depend on particular default translation strings
 * (“engineering English”) and translations in $FG_ROOT/Translations. If these
 * are modified, the changes will have to be reflected here.
 */

#include "test_FGTranslate.hxx"

#include "config.h"
#include "test_suite/FGTestApi/testGlobals.hxx"

#include <string>

#include <Main/globals.hxx>
#include <Main/locale.hxx>
#include <Translations/FGTranslate.hxx>

using namespace std::string_literals;

using std::string;
using flightgear::FGTranslate;

// The en_US strings may differ from the default translation strings (the
// former are found in <target> elements of
// Translations/en_US/FlightGear-nonQt.xlf, the latter in <source> elements of
// the same file and in Translations/default/{auto-extracted,}/*.xml; however,
// this should in general only happen for strings that have plural forms.
void FGTranslateTests::commonBetweenDefaultTranslationAndEn_US()
{
    string fetched = FGTranslate().get("options", "general-options");
    CPPUNIT_ASSERT_EQUAL(fetched, "General Options"s);

    fetched = FGTranslate("core").get("options", "general-options");
    CPPUNIT_ASSERT_EQUAL(fetched, "General Options"s);

    fetched = FGTranslate().get("options", "fg-scenery-desc", 0);
    CPPUNIT_ASSERT_EQUAL(fetched, "Specify the scenery path(s);"s);

    fetched = FGTranslate().get("options", "fg-scenery-desc", 1);
    CPPUNIT_ASSERT_EQUAL(fetched, "Defaults to $FG_ROOT/Scenery"s);

    fetched = FGTranslate().get("dialog-exit", "exit-button-label");
    CPPUNIT_ASSERT_EQUAL(fetched, "Exit"s);
}

void FGTranslateTests::testFGTranslate_defaultTranslation()
{
    FGTestApi::setUp::initTestGlobals("testFGTranslate_defaultTranslation",
                                      "default");
    commonBetweenDefaultTranslationAndEn_US();
    FGTestApi::tearDown::shutdownTestGlobals();
}

void FGTranslateTests::testFGTranslate_en_US()
{
    FGTestApi::setUp::initTestGlobals("testFGTranslate_en_US", "en_US");
    commonBetweenDefaultTranslationAndEn_US();
    FGTestApi::tearDown::shutdownTestGlobals();
}

void FGTranslateTests::testFGTranslate_fr()
{
    FGTestApi::setUp::initTestGlobals("testFGTranslate_fr", "fr");

    string fetched = FGTranslate().get("options", "general-options");
    CPPUNIT_ASSERT_EQUAL(fetched, "Options générales"s);

    fetched = FGTranslate("core").get("options", "general-options");
    CPPUNIT_ASSERT_EQUAL(fetched, "Options générales"s);

    fetched = FGTranslate().get("dialog-exit", "exit-button-label");
    CPPUNIT_ASSERT_EQUAL(fetched, "Quitter"s);

    fetched = FGTranslate().get("options", "fg-scenery-desc", 0);
    CPPUNIT_ASSERT_EQUAL(fetched,
                         "Spécifie l'emplacement des répertoires des scènes ;"s);

    fetched = FGTranslate().get("options", "fg-scenery-desc", 1);
    CPPUNIT_ASSERT_EQUAL(fetched, "Positionné par défaut à $FG_ROOT/Scenery"s);

    FGTestApi::tearDown::shutdownTestGlobals();
}

void FGTranslateTests::testFGTranslate_nonExistentTranslation()
{
    FGTestApi::setUp::initTestGlobals("testFGTranslate_nonExistentTranslation",
                                      "non-existent language");

    // None of the /sim/intl/locale[n] nodes matches the above language,
    // therefore FGLocale::selectLanguage() uses the fallback translation at
    // /sim/intl/locale[0], which is English.
    const string fetched = FGTranslate().get("options", "general-options");
    CPPUNIT_ASSERT_EQUAL(fetched, "General Options"s);

    FGTestApi::tearDown::shutdownTestGlobals();
}

void FGTranslateTests::testFGTranslate_getWithDefault()
{
    FGTestApi::setUp::initTestGlobals("testFGTranslate_getWithDefault", "en");

    string fetched = FGTranslate().getWithDefault("options", "general-options",
                                                  "some default");
    CPPUNIT_ASSERT_EQUAL(fetched, "General Options"s);

    fetched = FGTranslate().getWithDefault("options", "non-existent foobar",
                                           "the default");
    CPPUNIT_ASSERT_EQUAL(fetched, "the default"s);

    // Change the selected language to French
    globals->get_locale()->clear();
    globals->get_locale()->selectLanguage("fr");

    fetched = FGTranslate().getWithDefault("options", "general-options",
                                           "some default");
    CPPUNIT_ASSERT_EQUAL(fetched, "Options générales"s);

    fetched = FGTranslate().getWithDefault(
        "options", "non-existent foobar", "the default");
    CPPUNIT_ASSERT_EQUAL(fetched, "the default"s);

    FGTestApi::tearDown::shutdownTestGlobals();
}
