// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Class for retrieving translated strings
 */

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

#include "FGTranslate.hxx"
#include "TranslationDomain.hxx"

using std::string;

using flightgear::TranslationDomain;

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

static naRef f_get(const FGTranslate& tr, const nasal::CallContext& ctx)
{
    if (ctx.argc < 2 || ctx.argc > 3) {
        ctx.runtimeError("FGTranslate.get(resource, basicId[, index])");
    }

    const auto resource = ctx.requireArg<std::string>(0);
    const auto basicId = ctx.requireArg<std::string>(1);
    const auto index = ctx.getArg<int>(2); // the index defaults to 0

    return ctx.to_nasal(tr.get(std::move(resource), std::move(basicId),
                               index));
}

static naRef f_getPlural(const FGTranslate& tr, const nasal::CallContext& ctx)
{
    if (ctx.argc < 3 || ctx.argc > 4) {
        ctx.runtimeError(
            "FGTranslate.getPlural(cardinalNumber, resource, basicId[, index])");
    }

    const auto cardinalNumber = ctx.requireArg<FGTranslate::intType>(0);
    const auto resource = ctx.requireArg<std::string>(1);
    const auto basicId = ctx.requireArg<std::string>(2);
    const auto index = ctx.getArg<int>(3); // the index defaults to 0

    return ctx.to_nasal(tr.getPlural(cardinalNumber, std::move(resource),
                                     std::move(basicId), index));
}

static naRef f_getWithDefault(const FGTranslate& tr,
                              const nasal::CallContext& ctx)
{
    if (ctx.argc < 3 || ctx.argc > 5) {
        ctx.runtimeError("FGTranslate.getWithDefault(resource, basicId, "
                         "defaultValue[, index])");
    }

    const auto resource = ctx.requireArg<std::string>(0);
    const auto basicId = ctx.requireArg<std::string>(1);
    const auto defaultValue = ctx.requireArg<std::string>(2);
    const auto index = ctx.getArg<int>(3); // the index defaults to 0

    return ctx.to_nasal(
        tr.getWithDefault(std::move(resource), std::move(basicId),
                          std::move(defaultValue), index));
}

static naRef f_getPluralWithDefault(const FGTranslate& tr,
                                    const nasal::CallContext& ctx)
{
    if (ctx.argc < 3 || ctx.argc > 5) {
        ctx.runtimeError(
            "FGTranslate.getPluralWithDefault(cardinalNumber, resource, "
            "basicId, defaultValue[, index])");
    }

    const auto cardinalNumber = ctx.requireArg<FGTranslate::intType>(0);
    const auto resource = ctx.requireArg<std::string>(1);
    const auto basicId = ctx.requireArg<std::string>(2);
    const auto defaultValue = ctx.requireArg<std::string>(3);
    const auto index = ctx.getArg<int>(4); // the index defaults to 0

    return ctx.to_nasal(
        tr.getPluralWithDefault(cardinalNumber, std::move(resource),
                                std::move(basicId), std::move(defaultValue),
                                index));
}

static naRef f_translationUnit(const FGTranslate& tr,
                               const nasal::CallContext& ctx)
{
    if (ctx.argc < 2 || ctx.argc > 3) {
        ctx.runtimeError(
            "FGTranslate.translationUnit(resource, basicId[, index])");
    }

    const auto resource = ctx.requireArg<std::string>(0);
    const auto basicId = ctx.requireArg<std::string>(1);
    const auto index = ctx.getArg<int>(2); // the index defaults to 0

    return ctx.to_nasal(
        tr.translationUnit(std::move(resource), std::move(basicId), index));
}

// Static member function
void FGTranslate::setupGhost()
{
    using FGTranslateRef = std::shared_ptr<FGTranslate>;
    using NasalFGTranslate = nasal::Ghost<FGTranslateRef>;

    NasalFGTranslate::init("FGTranslate")
        .method("get", &f_get)
        .method("getPlural", &f_getPlural)
        .method("getWithDefault", &f_getWithDefault)
        .method("getPluralWithDefault", &f_getPluralWithDefault)
        .method("getAll", &FGTranslate::getAll)
        .method("getCount", &FGTranslate::getCount)
        .method("translationUnit", &f_translationUnit);
}
