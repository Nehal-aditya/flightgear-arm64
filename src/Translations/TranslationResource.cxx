// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Container class for related translation units
 */

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include <simgear/debug/logstream.hxx>

#include "TranslationResource.hxx"
#include "TranslationUnit.hxx"

using std::string;
using std::vector;

namespace flightgear
{

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
    SG_LOG(SG_GENERAL, SG_DEBUG,
           "Setting target text for '" << name << ":" << index <<
           "' to '" << targetText << '\'');

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
    SG_LOG(SG_GENERAL, SG_DEBUG,
           "Setting target texts for '" << name << ":" << index << ":\n\n");
    std::for_each(targetTexts.begin(), targetTexts.end(),
                  [](const std::string& t) {
                      SG_LOG(SG_GENERAL, SG_DEBUG, "\t" << t); });

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

vector<string> TranslationResource::getTranslations(const string& name) const
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

int TranslationResource::getNumberOfStringsWithId(const string& name) const
{
    int index = 0;

    while (_map.find(std::make_pair(name, index)) != _map.end()) {
        index++;
    }

    return index;
}

} // namespace flightgear
