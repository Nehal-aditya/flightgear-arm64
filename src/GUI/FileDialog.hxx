// FileDialog.hxx - abstract interface for a file open/save dialog

// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2012  James Turner - james@flightgear.org

#pragma once

#include <memory> // for std::unique_ptr

#include <simgear/misc/sg_path.hxx>
#include <simgear/misc/strutils.hxx> // for string_list

#include <simgear/nasal/cppbind/NasalCallContext.hxx>

// forward decls
class SGPropertyNode;

class FGFileDialog
{
public:
    typedef enum {
        USE_OPEN_FILE = 0,
        USE_SAVE_FILE,
        USE_CHOOSE_DIR
    } Usage;

    std::string getTitle() const
    {
        return _title;
    }

    void setTitle(const std::string& aTitle);

    std::string getButton() const
    {
        return _buttonText;
    }

    void setButton(const std::string& aText);

    SGPath getDirectory() const
    {
        return _initialPath;
    }

    void setDirectory(const SGPath& aPath);

    string_list filterPatterns() const
    {
        return _filterPatterns;
    }

    void setFilterPatterns(const string_list& patterns);

    /// for saving
    std::string getPlaceholder() const
    {
        return _placeholder;
    }

    void setPlaceholderName(const std::string& aName);

    bool showHidden() const
    {
        return _showHidden;
    }
    void setShowHidden(bool show);

    /**
     * @brief Set the current directory of the file dialog based on a standard-location string.
     * E.g., `DESKTOP` or `DOCUMENTS`. This allows Nasal to request such a location without Nasal
     * itself having read/write access until the user picks a path (and hence it's added as allowed)
     *
     * @param name
     */
    void setStandardLocation(const std::string& name);


    /**
     * Destructor.
     */
    virtual ~FGFileDialog();

    virtual void exec() = 0;
    virtual void close() = 0;

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void onFileDialogDone(FGFileDialog* ins, const SGPath& result) = 0;
    };

    virtual void setCallback(Callback* aCB);

    void setCallbackFromNasal(const nasal::CallContext& ctx);

protected:
    FGFileDialog(Usage use);

    /**
     * @brief helper for derived classes, when a path is selected
     *
     * @param p - the SGPath which was selected
     * @return true if the path was handled ok, false if path was disallowed
     */
    bool handleSelectedPath(const SGPath& p);


    const Usage _usage;
    std::string _title, _buttonText;
    SGPath _initialPath;
    string_list _filterPatterns;
    std::string _placeholder;
    bool _showHidden;
    std::unique_ptr<Callback> _callback;
};
