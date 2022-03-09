// StartupWindow.hxx - show a window during startup
// Copyright (C) 2022 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

namespace flightgear
{

// forward decls
class StartupSystemAdapter;

/**
 * @brief Show + update a minimal OSG window
 * 
 * This builds up the required OSG structures
 * but not the FG rendering / viewer pieces
 */
class StartupWindow
{
public:
    StartupWindow();
    ~StartupWindow();
    
    void init();
    void shutdown();

    void createSubsystems();
    void createWindow();
    
    void frame();

    bool isDone() const;
private:
    friend class StartupSystemAdapter;
    class StartupWindowPrivate;

    std::unique_ptr<StartupWindowPrivate> d;
    
    void setNasalModules();
};

/**
 * @brief create, show, run and then destroy
 * the startup window.
 */
void runStartupWindow();

} // of namespace

