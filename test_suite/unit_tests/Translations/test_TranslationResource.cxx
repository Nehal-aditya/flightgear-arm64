// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Translations: automated tests for the TranslationResource class
 *
 * Some tests in this file depend on particular default translation strings
 * (“engineering English”) and translations in $FG_ROOT/Translations. If these
 * are modified, the changes will have to be reflected here.
 */

#include "config.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include <Main/fg_props.hxx>
#include <Main/globals.hxx>
#include <Main/locale.hxx>
#include <Translations/FGTranslate.hxx>
#include <Translations/TranslationResource.hxx>

#include "test_TranslationResource.hxx"
#include "test_suite/FGTestApi/testGlobals.hxx"

using namespace std::string_literals;

using std::string;

// The en_US strings may differ from the default translation strings (the
// former are found in <target> elements of
// Translations/en_US/FlightGear-nonQt.xlf, the latter in <source> elements of
// the same file and in Translations/default/{auto-extracted,}/*.xml; however,
// this should in general only happen for strings that have plural forms.
void TranslationResourceTests::commonBetweenDefaultTranslationAndEn_US()
{
    auto res = FGTranslate().getResource("options");
    CPPUNIT_ASSERT_EQUAL("General Options"s, res->get("general-options"));
    CPPUNIT_ASSERT_EQUAL("Specify the scenery path(s);"s,
                         res->get("fg-scenery-desc", 0));
    CPPUNIT_ASSERT_EQUAL("Defaults to $FG_ROOT/Scenery"s,
                         res->get("fg-scenery-desc", 1));

    res = FGTranslate().getResource("dialog-exit");
    CPPUNIT_ASSERT_EQUAL("Exit"s, res->get("exit-button-label"));
}

void TranslationResourceTests::test_defaultTranslation()
{
    FGTestApi::setUp::initTestGlobals("test_defaultTranslation", "default");
    commonBetweenDefaultTranslationAndEn_US();
}

void TranslationResourceTests::test_en_US()
{
    FGTestApi::setUp::initTestGlobals("test_en_US", "en_US");
    commonBetweenDefaultTranslationAndEn_US();
}

void TranslationResourceTests::test_fr()
{
    FGTestApi::setUp::initTestGlobals("test_fr", "fr");

    auto res = FGTranslate().getResource("options");
    CPPUNIT_ASSERT_EQUAL("Options générales"s, res->get("general-options"));
    CPPUNIT_ASSERT_EQUAL("Spécifie l'emplacement des répertoires des scènes ;"s,
                         res->get("fg-scenery-desc", 0));
    CPPUNIT_ASSERT_EQUAL("Positionné par défaut à $FG_ROOT/Scenery"s,
                         res->get("fg-scenery-desc", 1));

    res = FGTranslate().getResource("dialog-exit");
    CPPUNIT_ASSERT_EQUAL("Quitter"s, res->get("exit-button-label"));
}

void TranslationResourceTests::test_nonExistentTranslation()
{
    FGTestApi::setUp::initTestGlobals("test_nonExistentTranslation",
                                      "non-existent language");

    // None of the /sim/intl/locale[n] nodes matches the above language,
    // therefore FGLocale::selectLanguage() uses the fallback translation at
    // /sim/intl/locale[0], which is English.
    const auto res = FGTranslate().getResource("options");
    CPPUNIT_ASSERT_EQUAL("General Options"s, res->get("general-options"));
}

void TranslationResourceTests::test_getWithDefault()
{
    FGTestApi::setUp::initTestGlobals("test_getWithDefault", "en");

    auto res = FGTranslate().getResource("options");
    string fetched = res->getWithDefault("general-options", "some default");
    CPPUNIT_ASSERT_EQUAL("General Options"s, fetched);

    fetched = res->getWithDefault("non-existent foobar", "the default");
    CPPUNIT_ASSERT_EQUAL("the default"s, fetched);

    // Change the selected language to French
    globals->get_locale()->selectLanguage("fr");
    res = FGTranslate().getResource("options");

    fetched = res->getWithDefault("general-options", "some default");
    CPPUNIT_ASSERT_EQUAL("Options générales"s, fetched);

    fetched = res->getWithDefault("non-existent foobar", "the default");
    CPPUNIT_ASSERT_EQUAL("the default"s, fetched);
}

void TranslationResourceTests::test_pluralsAndAircraftDomain()
{
    FGTestApi::setUp::initTestGlobals("test_pluralsAndAircraftDomain", "en_US");

    const auto dir = SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Aircraft" / "Test";
    fgSetString("/sim/aircraft-dir"s, dir.utf8Str());

    globals->get_locale()->loadAircraftTranslations();
    auto res = FGTranslate("current-aircraft").getResource("some-resource");

    auto translUnit = res->translationUnit("hello");
    CPPUNIT_ASSERT(!translUnit->getPluralStatus()); // no plural forms here

    string fetched = translUnit->getTranslation();
    CPPUNIT_ASSERT_EQUAL("Hello from the Test aircraft!"s, fetched);

    // String with plural forms. In English, we have singular for 1 and plural
    // for all other non-negative integers, including 0.
    translUnit = res->translationUnit("cats");
    CPPUNIT_ASSERT(translUnit->getPluralStatus()); // the string has plural forms

    fetched = translUnit->getTranslation(0);
    CPPUNIT_ASSERT_EQUAL("%1 cats is not enough cats."s, fetched);

    fetched = translUnit->getTranslation(1);
    CPPUNIT_ASSERT_EQUAL("%1 cat is not enough cats."s, fetched);

    fetched = translUnit->getTranslation(2);
    CPPUNIT_ASSERT_EQUAL("%1 cats is not enough cats."s, fetched);

    // Same thing, but done less efficiently (this involves more lookups)
    fetched = res->getPlural(0, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 cats is not enough cats."s, fetched);

    fetched = res->getPlural(1, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 cat is not enough cats."s, fetched);

    fetched = res->getPlural(2, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 cats is not enough cats."s, fetched);

    // Change the selected language to French
    globals->get_locale()->selectLanguage("fr");
    globals->get_locale()->loadAircraftTranslations();
    res = FGTranslate("current-aircraft").getResource("some-resource");

    translUnit = res->translationUnit("hello");
    CPPUNIT_ASSERT(!translUnit->getPluralStatus());

    fetched = translUnit->getTranslation();
    CPPUNIT_ASSERT_EQUAL("Bonjour depuis l'aéronef Test !"s, fetched);

    // String with plural forms. In French, we have singular for 0 and 1,
    // plural for all other non-negative integers.
    translUnit = res->translationUnit("cats");
    CPPUNIT_ASSERT(translUnit->getPluralStatus());

    fetched = translUnit->getTranslation(0);
    CPPUNIT_ASSERT_EQUAL("%1 chat, ce n'est pas assez de chats."s, fetched);

    fetched = translUnit->getTranslation(1);
    CPPUNIT_ASSERT_EQUAL("%1 chat, ce n'est pas assez de chats."s, fetched);

    fetched = translUnit->getTranslation(2);
    CPPUNIT_ASSERT_EQUAL("%1 chats, ce n'est pas assez de chats."s, fetched);

    // Same thing, but done less efficiently (this involves more lookups)
    fetched = res->getPlural(0, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 chat, ce n'est pas assez de chats."s, fetched);

    fetched = res->getPlural(1, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 chat, ce n'est pas assez de chats."s, fetched);

    fetched = res->getPlural(2, "cats");
    CPPUNIT_ASSERT_EQUAL("%1 chats, ce n'est pas assez de chats."s, fetched);

    fetched = res->getPluralWithDefault(2, "non-existent-id", "the default");
    CPPUNIT_ASSERT_EQUAL("the default"s, fetched);

    fetched = res->getPluralWithDefault(2, "non-existent-id",
                                        "the default", 0 /* explicit index */);
    CPPUNIT_ASSERT_EQUAL("the default"s, fetched);
}

void TranslationResourceTests::test_multipleIndices()
{
    FGTestApi::setUp::initTestGlobals("test_multipleIndices", "en_US");

    const auto dir = SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Aircraft" / "Test";
    fgSetString("/sim/aircraft-dir"s, dir.utf8Str());

    globals->get_locale()->loadAircraftTranslations();
    auto tr = FGTranslate("current-aircraft").getResource("dialog-whatever");

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), tr->getCount("sentence"));
    std::vector<std::string> v = {
        "Make sure the Prince doesn't leave this room until I come and get him.",
        "Not to leave the room even if you come and get him.",
        "No, no. Until I come and get him."
    };

    std::vector<std::string> v2 = tr->getAll("sentence");
    bool equal = std::equal(v.cbegin(), v.cend(), v2.cbegin(), v2.cend());
    CPPUNIT_ASSERT(equal);

    // Change the selected language to French
    globals->get_locale()->selectLanguage("fr");
    globals->get_locale()->loadAircraftTranslations();
    tr = FGTranslate("current-aircraft").getResource("dialog-whatever");

    // There are 3 strings with basicId "sentence" in resource "dialog-whatever"
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), tr->getCount("sentence"));
    v = {"Assurez-vous que le prince ne quitte pas cette pièce avant que je "
         "ne revienne le checher.",
         "Ne pas quitter la pièce même si vous revenez le chercher.",
         "Non, non. Jusqu'à ce que je revienne le chercher."
    };
    v2 = tr->getAll("sentence");
    equal = std::equal(v.cbegin(), v.cend(), v2.cbegin(), v2.cend());
    CPPUNIT_ASSERT(equal);

    // Similar thing, but done less efficiently (this involves more lookups)
    for (std::size_t i = 0; i < v.size(); i++) {
        string fetched = tr->get("sentence", i);
        CPPUNIT_ASSERT_EQUAL(v[i], fetched);
    }
}

// Subroutine called by tests that are run with different FGLocale settings
void TranslationResourceTests::NasalAPI_languageIndependentTests()
{
    std::string badCode = R"(
        var tr = FGTranslate.new("current-aircraft").getResource("some-resource");
        # This string has plural status true...
        var translUnit = tr.translationUnit("cats");
        # ... therefore the cardinalNumber argument is missing here:
        translUnit.getTranslation();
    )";
    bool ok = FGTestApi::executeNasal(badCode);
    CPPUNIT_ASSERT(!ok);

    badCode = R"(
        var tr = FGTranslate.new("current-aircraft").getResource("some-resource");
        # This string has plural status false...
        var translUnit = tr.translationUnit("hello");
        # ... therefore no argument must be provided here:
        translUnit.getTranslation(2);
    )";
    ok = FGTestApi::executeNasal(badCode);
    CPPUNIT_ASSERT(!ok);
}

void TranslationResourceTests::test_NasalAPI_en_US()
{
    FGTestApi::setUp::initTestGlobals("test_NasalAPI_en_US", "en_US");

    globals->get_subsystem_mgr()->bind();
    globals->get_subsystem_mgr()->init();
    FGTestApi::setUp::initStandardNasal();
    globals->get_subsystem_mgr()->postinit();

    const auto dir = SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Aircraft" / "Test";
    fgSetString("/sim/aircraft-dir"s, dir.utf8Str());

    globals->get_locale()->loadAircraftTranslations();

    bool ok = FGTestApi::executeNasal(R"(
        var tr1 = FGTranslate.new().getResource("dialog-exit");
        unitTest.assert_equal("Exit", tr1.get("exit-button-label"));

        var tr2 = FGTranslate.new("current-aircraft")
                    .getResource("some-resource");
        unitTest.assert_equal("Hello from the Test aircraft!",
                              tr2.get("hello"));

        unitTest.assert_equal("%1 cats is not enough cats.",
                              tr2.getPlural(0, "cats"));
        unitTest.assert_equal("%1 cat is not enough cats.",
                              tr2.getPlural(1, "cats"));
        unitTest.assert_equal("%1 cats is not enough cats.",
                              tr2.getPlural(2, "cats"));

        # Prepare for testing getAll() and getCount()
        var expectedVec = [
          "Make sure the Prince doesn't leave this room until I come and get him.",
          "Not to leave the room even if you come and get him.",
          "No, no. Until I come and get him.",
        ];
        var nbSentences = size(expectedVec);

        var tr3 = FGTranslate.new("current-aircraft")
                    .getResource("dialog-whatever");
        var v = tr3.getAll("sentence");
        unitTest.assert_equal(nbSentences, size(v));

        unitTest.assert_equal(nbSentences, tr3.getCount("sentence"));

        # Test TranslationResource.get() with the optional argument provided
        # (an index)
        for (var i = 0; i < nbSentences; i += 1) {
          unitTest.assert_equal(expectedVec[i], tr3.get("sentence", i));
        }

        # Test TranslationResource.getWithDefault()
        unitTest.assert_equal("Hello from the Test aircraft!",
                              tr2.getWithDefault("hello", "the default value"));
        unitTest.assert_equal("the default value",
                              tr2.getWithDefault("non-existent",
                                                 "the default value"));

        unitTest.assert_equal(expectedVec[2],
                              tr3.getWithDefault("sentence",
                                                 "the default value", 2));
        unitTest.assert_equal("the default value",
                              tr3.getWithDefault("non-existent",
                                                 "the default value", 2));

        # Test TranslationResource.getPluralWithDefault()
        unitTest.assert_equal(
          "%1 cats is not enough cats.",
          tr2.getPluralWithDefault(0, "cats", "default"));
        unitTest.assert_equal(
          "%1 cat is not enough cats.",
          tr2.getPluralWithDefault(1, "cats", "default"));
        unitTest.assert_equal(
          "default",
          tr2.getPluralWithDefault(0, "non-existent", "default"));
        unitTest.assert_equal(
          "default",
          tr2.getPluralWithDefault(1, "non-existent", "default"));

        # This string isn't defined with has-plural="true"
        var translUnit = tr2.translationUnit("hello");
        unitTest.assert_equal(0, translUnit.pluralStatus);

        # This string is defined with has-plural="true"
        var translUnit = tr2.translationUnit("cats");

        unitTest.assert_equal("%1 cat(s) is not enough cats.",
                              translUnit.sourceText);
        unitTest.assert_equal(1, translUnit.pluralStatus);
        # 2 plural forms in English: singular and plural
        unitTest.assert_equal(2, translUnit.nbTargetTexts);

        unitTest.assert_equal("%1 cat is not enough cats.",
                              translUnit.getTargetText(0));
        unitTest.assert_equal("%1 cats is not enough cats.",
                              translUnit.getTargetText(1));

        unitTest.assert_equal("%1 cats is not enough cats.",
                              translUnit.getTranslation(0));
        unitTest.assert_equal("%1 cat is not enough cats.",
                              translUnit.getTranslation(1));
        unitTest.assert_equal("%1 cats is not enough cats.",
                              translUnit.getTranslation(2));

        # TranslationResource.translationUnit() with the optional index argument
        var translUnit = tr3.translationUnit("sentence", 2);
        unitTest.assert_equal("No, no. Until I come and get him.",
                              translUnit.getTranslation());
    )");
    CPPUNIT_ASSERT(ok);

    NasalAPI_languageIndependentTests();
}

void TranslationResourceTests::test_NasalAPI_fr_FR()
{
    FGTestApi::setUp::initTestGlobals("test_NasalAPI_fr_FR", "fr_FR");

    globals->get_subsystem_mgr()->bind();
    globals->get_subsystem_mgr()->init();
    FGTestApi::setUp::initStandardNasal();
    globals->get_subsystem_mgr()->postinit();

    const auto dir = SGPath::fromUtf8(FG_TEST_SUITE_DATA) / "Aircraft" / "Test";
    fgSetString("/sim/aircraft-dir"s, dir.utf8Str());

    globals->get_locale()->loadAircraftTranslations();

    bool ok = FGTestApi::executeNasal(R"(
        var tr1 = FGTranslate.new().getResource("dialog-exit");
        unitTest.assert_equal("Quitter", tr1.get("exit-button-label"));

        var tr2 = FGTranslate.new("current-aircraft")
                    .getResource("some-resource");
        unitTest.assert_equal("Bonjour depuis l'aéronef Test !",
                              tr2.get("hello"));

        unitTest.assert_equal("%1 chat, ce n'est pas assez de chats.",
                              tr2.getPlural(0, "cats"));
        unitTest.assert_equal("%1 chat, ce n'est pas assez de chats.",
                              tr2.getPlural(1, "cats"));
        unitTest.assert_equal("%1 chats, ce n'est pas assez de chats.",
                              tr2.getPlural(2, "cats"));

        # Prepare for testing getAll() and getCount()
        var expectedVec = [
          "Assurez-vous que le prince ne quitte pas cette pièce avant que je ne revienne le checher.",
          "Ne pas quitter la pièce même si vous revenez le chercher.",
          "Non, non. Jusqu'à ce que je revienne le chercher.",
        ];
        var nbSentences = size(expectedVec);

        var tr3 = FGTranslate.new("current-aircraft")
                    .getResource("dialog-whatever");
        var v = tr3.getAll("sentence");
        unitTest.assert_equal(nbSentences, size(v));

        unitTest.assert_equal(nbSentences, tr3.getCount("sentence"));

        # Test TranslationResource.get() with the optional argument provided
        # (an index)
        for (var i = 0; i < nbSentences; i += 1) {
          unitTest.assert_equal(expectedVec[i], tr3.get("sentence", i));
        }

        # Test TranslationResource.getWithDefault()
        unitTest.assert_equal("Bonjour depuis l'aéronef Test !",
                              tr2.getWithDefault("hello", "the default value"));
        unitTest.assert_equal("the default value",
                              tr2.getWithDefault("non-existent",
                                                 "the default value"));

        unitTest.assert_equal(expectedVec[2],
                              tr3.getWithDefault("sentence",
                                                "the default value", 2));
        unitTest.assert_equal("the default value",
                              tr3.getWithDefault("non-existent",
                                                "the default value", 2));

        # Test TranslationResource.getPluralWithDefault()
        unitTest.assert_equal(
          "%1 chat, ce n'est pas assez de chats.",
          tr2.getPluralWithDefault(0, "cats", "default"));
        unitTest.assert_equal(
          "%1 chats, ce n'est pas assez de chats.",
          tr2.getPluralWithDefault(2, "cats", "default"));
        unitTest.assert_equal(
          "default",
          tr2.getPluralWithDefault(0, "non-existent", "default"));
        unitTest.assert_equal(
          "default",
          tr2.getPluralWithDefault(2, "non-existent", "default"));

        # This string isn't defined with has-plural="true"
        var translUnit = tr2.translationUnit("hello");
        unitTest.assert_equal(0, translUnit.pluralStatus);

        # This string is defined with has-plural="true"
        var translUnit = tr2.translationUnit("cats");

        unitTest.assert_equal(
          "%1 cat(s) is not enough cats.", translUnit.sourceText);
        unitTest.assert_equal(1, translUnit.pluralStatus);
        # 2 plural forms in French: singular and plural
        unitTest.assert_equal(2, translUnit.nbTargetTexts);

        unitTest.assert_equal(
          "%1 chat, ce n'est pas assez de chats.", translUnit.getTargetText(0));
        unitTest.assert_equal(
          "%1 chats, ce n'est pas assez de chats.", translUnit.getTargetText(1));

        unitTest.assert_equal("%1 chat, ce n'est pas assez de chats.",
                              translUnit.getTranslation(0));
        unitTest.assert_equal("%1 chat, ce n'est pas assez de chats.",
                              translUnit.getTranslation(1));
        unitTest.assert_equal("%1 chats, ce n'est pas assez de chats.",
                              translUnit.getTranslation(2));

        # TranslationResource.translationUnit() with the optional index argument
        var translUnit = tr3.translationUnit("sentence", 2);
        unitTest.assert_equal(
          "Non, non. Jusqu'à ce que je revienne le chercher.",
          translUnit.getTranslation());
    )");
    CPPUNIT_ASSERT(ok);

    NasalAPI_languageIndependentTests();
}

void TranslationResourceTests::tearDown()
{
    FGTestApi::tearDown::shutdownTestGlobals();
}
