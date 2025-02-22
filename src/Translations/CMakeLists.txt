# SPDX-FileCopyrightText: 2025 Florent Rougon
# SPDX-License-Identifier: GPL-2.0-or-later

include(FlightGearComponent)

set(SOURCES DefaultTranslationParser.cxx
            FGTranslate.cxx
            LanguageInfo.cxx
            TranslationDomain.cxx
            TranslationResource.cxx
            TranslationUnit.cxx
            XLIFFParser.cxx
            )

set(HEADERS DefaultTranslationParser.hxx
            FGTranslate.hxx
            LanguageInfo.hxx
            TranslationDomain.hxx
            TranslationResource.hxx
            TranslationUnit.hxx
            XLIFFParser.hxx
            )

flightgear_component(Translations "${SOURCES}" "${HEADERS}")
