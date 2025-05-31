// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Translations: Nasal interface to FGTranslate and related classes
 */

#include <memory>
#include <string>

#include <simgear/nasal/cppbind/Ghost.hxx>
#include <simgear/nasal/cppbind/NasalCallContext.hxx>
#include <simgear/nasal/cppbind/NasalHash.hxx>
#include <simgear/nasal/nasal.h>

#include <Translations/FGTranslate.hxx>
#include <Translations/TranslationResource.hxx>
#include <Translations/TranslationUnit.hxx>

namespace flightgear
{

static naRef f_new_FGTranslate(const nasal::CallContext& ctx)
{
    using FGTranslateRef = std::shared_ptr<FGTranslate>;
    std::string domain;

    switch (ctx.argc) {
    case 0:
        return ctx.to_nasal(FGTranslateRef(new FGTranslate()));
    case 1:
        domain = ctx.requireArg<std::string>(0);
        return ctx.to_nasal(FGTranslateRef(new FGTranslate(std::move(domain))));
    default:
        ctx.runtimeError("FGTranslate.new() or FGTranslate.new(domain)");
    }

    return {};                  // unreachable
}

void initNasalTranslations(naRef globals, naContext c)
{
    nasal::Hash globalsModule(globals, c);
    nasal::Hash fgTranslateModule = globalsModule.createHash("FGTranslate");

    fgTranslateModule.set("new", &f_new_FGTranslate);

    FGTranslate::setupGhost();
    TranslationResource::setupGhost();
    TranslationUnit::setupGhost();
}

} // of namespace flightgear
