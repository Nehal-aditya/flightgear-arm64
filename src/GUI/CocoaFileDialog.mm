// CocoaFileDialog.mm - Cocoa implementation of file-dialog interface
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2013  James Turner - james@flightgear.org

#include "CocoaFileDialog.hxx"
#include "GUI/FileDialog.hxx"
#include "simgear/debug/debug_types.h"

#include <AppKit/NSSavePanel.h>
#include <AppKit/NSOpenPanel.h>

#include <osgViewer/Viewer>
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>

#include <simgear/debug/logstream.hxx>
#include <simgear/misc/strutils.hxx>

#include <GUI/CocoaHelpers_private.h>
#include <Main/globals.hxx>
#include <Main/fg_props.hxx>
#include <Viewer/renderer.hxx>

class CocoaFileDialog::CocoaFileDialogPrivate
{
public:
    CocoaFileDialogPrivate() :
        panel(nil)
    {

    }

    ~CocoaFileDialogPrivate()
    {
        [panel release];
    }

    NSSavePanel* panel;
};

CocoaFileDialog::CocoaFileDialog(FGFileDialog::Usage use) :
    FGFileDialog(use)
{
    d.reset(new CocoaFileDialogPrivate);
    if (use == USE_SAVE_FILE) {
        d->panel = [NSSavePanel savePanel];
    } else {
        NSOpenPanel* openPanel = [NSOpenPanel openPanel];
        d->panel = openPanel;

        if (use == USE_CHOOSE_DIR) {
            [openPanel setCanChooseDirectories:YES];
        }
    } // of USE_OPEN_FILE or USE_CHOOSE_DIR -> building NSOpenPanel

    [d->panel retain];
}

CocoaFileDialog::~CocoaFileDialog()
{

}

void CocoaFileDialog::exec()
{
// find the native Cocoa NSWindow handle so we can parent the dialog and show
// it window-modal.
    NSWindow* cocoaWindow = nil;
    std::vector<osgViewer::GraphicsWindow*> windows;
    globals->get_renderer()->getViewerBase()->getWindows(windows);

    for (auto gw : windows) {
        // OSG doesn't use RTTI, so no dynamic cast. Let's check the class type
        // using OSG's own system, before we blindly static_cast<> and break
        // everything.
        if (strcmp(gw->className(), "GraphicsWindowCocoa")) {
            continue;
        }

        osgViewer::GraphicsWindowCocoa* gwCocoa = static_cast<osgViewer::GraphicsWindowCocoa*>(gw);
        cocoaWindow = (NSWindow*) gwCocoa->getWindow();
        break;
    }

// setup the panel fields now we have collected all the data
    if (_usage == USE_SAVE_FILE) {
        [d->panel setNameFieldStringValue:stdStringToCocoa(_placeholder)];
    }

    if (_filterPatterns.empty()) {
        [d->panel setAllowedFileTypes:nil];
    } else {
        NSMutableArray* extensions = [NSMutableArray arrayWithCapacity:0];
        for (const auto& ext : _filterPatterns) {
            if (!simgear::strutils::starts_with(ext, "*.")) {
                SG_LOG(SG_GENERAL, SG_INFO, "can't use pattern on Cococa:" << ext);
                continue;
            }
            [extensions addObject:stdStringToCocoa(ext.substr(2))];
        }

        [d->panel setAllowedFileTypes:extensions];
    }

    [d->panel setTitle:stdStringToCocoa(_title)];
    if (_showHidden) {
        [d->panel setShowsHiddenFiles:YES];
    }

    [d->panel setDirectoryURL: pathToNSURL(_initialPath)];

    [d->panel beginSheetModalForWindow:cocoaWindow completionHandler:^(NSInteger result)
    {
        if (result == NSModalResponseOK) {
          NSString *nspath = [[d->panel URL] path];
          // NSLog(@"the URL is: %@", d->panel URL]);
          auto p = SGPath::fromUtf8([nspath UTF8String]);
          handleSelectedPath(p);
        }
    }];
}

void CocoaFileDialog::close()
{
    [d->panel close];
}
