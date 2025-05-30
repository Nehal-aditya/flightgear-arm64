// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Container class for related translation units
 */

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include <simgear/debug/logstream.hxx>

#include "TranslationResource.hxx"
#include "TranslationUnit.hxx"

using std::string;
using std::vector;

TranslationResource::TranslationResource(std::string name) noexcept
    :    _name(std::move(name))
{ }

void TranslationResource::addTranslationUnit(std::string name, int index,
                                             std::string sourceText,
                                             bool hasPlural)
{
    _map.emplace(KeyType(std::move(name), index),
                 new TranslationUnit(std::move(sourceText), {}, hasPlural));
}

void TranslationResource::setFirstTargetText(
    std::string name, int index, std::string targetText)
{
    const auto key = std::make_pair(std::move(name), index);
    const auto translationUnit = _map[key];

    // If the smart pointer is empty, it means addTranslationUnit() wasn't
    // called for this string, therefore it isn't in the default translation.
    // It's an obsolete string from the XLIFF file being loaded → ignore it.
    if (translationUnit) {
        // Set the first plural form
        translationUnit->setTargetText(0, std::move(targetText));
    }
}

void TranslationResource::setTargetTexts(
    std::string name, int index, std::vector<std::string> targetTexts)
{
    const auto key = std::make_pair(std::move(name), index);
    const auto translationUnit = _map[key];

    // Set the target texts only if this is not an obsolete string (see above)
    if (translationUnit) {
        translationUnit->setTargetTexts(std::move(targetTexts));
    }
}

TranslationResource::TranslationUnitRef
TranslationResource::translationUnit(const std::string& name, int index) const
{
    auto it = _map.find(std::make_pair(name, index));
    if (it != _map.end()) {
        return it->second;
    }

    return {};
}

string TranslationResource::get(const string& basicId, int index) const
{
    const auto translUnit = translationUnit(basicId, index);

    if (!translUnit) {
        return {};
    }

    if (translUnit->getPluralStatus()) {
        SG_LOG(SG_GENERAL, SG_DEV_ALERT,
               "TranslationResource::get() or "
               "TranslationResource::getWithDefault() used on translatable "
               "string '" << _name << "/" << basicId << ":" << index <<
               "' defined with has-plural=\"true\" in the default translation. "
               "Use TranslationResource::getPlural() or "
               "TranslationResource::getPluralWithDefault() instead.");
        return translUnit->getSourceText();
    } else {
        return translUnit->getTranslation();
    }
}

string TranslationResource::getPlural(intType cardinalNumber,
                                      const string& basicId, int index) const
{
    const auto translUnit = translationUnit(basicId, index);

    if (!translUnit) {
        return {};
    }

    if (!translUnit->getPluralStatus()) {
        SG_LOG(SG_GENERAL, SG_DEV_ALERT,
               "TranslationResource::getPlural() or "
               "TranslationResource::getPluralWithDefault() used on "
               "translatable string '" << _name << "/" << basicId << ":" <<
               index << "' that isn't defined with has-plural=\"true\" in the "
               "default translation. Use TranslationResource::get() or "
               "TranslationResource::getWithDefault() instead.");
        return translUnit->getSourceText();
    } else {
        return translUnit->getTranslation(cardinalNumber);
    }

    return {};
}

string TranslationResource::getWithDefault(
    const string& basicId, const string& defaultValue, int index) const
{
    const string result = get(basicId, index);

    return result.empty() ? defaultValue : result;
}

string TranslationResource::getPluralWithDefault(
    intType cardinalNumber, const string& basicId, const string& defaultValue,
    int index) const
{
    const string result = getPlural(cardinalNumber, basicId, index);

    return result.empty() ? defaultValue : result;
}

vector<string> TranslationResource::getAll(const string& name) const
{
    vector<string> result;
    decltype(_map)::const_iterator it;

    for (int index = 0;
         (it = _map.find(std::make_pair(name, index))) != _map.end();
         index++) {
        const auto& transUnit = it->second;
        // Plural form indices all hardcoded to 0
        const string targetText = transUnit->getTargetText(0);
        result.push_back(
            targetText.empty() ? transUnit->getSourceText() : targetText);
    }

    return result;
}

std::size_t TranslationResource::getCount(const string& name) const
{
    std::size_t index = 0;

    while (_map.find(std::make_pair(name, index)) != _map.end()) {
        index++;
    }

    return index;
}
