// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Parse a FlightGear default translation file (e.g., menu.xml)
 */

#pragma once

#include <string>

#include <simgear/xml/easyxml.hxx>

#include "TranslationResource.hxx"

namespace flightgear
{

/**
 * @brief Class for parsing a FlightGear default translation file (e.g.,
 *        menu.xml)
 *
 * This class implements a parser for XML translation files normally located
 * under ⟨base-dir⟩/Translations/default, where ⟨base-dir⟩ can be $FG_ROOT or
 * the top-level directory of an aircraft or add-on.
 */

class DefaultTranslationParser : public XMLVisitor
{
public:
    DefaultTranslationParser(TranslationResource* resource);

protected:
    void startXML () override;
    void endXML   () override;
    void startElement (const char * name, const XMLAttributes &atts) override;
    void endElement (const char * name) override;
    void data (const char * s, int len) override;
    void warning (const char * message, int line, int column) override;

private:
    /**
     * @brief Convert a string such as an attribute value to a boolean.
     *
     * @param  str  string to convert
     * @return The boolean
     *
     * This is used for the 'has-plural' attribute.
     */
    bool asBoolean(const std::string& str);
    [[noreturn]] void parseError(const std::string& message);
    /// Called when an element starts that is inside <meta>...</meta>
    void startElementInsideMeta(const std::string& name);
    /// Check if we found supported 'file-type' and 'format-version' values
    void checkIfFormatIsSupported();

    /// Points to a container for the translation units
    TranslationResource* _resource;
    std::string _text;            ///< Contents-so-far of the current XML element
    std::string _stringTagName;   ///< Tag name of a translatable string
    bool _hasPlural = false;      ///< Whether the string has plural forms
    /**
     * @brief Number of translatable strings found with a given _stringTagName,
     *        at any time.
     *
     * Values in this map indicate the index to assign to the next
     * element (translatable string) whose tag name is the key.
     */
    std::map<std::string, int> _nextIndex;

    // Our parser is a state machine
    enum class State {
        LOOKING_FOR_RESOURCE_ELEMENT, // this is the root element
        LOOKING_FOR_META_ELEMENT,
        READING_META_ELEMENT,
        READING_FILE_TYPE_ELEMENT,
        READING_FORMAT_VERSION_ELEMENT,
        LOOKING_FOR_STRINGS_ELEMENT,
        READING_STRINGS_ELEMENT,
        READING_TRANSLATABLE_STRING,
        AFTER_STRINGS_ELEMENT,
    };

    State _state = State::LOOKING_FOR_RESOURCE_ELEMENT;

    /// Information on the file format, read from its <meta> element
    std::string _fileType;
    bool _foundFileType = false;
    std::string _formatVersion;
    bool _foundFormatVersion = false;
};

} // namespace flightgear
