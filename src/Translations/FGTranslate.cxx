// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Class for retrieving translated strings
 */

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include <simgear/debug/logstream.hxx>

#include <Main/locale.hxx>
#include <Main/globals.hxx>

#include "FGTranslate.hxx"
#include "LanguageInfo.hxx"
#include "TranslationDomain.hxx"

using std::string;

namespace flightgear {

FGTranslate& FGTranslate::setDomain(string domain)
{
    _domain = std::move(domain);
    return *this;
}

FGTranslate& FGTranslate::setIndex(int index)
{
    _elementIndex = index;
    return *this;
}

FGTranslate& FGTranslate::setCardinalNumber(intType number)
{
    _cardinalNumber = number;
    return *this;
}

TranslationDomain::ResourceRef
FGTranslate::getResource(const string& resourceName) const
{
    // This logs a warning if the domain can't be found.
    const TranslationDomain* domain = globals->get_locale()->getDomain(_domain);

    if (domain) {
        // This logs a warning if the resource can't be found.
        return domain->getResource(resourceName);
    }

    return {};
}

string FGTranslate::get(const string& resourceName, const string& basicId)
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        const string languageId = globals->get_locale()->getLanguageId();
        const std::size_t pluralFormIndex = _cardinalNumber.has_value() ?
            LanguageInfo::getPluralFormIndex(languageId, *_cardinalNumber) : 0;

        return resource->getTranslation(basicId, _elementIndex, pluralFormIndex);
    }

    return {};
}

string FGTranslate::getWithDefault(
    const string& resourceName, const string& basicId, const string& defaultValue)
{
    const string result = get(resourceName, basicId);

    return result.empty() ? defaultValue : result;
}

std::vector<string> FGTranslate::getAll(const string& resourceName,
                                        const string& basicId)
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        return resource->getTranslations(basicId);
    }

    return {};
}

std::size_t FGTranslate::getCount(const string& resourceName,
                                  const string& basicId)
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        return resource->getNumberOfStringsWithId(basicId);
    }

    return 0;
}

} // namespace flightgear
