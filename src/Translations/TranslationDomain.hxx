// SPDX-FileCopyrightText: 2025 Florent Rougon
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Container for all TranslationResource's belonging to a domain
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include "TranslationResource.hxx"

namespace flightgear
{

/**
 * @brief Class that holds translation resources within a domain
 *
 * This class is essentially a map from resource names (strings) to
 * instances of std::shared_ptr<TranslationResource>.
 */

class TranslationDomain
{
public:
    using ResourceRef = std::shared_ptr<TranslationResource>;
    /**
     * @brief Get the specified TranslationResource instance.
     *
     * Create, insert and return an empty one if there is no such resource yet.
     */
    ResourceRef getOrCreateResource(const std::string& resourceName);
    /**
     * @brief Get the specified TranslationResource instance.
     *
     * Return an empty shared pointer if there is no such resource yet.
     */
    ResourceRef getResource(const std::string& resourceName) const;

private:
    std::map<std::string, ResourceRef> _map;
};

} // namespace flightgear
