// QtFileDialog.hxx - Qt5 implementation of FGFileDialog
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2015 Rebecca Palmer <rebecca_palmer@zoho.com>

#pragma once

#include <GUI/FileDialog.hxx>

class QtFileDialog : public FGFileDialog
{
public:
    QtFileDialog(FGFileDialog::Usage use);

    virtual ~QtFileDialog();
    
    void exec() override;
    void close() override;
};

