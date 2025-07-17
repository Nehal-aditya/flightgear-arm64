// FileDialog.cxx - generic FileDialog interface and Nasal wrapper
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2012  James Turner - james@flightgear.org


#include "config.h"
#include "simgear/debug/debug_types.h"
#include "simgear/structure/exception.hxx"

#include "FileDialog.hxx"

#include <simgear/nasal/cppbind/Ghost.hxx>

#include <Main/globals.hxx>
#include <Scripting/NasalSGPath.hxx>
#include <Scripting/NasalSys.hxx>

#if defined(SG_MAC)
#include "CocoaFileDialog.hxx"
#endif

#if defined(HAVE_QT)
#include "QtFileDialog.hxx"
#endif

FGFileDialog::FGFileDialog(Usage use) : _usage(use),
                                        _showHidden(false)
{
}

FGFileDialog::~FGFileDialog()
{
    // ensure this is concrete so callback gets cleaned up.
}

void FGFileDialog::setTitle(const std::string& aText)
{
    _title = aText;
}

void FGFileDialog::setButton(const std::string& aText)
{
    _buttonText = aText;
}

void FGFileDialog::setDirectory(const SGPath& aPath)
{
    _initialPath = aPath;
}

void FGFileDialog::setStandardLocation(const std::string& s)
{
    try {
        const auto l = standardLocationFromString(s);
        _initialPath = SGPath::standardLocation(l);
    } catch (sg_exception&) {
        SG_LOG(SG_GUI, SG_DEV_ALERT, "FileDialog was requested to use invalid standard location: " + s);
    }
}

void FGFileDialog::setFilterPatterns(const string_list& patterns)
{
    _filterPatterns = patterns;
}

void FGFileDialog::setPlaceholderName(const std::string& aName)
{
    _placeholder = aName;
}

void FGFileDialog::setCallback(Callback* aCB)
{
    _callback.reset(aCB);
}

void FGFileDialog::setShowHidden(bool show)
{
    _showHidden = show;
}

bool FGFileDialog::handleSelectedPath(const SGPath& p)
{
    // mark the path as allowed. For CHOOSE_DIR we will get read permissions as well
    const auto perm = (_usage == USE_SAVE_FILE) ? SGPath::Permissions{false, true} : SGPath::Permissions{true, false};

    bool ok = false;
    const auto pathString = p.realpath().utf8Str();
    if (_usage == USE_CHOOSE_DIR) {
        ok = SGPath::addAllowedDirectoryHierarchy(pathString, perm);
    } else {
        ok = SGPath::addAllowedPath(pathString, perm);
    }

    if (!ok) {
        SG_LOG(SG_IO, SG_POPUP, "The selected location '" << p.utf8Str() << "' is not allowed. (The location was resolved to a path that contains characters such as '*')");
        return false;
    }

    _callback->onFileDialogDone(this, p);
    return true;
}

class NasalCallback : public FGFileDialog::Callback
{
public:
    NasalCallback(naRef f, naRef obj) : func(f),
                                        object(obj)
    {
        auto sys = globals->get_subsystem<FGNasalSys>();
        _gcKeys[0] = sys->gcSave(f);
        _gcKeys[1] = sys->gcSave(obj);
    }

    void onFileDialogDone(FGFileDialog* instance, const SGPath& aPath) override
    {
        auto sys = globals->get_subsystem<FGNasalSys>();

        naContext ctx = naNewContext();
        naRef args[1];
        args[0] = nasal::to_nasal(ctx, aPath);

        sys->callMethod(func, object, 1, args, naNil() /* locals */);
        naFreeContext(ctx);
    }

    ~NasalCallback()
    {
        auto sys = globals->get_subsystem<FGNasalSys>();
        if (!sys) // happens during Nasal shutdown on reset
            return;

        sys->gcRelease(_gcKeys[0]);
        sys->gcRelease(_gcKeys[1]);
    }

private:
    naRef func;
    naRef object;
    int _gcKeys[2];
};

void FGFileDialog::setCallbackFromNasal(const nasal::CallContext& ctx)
{
    // wrap up the naFunc in our callback type
    naRef func = ctx.requireArg<naRef>(0);
    naRef object = ctx.getArg<naRef>(1, naNil());

    setCallback(new NasalCallback(func, object));
}

typedef std::shared_ptr<FGFileDialog> FileDialogPtr;
typedef nasal::Ghost<FileDialogPtr> NasalFileDialog;

/**
 * Create new FGFileDialog and get ghost for it.
 */
static naRef f_createFileDialog(const nasal::CallContext& ctx)
{
    FGFileDialog::Usage usage = (FGFileDialog::Usage)ctx.requireArg<int>(0);

#if defined(SG_MAC)
    FileDialogPtr fd(new CocoaFileDialog(usage));
#elif defined(HAVE_QT)
    FileDialogPtr fd(new QtFileDialog(usage));
#else
    // we need a fallback implementation
    FileDialogPtr fd;
#endif

    return ctx.to_nasal(fd);
}

void postinitNasalGUI(naRef globals, naContext c)
{
    NasalFileDialog::init("gui._FileDialog")
        .member("title", &FGFileDialog::getTitle, &FGFileDialog::setTitle)
        .member("button", &FGFileDialog::getButton, &FGFileDialog::setButton)
        .member("location", &FGFileDialog::setStandardLocation)
        .member("directory", &FGFileDialog::getDirectory, &FGFileDialog::setDirectory)
        .member("show_hidden", &FGFileDialog::showHidden, &FGFileDialog::setShowHidden)
        .member("placeholder", &FGFileDialog::getPlaceholder, &FGFileDialog::setPlaceholderName)
        .member("pattern", &FGFileDialog::filterPatterns, &FGFileDialog::setFilterPatterns)
        .method("open", &FGFileDialog::exec)
        .method("close", &FGFileDialog::close)
        .method("setCallback", &FGFileDialog::setCallbackFromNasal);

    nasal::Hash guiModule = nasal::Hash(globals, c).get<nasal::Hash>("gui");

    guiModule.set("FILE_DIALOG_OPEN_FILE", (int)FGFileDialog::USE_OPEN_FILE);
    guiModule.set("FILE_DIALOG_SAVE_FILE", (int)FGFileDialog::USE_SAVE_FILE);
    guiModule.set("FILE_DIALOG_CHOOSE_DIR", (int)FGFileDialog::USE_CHOOSE_DIR);
    guiModule.set("_createFileDialog", f_createFileDialog);
}
