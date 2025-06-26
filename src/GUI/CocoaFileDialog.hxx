// CocoaFileDialog.h - Cocoa implementation of file-dialog interface
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2013  James Turner - james@flightgear.org

#pragma once

#include <GUI/FileDialog.hxx>

class CocoaFileDialog : public FGFileDialog
{
public:
    CocoaFileDialog(FGFileDialog::Usage use);
    
    virtual ~CocoaFileDialog();

    void exec() override;
    void close() override;

private:
    class CocoaFileDialogPrivate;
    std::unique_ptr<CocoaFileDialogPrivate> d;
};
