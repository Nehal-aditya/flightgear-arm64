// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Class for retrieving translated strings
 */

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <simgear/debug/logstream.hxx>

#include <Main/locale.hxx>
#include <Main/globals.hxx>

#include "FGTranslate.hxx"
#include "TranslationDomain.hxx"

using std::string;

namespace flightgear {

FGTranslate::FGTranslate(const std::string& domain)
    : _domain(globals->get_locale()->getDomain(domain))
{ }

FGTranslate& FGTranslate::setDomain(const string& domain)
{
    // This logs a warning if the domain can't be found.
    _domain = globals->get_locale()->getDomain(domain);
    return *this;
}

TranslationDomain::ResourceRef
FGTranslate::getResource(const string& resourceName) const
{
    if (_domain) {
        return _domain->getResource(resourceName);
    }

    return {};
}

std::shared_ptr<TranslationUnit>
FGTranslate::translationUnit(const string& resourceName, const string& basicId,
                             int index) const
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        return resource->translationUnit(basicId, index);
    }

    return {};
}

string FGTranslate::get(const string& resourceName, const string& basicId,
                        int index) const
{
    const auto translUnit = translationUnit(resourceName, basicId, index);

    if (!translUnit) {
        return {};
    }

    if (translUnit->getPluralStatus()) {
        SG_LOG(SG_GENERAL, SG_DEV_ALERT,
               "FGTranslate::get() or FGTranslate::getWithDefault() used on "
               "translatable string '" << resourceName << "/" << basicId <<
               ":" << index << "' defined with has-plural=\"true\" in the "
               "default translation. Use FGTranslate::getPlural() or "
               "FGTranslate::getPluralWithDefault() instead.");
        return translUnit->getSourceText();
    } else {
        return translUnit->getTranslation();
    }
}

string FGTranslate::getPlural(intType cardinalNumber, const string& resourceName,
                              const string& basicId, int index) const
{
    const auto translUnit = translationUnit(resourceName, basicId, index);

    if (!translUnit) {
        return {};
    }

    if (!translUnit->getPluralStatus()) {
        SG_LOG(SG_GENERAL, SG_DEV_ALERT,
               "FGTranslate::getPlural() or FGTranslate::getPluralWithDefault() "
               "used on translatable string '" << resourceName << "/" <<
               basicId << ":" << index << "' that isn't defined with "
               "has-plural=\"true\" in the default translation. Use "
               "FGTranslate::get() or FGTranslate::getWithDefault() instead.");
        return translUnit->getSourceText();
    } else {
        return translUnit->getTranslation(cardinalNumber);
    }

    return {};
}

string FGTranslate::getWithDefault(const string& resource, const string& basicId,
                                   const string& defaultValue, int index) const
{
    const string result = get(resource, basicId, index);

    return result.empty() ? defaultValue : result;
}

string FGTranslate::getPluralWithDefault(
    intType cardinalNumber, const string& resource, const string& basicId,
    const string& defaultValue, int index) const
{
    const string result = getPlural(cardinalNumber, resource, basicId, index);

    return result.empty() ? defaultValue : result;
}

std::vector<string> FGTranslate::getAll(const string& resourceName,
                                        const string& basicId) const
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        return resource->getTranslations(basicId);
    }

    return {};
}

std::size_t FGTranslate::getCount(const string& resourceName,
                                  const string& basicId) const
{
    TranslationDomain::ResourceRef resource = getResource(resourceName);

    if (resource) {
        return resource->getNumberOfStringsWithId(basicId);
    }

    return 0;
}

} // namespace flightgear
