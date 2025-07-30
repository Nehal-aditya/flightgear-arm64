/*
 *
 * SPDX-FileCopyrightText: 2021 James Turner
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <memory>

#include <simgear/structure/subsystem_mgr.hxx>

namespace flightgear {

class ErrorReporter : public SGSubsystem
{
public:
    ErrorReporter();
    ~ErrorReporter();

    void preinit();

    void init() override;
    void bind() override;
    void unbind() override;

    void update(double dt) override;

    void shutdown() override;

    static const char* staticSubsystemClassId() { return "error-reporting"; }

    static std::string threadSpecificContextValue(const std::string& key);

private:
    class ErrorReporterPrivate;

    std::unique_ptr<ErrorReporterPrivate> d;
};

} // namespace flightgear
