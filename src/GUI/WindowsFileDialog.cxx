// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2013 James Turner <james@flightgear.org>

#include "WindowsFileDialog.hxx"

#include <windows.h>
#include <Shlobj.h>

#include <osgViewer/Viewer>
#include <osgViewer/api/Win32/GraphicsWindowWin32>

#include <simgear/debug/logstream.hxx>
#include <simgear/misc/strutils.hxx>

#include <Main/globals.hxx>
#include <Main/fg_props.hxx>
#include <Viewer/renderer.hxx>

namespace {

HWND getMainViewerHWND()
{
	osgViewer::Viewer::Windows windows;
	if (!globals->get_renderer() || !globals->get_renderer()->getViewerBase()) {
		return 0;
	}

    globals->get_renderer()->getViewerBase()->getWindows(windows);
    osgViewer::Viewer::Windows::const_iterator it = windows.begin();
    for(; it != windows.end(); ++it) {
        if (strcmp((*it)->className(), "GraphicsWindowWin32")) {
            continue;
        }

        osgViewer::GraphicsWindowWin32* platformWin =
            static_cast<osgViewer::GraphicsWindowWin32*>(*it);
        return platformWin->getHWND();
    }

    return 0;
}

static int CALLBACK BrowseFolderCallback(
                  HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED) {
		// set the initial directory now
		WindowsFileDialog* dlg = reinterpret_cast<WindowsFileDialog*>(lpData);
		const auto w = dlg->getDirectory().wstr();
        ::SendMessageW(hwnd, BFFM_SETSELECTIONW, true, (LPARAM) w.c_str());
    }
    return 0;
}

} // of anonymous namespace

WindowsFileDialog::WindowsFileDialog(FGFileDialog::Usage use) :
    FGFileDialog(use)
{

}

WindowsFileDialog::~WindowsFileDialog()
{

}

void WindowsFileDialog::exec()
{
    const std::wstring wtitle = simgear::strutils::convertUtf8ToWString(_title);
    wchar_t Filestring[MAX_PATH];
    Filestring[0] = 0;

    OPENFILENAMEW opf = {0};
    opf.lStructSize = sizeof(OPENFILENAME);
    opf.lpstrFile = Filestring;
    opf.lpstrTitle = wtitle.c_str();
    opf.nMaxFile = MAX_PATH;

    std::wstring wideExtensions;

    if (!_filterPatterns.empty()) {
        std::string extensions;
        for (const auto& ext : _filterPatterns) {
            if (!simgear::strutils::starts_with(ext, "*.")) {
                SG_LOG(SG_GENERAL, SG_ALERT, "WindowsFileDialog: can't use pattern on Windows:" << ext);
                continue;
            }
            extensions += "(" + ext + ")\0" + ext + "\0";
        }
        wideExtensions = simgear::strutils::convertUtf8ToWString(extensions);
        opf.lpstrFilter = wideExtensions.c_str();
    }

    std::wstring w = _initialPath.wstr();
    opf.lpstrInitialDir = w.c_str();

    if (_showHidden) {
        opf.Flags = OFN_PATHMUSTEXIST;
    }

    if (_usage == USE_SAVE_FILE) {
        if (GetSaveFileNameW(&opf)) {
            std::wstring stringPath(opf.lpstrFile);
            handleSelectedPath(SGPath(stringPath));
        }
    } else if (_usage == USE_CHOOSE_DIR) {
        chooseDir();
    } else {
        if (GetOpenFileNameW(&opf)) {
            std::wstring stringPath(opf.lpstrFile);
            handleSelectedPath(SGPath(stringPath));
        }
    }
}

void WindowsFileDialog::close()
{

}

void WindowsFileDialog::chooseDir()
{
	// MSDN says this needs to be called first
	OleInitialize(NULL);

    wchar_t pathBuf[MAX_PATH];
    pathBuf[0] = 0;

    const std::wstring wtitle = simgear::strutils::convertUtf8ToWString(_title);
    BROWSEINFOW binfo;
    memset(&binfo, 0, sizeof(BROWSEINFOW));
    binfo.hwndOwner = getMainViewerHWND();
	binfo.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;

	binfo.pidlRoot = NULL; // can browse anywhere
    binfo.lpszTitle = wtitle.c_str();
    binfo.lpfn = BrowseFolderCallback;
    binfo.lParam = reinterpret_cast<LPARAM>(this);

    PIDLIST_ABSOLUTE results = SHBrowseForFolderW(&binfo);
    if (results == NULL) {
		// user cancelled
		return;
	}

    SHGetPathFromIDListW(results, pathBuf);
    CoTaskMemFree(results);

    handleSelectedPath(SGPath(std::wstring(pathBuf)));
}
