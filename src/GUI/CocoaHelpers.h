// CocoaHelpers.h - C++ interface to Cocoa/AppKit helpers

// SPDX-FileCopyrightText: (C) 2013 James Turner <james@flightgear.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include <simgear/misc/sg_path.hxx>
/**
 * open a URL using the system's web-browser
 */
void cocoaOpenUrl(const std::string& url);

/**
 * Cocoa implementation so we can use NSURL
 */
SGPath platformDefaultDataPath();

/**
 * When we run non-bundled, we need to transform to a GUI (foreground) app
 * osgViewer does this for us normally, but we need to do it ourselves
 * to show a message box before OSG is initialized.
 */
void transformToForegroundApp();

/**
 * AppKit shuts us down via exit(), the code in main to cleanup is not run
 * in that scenario. Do some cleanup manually to avoid crashes on exit.
 */
void cocoaRegisterTerminateHandler();

/**
 * @brief helper to detect if we're running translocated or not.
 * Google 'Gatekeep app translation' for more info about this; basically it
 * happens when the user runs us directly from the DMG, and this makes
 * for very nasty file paths.
 */
bool cocoaIsRunningTranslocated();
