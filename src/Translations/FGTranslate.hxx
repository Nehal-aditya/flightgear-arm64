// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Class for retrieving translated strings
 */

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "LanguageInfo.hxx"
#include "TranslationDomain.hxx"

namespace flightgear
{
/**
 * @brief Class for retrieving translated strings
 *
 * This class has member functions that can be chained in order to set
 * options: these are setDomain(), setIndex() and setCardinalNumber().
 * The other member functions return what is asked for (a translation, a
 * vector of translations sharing the same id, the number of translated
 * strings sharing a specified id).
 *
 * The defaut domain is “core”; it corresponds to translations defined in
 * FGData. Other domains are “current-aircraft” and “addons/⟨addonId⟩”.
 *
 * The default index is 0, meaning the first translatable string having
 * the specified id (in the domain and resource as per setDomain() and the
 * @a resource argument).
 *
 * If the translatable string specified by the (domain, resource, id)
 * tuple doesn't have has-plural="true" in the default translation, it is
 * a string with no plural forms. setCardinalNumber() should not be called
 * for such strings. On the other hand, if a translatable string is
 * defined with has-plural="true" in the default translation, it has
 * plural forms and setCardinalNumber() must be called on the FGTranslate
 * instance before retrieving data concerning the string, i.e. before
 * using get(), getWithDefault(), getAll() or getCount().
 *
 * Examples:
 *
 * Retrieve the translation of a string defined in FGData that has no
 * plural forms (domain = "core", resource = "options", id = "usage",
 * index = 0):
 * @code
 * std::string t = FGTranslate().get("options", "usage");
 * @endcode
 *
 * Similar thing but using the second string (index 1) having the id
 * "fg-scenery-desc":
 * @code
 * std::string t = FGTranslate().setIndex(1).get("options",
 *                                               "fg-scenery-desc");
 * @endcode
 *
 * Similar thing but from the Skeleton add-on:
 * @code
 * std::string t =
 *   FGTranslate().setDomain("addons/org.flightgear.addons.Skeleton")
 *                .setIndex(1)
 *                .get("some-resource", "some-id");
 * @endcode
 *
 * Retrieve a translation with plural forms defined in the current
 * aircraft. Let's assume the translation depends on a number of
 * liveries present in the @p nbLiveries variable.
 * @code
 * std::string t = FGTranslate().setDomain("current-aircraft")
 *                              .setCardinalNumber(nbLiveries)
 *                              .get("some-resource", "some-id");
 * @endcode
 */

class FGTranslate
{
public:
    // I did this to avoid making both LanguageInfo and FGTranslate class
    // templates...
    using intType = LanguageInfo::intType;

    /**
     * @brief Set the domain from which to retrieve translations.
     *
     * @param domain  a string such as “core”, “current-aircraft” or
     *                “addons/⟨addonId⟩”
     *
     * Newly-created instances of this class have their domain set to “core”.
     */
    FGTranslate& setDomain(std::string domain);
    /**
     * @brief Set the element (string) index used by get() and getWithDefault().
     *
     * @param index  an integer corresponding to the index of an XML element
     *
     * In the default translation, the first element with a given tag name is
     * assigned index 0, the second element with the same tag name is assigned
     * index 1, etc. In most cases, there is only one element with a given tag
     * name in the default translation, so the index is 0 in newly-created
     * instances of this class.
     */
    FGTranslate& setIndex(int index);
    /**
     * @brief Set the number of (items, etc.) that determines the plural form.
     *
     * @param number  an integer that correponds to a number of “things”
     *                (concrete or abstract)
     *
     * If you're using a translatable string such as `Found %1 file(s)`,
     * call this method with the actual number of files. This way, the
     * appropriate form (singular, plural, etc.) in the target language can be
     * used.
     *
     * Translatable strings with plural forms must be marked with the
     * `has-plural="true"` attribute in the default translation.
     * setCardinalNumber() should be called only when using these strings.
     */
    FGTranslate& setCardinalNumber(intType number);

    /**
     * @brief Get a single translation.
     *
     * @param resource  resource name, aka “context” (such as "atc", "menu",
     *                  "sys", etc.) the translatable string belongs to
     * @param basicId   name of the XML element corresponding to the
     *                  translation to retrieve in the default translation
     *                  file for the specified resource
     * @return The translated string
     *
     * The translation is fetched from the domain specified with setDomain()
     * (the domain in newly-created instances is `core`).
     *
     * There may be several elements named @a basicId in the default
     * translation file for the specified resource; these elements are
     * distinguished by their index. Newly-created instances of this class use
     * index 0; call setIndex() before get() in order to retrieve the
     * translation of a string whose with a non-zero index.
     */
    std::string get(const std::string& resource,
                    const std::string& basicId);
    /**
     * @brief Get a single translation, with default for missing or empty strings
     *
     * @param resource      same as for get()
     * @param basicId       same as for get()
     * @param defaultValue  returned if the string can't be found or is
     *                      declared with an empty source text in the
     *                      default translation
     */
    std::string getWithDefault(const std::string& resource,
                               const std::string& basicId,
                               const std::string& defaultValue);
    /**
     * @brief Get all translations associated to an id (tag name).
     *
     * @param resource      same as for get()
     * @param basicId       same as for get()
     * @return A vector containing all translations with id @a basicId in the
     *         specified resource
     */
    std::vector<std::string> getAll(const std::string& resource,
                                    const std::string& basicId);
    /**
     * @brief Get the number of translatable strings with a given id (tag name).
     *
     * @param resource      same as for get()
     * @param basicId       same as for get()
     * @return The number of elements named @a basicId in the specified resource
     *         (this is the size of the vector that getAll() would return)
     */
    std::size_t getCount(const std::string& resource,
                         const std::string& basicId);

private:
    /**
     * @brief Get the specified resource.
     *
     * @param resourceName  name of the resource
     * @return A pointer to the resource
     *
     * This function logs warnings if the domain or resource can't be found.
     */
    TranslationDomain::ResourceRef getResource(const std::string& resourceName)
        const;

    std::string _domain = "core";
    int _elementIndex = 0; ///< for sibling elements with the same name
    /// Determines which plural form will be used; it has no value initially.
    std::optional<intType> _cardinalNumber;
};

} // namespace flightgear
