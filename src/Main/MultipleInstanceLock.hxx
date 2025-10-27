// SPDX-FileCopyrightText: 2025 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <optional>
#include <string>

#include <simgear/misc/sg_path.hxx>

namespace flightgear {

/**
    * @brief RAII object to ensure only a single instance of FG is running
    * a particular section of code.
    *
    */
class ExclusiveInstanceLock
{
public:
    static ExclusiveInstanceLock* instance();

    static ExclusiveInstanceLock* createInstance(const std::string& reason);

    static void destroyInstance();

    ~ExclusiveInstanceLock();

    // @brief update the reason code, without releasing the lock
    void updateReason(const std::string& reason);

    /** check if another instance of FG is holding the exclusive lock */
    static std::optional<std::string> isLocked();

    /**
     * @brief show a dialog telling the user to wait until the exclusive lock is
     * released by another copy of FG
     *
     */
    static bool showWaitDialog();

private:
    ExclusiveInstanceLock(std::string reason);

    std::string _reason;
    SGPath _lockPath;
};


} // namespace flightgear
