// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Container class for a string and its translation
 */

#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <simgear/debug/logstream.hxx>
#include <simgear/nasal/cppbind/Ghost.hxx>
#include <simgear/nasal/cppbind/NasalCallContext.hxx>

#include <Main/locale.hxx>
#include <Main/globals.hxx>

#include "TranslationUnit.hxx"

using flightgear::LanguageInfo;

TranslationUnit::TranslationUnit(const std::string sourceText,
                                 const std::vector<std::string> targetTexts,
                                 bool hasPlural)
    : _sourceText(std::move(sourceText)),
      _targetTexts(std::move(targetTexts)),
      _hasPlural(hasPlural)
{ }

std::string TranslationUnit::getSourceText() const
{
    return _sourceText;
}

void TranslationUnit::setSourceText(std::string text)
{
    _sourceText = std::move(text);
}

std::string TranslationUnit::getTargetText(int pluralFormIndex) const
{
    if (static_cast<std::size_t>(pluralFormIndex) >= _targetTexts.size()) {
        return {};
    }

    return _targetTexts[pluralFormIndex];
}

std::size_t TranslationUnit::getNumberOfTargetTexts() const
{
    return _targetTexts.size();
}

void TranslationUnit::setTargetText(int pluralFormIndex, std::string text)
{
    if (static_cast<std::size_t>(pluralFormIndex) >= _targetTexts.size()) {
        _targetTexts.resize(pluralFormIndex + 1);
    }

    _targetTexts[pluralFormIndex] = std::move(text);
}

void TranslationUnit::setTargetTexts(std::vector<std::string> texts)
{
    _targetTexts = std::move(texts);
}

bool TranslationUnit::getPluralStatus() const
{
    return _hasPlural;
}

void TranslationUnit::setPluralStatus(int hasPlural)
{
    _hasPlural = hasPlural;
}

std::string TranslationUnit::getTranslation() const
{
    const std::size_t nbTargetTexts = getNumberOfTargetTexts();
    std::string res;            // empty result by default

    if (nbTargetTexts == 0) { // e.g., in the default translation
        res = getSourceText();
    } else {
        res = getTargetText(0);

        if (res.empty()) {      // not translated
            res = getSourceText();
        }
    }

    return res;
}

std::string TranslationUnit::getTranslation(intType cardinalNumber) const
{
    if (!getPluralStatus()) {
        SG_LOG(SG_GENERAL, SG_WARN,
               "TranslationUnit::getTranslation(intType cardinalNumber) called "
               "for “" << getSourceText() << "”), however this string wasn't "
               "declared with has-plural=\"true\" in the default translation");
        return getSourceText();
    }

    const std::size_t nbTargetTexts = getNumberOfTargetTexts();
    if (nbTargetTexts == 0) { // e.g., in the default translation
        return getSourceText();
    }

    const std::string languageId = globals->get_locale()->getLanguageId();
    const std::size_t pluralFormIndex =
        LanguageInfo::getPluralFormIndex(languageId, cardinalNumber);
    assert(pluralFormIndex < nbTargetTexts);

    std::string res = getTargetText(pluralFormIndex);

    if (res.empty()) {          // not translated
        res = getSourceText();
    }

    return res;
}

static naRef f_getTranslation(const TranslationUnit& translUnit,
                              nasal::CallContext ctx)
{
    using intType = LanguageInfo::intType;
    intType cardinalNumber;

    switch (ctx.argc) {
    case 0:
        if (translUnit.getPluralStatus()) {
            ctx.runtimeError(
                "TranslationUnit has plural status 1, therefore its "
                "getTranslation() method requires an argument");
        }
        return ctx.to_nasal(translUnit.getTranslation());
    case 1:
        if (!translUnit.getPluralStatus()) {
            ctx.runtimeError(
                "TranslationUnit has plural status 0, therefore its "
                "getTranslation() method must be called with no argument");
         }
        cardinalNumber = ctx.requireArg<intType>(0);
        return ctx.to_nasal(translUnit.getTranslation(cardinalNumber));
    default:
        ctx.runtimeError(
            "TranslationUnit.getTranslation() or "
            "TranslationUnit.getTranslation(cardinalNumber)");
    }

    return {};                  // unreachable
}

static naRef f_getTargetText(const TranslationUnit& translUnit,
                             nasal::CallContext ctx)
{
    if (ctx.argc > 1) {
        ctx.runtimeError("TranslationUnit.getTargetText([index])");
    }

    const auto index = ctx.getArg<std::size_t>(0); // the index defaults to 0

    if (translUnit.getNumberOfTargetTexts() == 0) {
        ctx.runtimeError(
            "cannot call getTargetText() on a TranslationUnit that has "
            "no target texts");
    } else if (index >= translUnit.getNumberOfTargetTexts()) {
        ctx.runtimeError(
            "invalid target text index %d: TranslationUnit only has %d "
            "target texts", index, translUnit.getNumberOfTargetTexts());
    }

    return ctx.to_nasal(translUnit.getTargetText(index));
}

// Static member function
void TranslationUnit::setupGhost()
{
    using TranslationUnitRef = std::shared_ptr<TranslationUnit>;
    using NasalTranslationUnit = nasal::Ghost<TranslationUnitRef>;

    NasalTranslationUnit::init("TranslationUnit")
        .member("sourceText", &TranslationUnit::getSourceText)
        .member("pluralStatus", &TranslationUnit::getPluralStatus)
        .member("nbTargetTexts", &TranslationUnit::getNumberOfTargetTexts)
        .method("getTargetText", &f_getTargetText)
        .method("getTranslation", &f_getTranslation);
}
