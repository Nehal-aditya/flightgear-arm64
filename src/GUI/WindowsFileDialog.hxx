// WindowsFileDialog.hxx - file dialog implemented using Windows
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2013 James Turner <james@flightgear.org>

#pragma once

#include <GUI/FileDialog.hxx>

class WindowsFileDialog : public FGFileDialog
{
public:
    WindowsFileDialog(FGFileDialog::Usage use);

    virtual ~WindowsFileDialog();

    virtual void exec();
    virtual void close();
private:
    void chooseDir();
};
